// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Summon/ParadiseSummonPopup.h"
#include "UI/Panel/Summon/ParadiseSummonPanel.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

#pragma region 생명주기
void UParadiseSummonPopup::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 탭 버튼 바인딩
	if (Btn_Tab_Character)
	{
		Btn_Tab_Character->OnClicked.AddDynamic(this, &UParadiseSummonPopup::OnCharacterTabClicked);
	}
	if (Btn_Tab_Equipment)
	{
		Btn_Tab_Equipment->OnClicked.AddDynamic(this, &UParadiseSummonPopup::OnEquipmentTabClicked);
	}

	// 2. 뒤로가기 버튼 바인딩
	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &UParadiseSummonPopup::OnBackButtonClicked);
	}

	// 3. 초기 상태 설정 (캐릭터 탭 기본)
	SwitchTab(INDEX_CHARACTER);
}

void UParadiseSummonPopup::NativeDestruct()
{
	// 델리게이트 안전 해제
	if (Btn_Tab_Character) Btn_Tab_Character->OnClicked.RemoveAll(this);
	if (Btn_Tab_Equipment) Btn_Tab_Equipment->OnClicked.RemoveAll(this);
	if (Btn_Back) Btn_Back->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}
#pragma endregion 생명주기

#pragma region 내부 로직
void UParadiseSummonPopup::OnCharacterTabClicked()
{
	SwitchTab(INDEX_CHARACTER);
}

void UParadiseSummonPopup::OnEquipmentTabClicked()
{
	SwitchTab(INDEX_EQUIPMENT);
}

void UParadiseSummonPopup::OnBackButtonClicked()
{
	// 팝업 닫기 (부모 뷰포트나 스택에서 제거)
	RemoveFromParent();
	UE_LOG(LogTemp, Log, TEXT("[SummonPopup] 로비로 복귀합니다."));
}

void UParadiseSummonPopup::SwitchTab(int32 NewIndex)
{
	if (!Switcher_Content) return;

	// 1. 위젯 스위처 인덱스 변경
	Switcher_Content->SetActiveWidgetIndex(NewIndex);

	// 2. 해당 패널 데이터 갱신 (선택되었을 때만 데이터 로드 -> 최적화)
	if (NewIndex == INDEX_CHARACTER && Panel_Character)
	{
		Panel_Character->RefreshPanelData();
	}
	else if (NewIndex == INDEX_EQUIPMENT && Panel_Equipment)
	{
		Panel_Equipment->RefreshPanelData();
	}

	// 3. 버튼 스타일 업데이트 (선택된 탭 비활성화 등 시각적 피드백)
	if (Btn_Tab_Character) Btn_Tab_Character->SetIsEnabled(NewIndex != INDEX_CHARACTER);
	if (Btn_Tab_Equipment) Btn_Tab_Equipment->SetIsEnabled(NewIndex != INDEX_EQUIPMENT);

	UE_LOG(LogTemp, Log, TEXT("[SummonPopup] 탭 전환 완료: %d"), NewIndex);
}
#pragma endregion 내부 로직