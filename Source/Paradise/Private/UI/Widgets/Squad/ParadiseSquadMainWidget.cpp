// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Squad/ParadiseSquadMainWidget.h"
#include "UI/Widgets/Squad/Inventory/ParadiseSquadInventoryWidget.h"
#include "UI/Widgets/Squad/ParadiseSquadFormationWidget.h"
#include "UI/Widgets/Squad/ParadiseSquadDetailWidget.h"

#include "Framework/Core/ParadiseGameInstance.h"
#include "Framework/InGame/InGamePlayerState.h"
#include "Components/InventoryComponent.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"

#include "Data/Structs/UnitStructs.h"
#include "Data/Structs/ItemStructs.h"

#pragma region 생명주기
void UParadiseSquadMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. GameInstance 캐싱 (데이터 테이블 접근)
	CachedGI = Cast<UParadiseGameInstance>(GetGameInstance());
	if (!CachedGI.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadMain] GameInstance is invalid! Data loading will fail."));
	}

	// 2. Inventory Component 캐싱 (보유 데이터 접근)
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AInGamePlayerState* PS = PC->GetPlayerState<AInGamePlayerState>())
		{
			CachedInventory = PS->GetInventoryComponent();
		}
	}

	// 3. 탭 버튼 바인딩
	if (Btn_Tab_Character) Btn_Tab_Character->OnClicked.AddDynamic(this, &UParadiseSquadMainWidget::OnClickCharTab);
	if (Btn_Tab_Weapon)    Btn_Tab_Weapon->OnClicked.AddDynamic(this, &UParadiseSquadMainWidget::OnClickWpnTab);
	if (Btn_Tab_Armor)     Btn_Tab_Armor->OnClicked.AddDynamic(this, &UParadiseSquadMainWidget::OnClickArmTab);
	if (Btn_Tab_Unit)      Btn_Tab_Unit->OnClicked.AddDynamic(this, &UParadiseSquadMainWidget::OnClickUnitTab);

	// 4. 자식 위젯 이벤트 구독
	if (WBP_InventoryPanel)
	{
		WBP_InventoryPanel->OnItemClicked.AddDynamic(this, &UParadiseSquadMainWidget::HandleInventoryItemClicked);
	}

	if (WBP_FormationPanel)
	{
		WBP_FormationPanel->OnSlotSelected.AddDynamic(this, &UParadiseSquadMainWidget::HandleFormationSlotSelected);
	}

	if (WBP_DetailPanel)
	{
		WBP_DetailPanel->OnSwapEquipmentClicked.AddDynamic(this, &UParadiseSquadMainWidget::HandleSwapEquipmentMode);
		WBP_DetailPanel->OnCancelClicked.AddDynamic(this, &UParadiseSquadMainWidget::HandleCancelEquipMode);
	}

	// 5. 초기 상태 설정 (캐릭터 탭)
	SwitchTab(SquadTabs::Character);
}

void UParadiseSquadMainWidget::NativeDestruct()
{
	// 델리게이트 안전 해제
	if (Btn_Tab_Character) Btn_Tab_Character->OnClicked.RemoveAll(this);

	// 자식 위젯 델리게이트는 위젯 소멸 시 자동 해제되지만, 명시적 해제가 안전함
	if (WBP_InventoryPanel) WBP_InventoryPanel->OnItemClicked.RemoveAll(this);
	if (WBP_FormationPanel) WBP_FormationPanel->OnSlotSelected.RemoveAll(this);

	Super::NativeDestruct();
}
#pragma endregion 생명주기

#pragma region 로직 - 탭 및 상태 제어
void UParadiseSquadMainWidget::OnClickCharTab() { SwitchTab(SquadTabs::Character); }
void UParadiseSquadMainWidget::OnClickWpnTab() { SwitchTab(SquadTabs::Weapon); }
void UParadiseSquadMainWidget::OnClickArmTab() { SwitchTab(SquadTabs::Armor); }
void UParadiseSquadMainWidget::OnClickUnitTab() { SwitchTab(SquadTabs::Unit); }

void UParadiseSquadMainWidget::SwitchTab(int32 NewTab)
{
	// 같은 탭 재클릭 시 무시
	if (CurrentTabIndex == NewTab) return;

	// [Rule] 장비 교체 모드일 때는 캐릭터/유닛 탭으로 이동 불가 (UI 잠금)
	if (CurrentState == ESquadUIState::EquipMode)
	{
		if (NewTab == SquadTabs::Character || NewTab == SquadTabs::Unit) return;
	}

	CurrentTabIndex = NewTab;

	// 1. 좌측 Formation 패널 모드 변경 (유닛 탭일 때만 유닛 덱 모드)
	if (WBP_FormationPanel)
	{
		WBP_FormationPanel->SetFormationMode(CurrentTabIndex == SquadTabs::Unit);
	}

	// 2. 하단 Detail 패널 버튼 갱신 (유닛 탭은 장비 교체 버튼 숨김 등)
	if (WBP_DetailPanel)
	{
		WBP_DetailPanel->UpdateButtonState(CurrentState, CurrentTabIndex == SquadTabs::Unit);
	}

	// 3. 인벤토리 리스트 데이터 갱신
	RefreshInventoryUI();
}

void UParadiseSquadMainWidget::UpdateUIState()
{
	bool bIsNormal = (CurrentState == ESquadUIState::Normal);

	// 교체 모드 시 캐릭터/유닛 탭 비활성화 (시각적 처리)
	if (Btn_Tab_Character) Btn_Tab_Character->SetIsEnabled(bIsNormal);
	if (Btn_Tab_Unit)      Btn_Tab_Unit->SetIsEnabled(bIsNormal);

	// Detail 패널 버튼 모드 전환 (교체 -> 취소/완료)
	if (WBP_DetailPanel)
	{
		WBP_DetailPanel->UpdateButtonState(CurrentState, CurrentTabIndex == SquadTabs::Unit);
	}
}
#pragma endregion 로직 - 탭 및 상태 제어

#pragma region 로직 - 데이터 처리
void UParadiseSquadMainWidget::RefreshInventoryUI()
{
	if (!CachedInventory.IsValid() || !CachedGI.IsValid() || !WBP_InventoryPanel) return;

	TArray<FSquadItemUIData> ListData;

	// 현재 탭에 맞는 데이터를 인벤토리에서 가져와 UI 데이터로 가공
	switch (CurrentTabIndex)
	{
	case SquadTabs::Character:
		for (const auto& Data : CachedInventory->GetOwnedHeroes())
		{
			// GameInstance의 테이블 조회 로직 활용
			ListData.Add(MakeUIData(Data.CharacterID, Data.Level, SquadTabs::Character));
		}
		break;

	case SquadTabs::Weapon:
		for (const auto& Data : CachedInventory->GetOwnedItems())
		{
			// 무기 테이블에 존재하는 ID만 필터링하여 리스트에 추가
			if (CachedGI->GetDataTableRow<FWeaponStats>(CachedGI->WeaponStatsDataTable, Data.ItemID))
			{
				ListData.Add(MakeUIData(Data.ItemID, Data.EnhancementLevel, SquadTabs::Weapon));
			}
		}
		break;

	case SquadTabs::Armor:
		for (const auto& Data : CachedInventory->GetOwnedItems())
		{
			// 방어구 테이블에 존재하는 ID만 필터링
			if (CachedGI->GetDataTableRow<FArmorStats>(CachedGI->ArmorStatsDataTable, Data.ItemID))
			{
				ListData.Add(MakeUIData(Data.ItemID, Data.EnhancementLevel, SquadTabs::Armor));
			}
		}
		break;

	case SquadTabs::Unit:
		for (const auto& Data : CachedInventory->GetOwnedFamiliars())
		{
			ListData.Add(MakeUIData(Data.FamiliarID, Data.Level, SquadTabs::Unit));
		}
		break;
	}

	// 가공된 데이터를 뷰(Inventory Panel)에 전달
	WBP_InventoryPanel->UpdateList(CurrentTabIndex, ListData);
}

FSquadItemUIData UParadiseSquadMainWidget::MakeUIData(FName ID, int32 LevelOrCount, int32 TabType)
{
	FSquadItemUIData Result;
	Result.ID = ID;
	Result.LevelOrCount = LevelOrCount;
	Result.Name = FText::FromName(ID); // 기본값 (테이블 조회 실패 대비)

	if (!CachedGI.IsValid()) return Result;

	// GameInstance의 템플릿 함수(GetDataTableRow)를 사용하여 데이터 테이블 안전하게 조회
	if (TabType == SquadTabs::Character)
	{
		if (auto* Stat = CachedGI->GetDataTableRow<FCharacterStats>(CachedGI->CharacterStatsDataTable, ID))
		{
			// 실제 변수명(예: Rarity)을 확인하여 태그로 변환하거나, 변수명을 맞추세요.
			// 일단 컴파일을 위해 빈 태그로 둡니다.
			Result.RankTag = FGameplayTag::EmptyTag;
		}
		if (auto* Asset = CachedGI->GetDataTableRow<FCharacterAssets>(CachedGI->CharacterAssetsDataTable, ID))
		{
			Result.Icon = Asset->FaceIcon.LoadSynchronous();
		}
	}
	else if (TabType == SquadTabs::Weapon)
	{
		if (auto* Stat = CachedGI->GetDataTableRow<FWeaponStats>(CachedGI->WeaponStatsDataTable, ID))
		{
			Result.Name = Stat->DisplayName;
			// 여기도 마찬가지
			Result.RankTag = FGameplayTag::EmptyTag;
		}
		if (auto* Asset = CachedGI->GetDataTableRow<FWeaponAssets>(CachedGI->WeaponAssetsDataTable, ID))
		{
			Result.Icon = Asset->Icon.LoadSynchronous();
		}
	}
	else if (TabType == SquadTabs::Armor)
	{
		if (auto* Stat = CachedGI->GetDataTableRow<FArmorStats>(CachedGI->ArmorStatsDataTable, ID))
		{
			Result.Name = Stat->DisplayName;
			Result.RankTag = FGameplayTag::EmptyTag;
		}
		if (auto* Asset = CachedGI->GetDataTableRow<FArmorAssets>(CachedGI->ArmorAssetsDataTable, ID))
		{
			Result.Icon = Asset->Icon.LoadSynchronous();
		}
	}
	else if (TabType == SquadTabs::Unit)
	{
		if (auto* Stat = CachedGI->GetDataTableRow<FFamiliarStats>(CachedGI->FamiliarStatsDataTable, ID))
		{
			// 퍼밀리어는 구조체에 RankTypeTag가 있다면 주석 해제
			// Result.RankTag = Stat->RankTypeTag;
			Result.RankTag = FGameplayTag::EmptyTag;
		}
	}

	return Result;
}
#pragma endregion 로직 - 데이터 처리

#pragma region 로직 - 이벤트 핸들러
void UParadiseSquadMainWidget::HandleFormationSlotSelected(int32 SlotIndex)
{
	SelectedFormationSlotIndex = SlotIndex;

	// TODO: 실제로는 SquadComponent 등을 통해 해당 슬롯에 장착된 캐릭터 ID를 가져와야 함.
	// 현재는 슬롯 선택 시 Detail 패널의 버튼 상태를 갱신하는 로직 위주로 구현.

	// 임시 데이터로 상세창 갱신 테스트
	FSquadItemUIData DummyData;
	DummyData.Name = FText::FromString(TEXT("선택된 슬롯"));
	DummyData.LevelOrCount = SlotIndex;
	WBP_DetailPanel->ShowInfo(DummyData);

	// 버튼 상태 업데이트 (Unit 슬롯(3번 이상)인 경우 장비 교체 불가능)
	const bool bIsUnitSlot = (SlotIndex >= 3);
	WBP_DetailPanel->UpdateButtonState(CurrentState, bIsUnitSlot);
}

void UParadiseSquadMainWidget::HandleSwapEquipmentMode()
{
	// 1. 상태 변경 (장비 교체 모드)
	CurrentState = ESquadUIState::EquipMode;

	// 2. 무기 탭으로 강제 이동
	SwitchTab(SquadTabs::Weapon);

	// 3. UI 잠금 처리 및 버튼 변경
	UpdateUIState();

	UE_LOG(LogTemp, Log, TEXT("[SquadMain] Entered Equipment Swap Mode."));
}

void UParadiseSquadMainWidget::HandleCancelEquipMode()
{
	// 1. 상태 복구 (일반 모드)
	CurrentState = ESquadUIState::Normal;

	// 2. UI 잠금 해제 및 버튼 복구
	UpdateUIState();

	UE_LOG(LogTemp, Log, TEXT("[SquadMain] Exited Equipment Swap Mode."));
}

void UParadiseSquadMainWidget::HandleInventoryItemClicked(FSquadItemUIData ItemData)
{
	if (CurrentState == ESquadUIState::EquipMode)
	{
		// [장비 교체 로직]
		// EquipmentComponent->EquipItem(ItemData.ID) 등을 호출해야 함.
		UE_LOG(LogTemp, Log, TEXT("[SquadMain] Try Equip Item: %s to Slot: %d"), *ItemData.ID.ToString(), SelectedFormationSlotIndex);

		// 장착 후 모드 유지 (사용자가 완료 버튼을 누를 때까지)
	}
	else
	{
		// [일반 모드]
		if (CurrentTabIndex == SquadTabs::Character)
		{
			// 캐릭터 탭이면 캐릭터 교체 시도
			UE_LOG(LogTemp, Log, TEXT("[SquadMain] Try Swap Character: %s"), *ItemData.ID.ToString());
		}
		else
		{
			// 그 외에는 단순 정보 표시
			WBP_DetailPanel->ShowInfo(ItemData);
		}
	}
}
#pragma endregion 로직 - 이벤트 핸들러