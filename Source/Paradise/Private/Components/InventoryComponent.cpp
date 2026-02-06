// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Data/Structs/ItemStructs.h"
#include "Data/Structs/UnitStructs.h"
#include "Data/Structs/InventoryStruct.h"
#include "Framework/Core/ParadiseGameInstance.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInventoryComponent::InitInventory(const TArray<FOwnedCharacterData>& InHeroes, const TArray<FOwnedFamiliarData>& InFamiliars, const TArray<FOwnedItemData>& InItems)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;

	OwnedCharacters.Empty();
	OwnedFamiliars.Empty();
	OwnedItems.Empty();

	if (InHeroes.Num() > 0)
	{
		for (int i = 0; i < InHeroes.Num(); i++)
		{
			//캐릭터(영웅) 유효성 검사
			//Character 데이터테이블 조회
			if (GI->GetDataTableRow<FCharacterAssets>(GI->CharacterAssetsDataTable, InHeroes[i].CharacterID)) {
				OwnedCharacters.Add(InHeroes[i]);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
			}
		}
	}

	if (InFamiliars.Num() > 0)
	{
		for (int i = 0; i < InFamiliars.Num(); i++)
		{
			//퍼밀리어 유효성 검사
			//Familiar 데이터테이블 조회
			if (GI->GetDataTableRow<FFamiliarAssets>(GI->FamiliarAssetsDataTable, InFamiliars[i].FamiliarID)) {
				OwnedFamiliars.Add(InFamiliars[i]);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
			}	
		}

	}

	if (InItems.Num() > 0)
	{
		for (int i = 0; i < InItems.Num(); i++)
		{
			//아이템 유효성 검사
			//Weapon , Armor 데이터테이블 조회
			if (GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, InItems[i].ItemID)
				&& GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, InItems[i].ItemID)){
				OwnedItems.Add(InItems[i]);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
			}
		}
	}

	/*OwnedHeroes.Append(InHeroes);
	OwnedFamiliars.Append(InFamiliars);
	OwnedItems.Append(InItems);*/

	//인벤토리 변경 델리게이트 발송
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();	
	}

	UE_LOG(LogTemp, Log, TEXT("✅ 인벤토리 로드 완료 (영웅:%d, 병사:%d, 아이템:%d)"),
		OwnedCharacters.Num(), OwnedFamiliars.Num(), OwnedItems.Num());

	
}

void UInventoryComponent::AddItem(FName ItemID, int32 Count)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (ItemID.IsNone()|| Count<=0) return;

	FWeaponAssets* WeaponData  = GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, ItemID);
	FArmorAssets* ArmorData = GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, ItemID);

	if (!WeaponData&& !ArmorData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 아이템입니다."));
		return;
	}

	for (int i = 0; i < OwnedItems.Num(); i++)
	{
		if (OwnedItems[i].ItemID == ItemID)
		{
			OwnedItems[i].Quantity += Count;
			//이미 있는 아이템 //수량변동 //강화수치비교필요
			return;
		}
	}
	
	FOwnedItemData NewItem;
	NewItem.ItemID = ItemID;
	NewItem.EnhancementLevel = 1;
	NewItem.Quantity = Count;

	OwnedItems.Add(NewItem);

	//UI에 알림
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
	
}

void UInventoryComponent::AddCharacter(FName CharacterID)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (CharacterID.IsNone()) return;

	FCharacterAssets* CharacterData = GI->GetDataTableRow<FCharacterAssets>(GI->CharacterAssetsDataTable, CharacterID);
	if (!CharacterData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 캐릭터입니다."));
		return;
	}

	for (int i = 0; i < OwnedCharacters.Num(); i++)
	{
		if (OwnedCharacters[i].CharacterID == CharacterID)
		{
			//이미 있는 캐릭터 // 조각변환등 추가처리필요
			return;
		}
	}

	//중복이 없으므로 새 캐릭터로 추가
	FOwnedCharacterData NewCharacter;
	NewCharacter.CharacterID = CharacterID;
	NewCharacter.Level = 1;
	NewCharacter.AwakeningLevel = 1;

	OwnedCharacters.Add(NewCharacter);

	//UI에 알림
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
}

void UInventoryComponent::AddFamiliar(FName FamiliarID)
{
	UParadiseGameInstance* GI = GetParadiseGI();
	if (!GI) return;
	if (FamiliarID.IsNone()) return;

	FFamiliarAssets* FamiliarData = GI->GetDataTableRow<FFamiliarAssets>(GI->FamiliarAssetsDataTable, FamiliarID);

	if (!FamiliarData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent] 유효성 검사 : 데이터 테이블에 존재하지 않는 퍼밀리어입니다."));
		return;
	}

	for (int i = 0; i < OwnedFamiliars.Num(); i++)
	{
		if (OwnedFamiliars[i].FamiliarID == FamiliarID)
		{
			//이미 있는 퍼밀리어 // 조각변환등 추가처리필요
			return;
		}
	}
	
	FOwnedFamiliarData NewFamiliar;
	NewFamiliar.FamiliarID = FamiliarID;
	NewFamiliar.Level = 1;
	NewFamiliar.Quantity = 1;

	OwnedFamiliars.Add(NewFamiliar);

	//UI에 알림
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
}

void UInventoryComponent::Debug_TestInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("=========================================="));
	UE_LOG(LogTemp, Warning, TEXT("🧪 [Debug] 인벤토리 테스트 시작"));
	UE_LOG(LogTemp, Warning, TEXT("=========================================="));

	// 1. 테스트 데이터 추가 시도 (데이터 테이블에 실제 존재하는 ID를 넣어야 합니다!)
	// 예시 ID를 사용했으니, 본인 프로젝트 테이블에 있는 ID로 바꿔주세요.
	AddCharacter(FName("Character_Knight"));       // 성공 예상
	AddCharacter(FName("Character_Knight"));       // 중복 실패 예상 (이미 보유)

	AddFamiliar(FName("Fam_Skeleton"));
	AddFamiliar(FName("Fam_Skeleton")); 

	AddItem(FName("W_BasicSword"), 1);
	AddItem(FName("A_WoodHelmet"), 10);

	// 2. 전체 리스트 출력
	UE_LOG(LogTemp, Log, TEXT(" "));
	UE_LOG(LogTemp, Log, TEXT("📂 [인벤토리 현황 리포트]"));

	// [영웅 목록]
	UE_LOG(LogTemp, Log, TEXT("------------------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("1. 보유 영웅 (총 %d명)"), OwnedCharacters.Num());
	for (int32 i = 0; i < OwnedCharacters.Num(); ++i)
	{
		const auto& Data = OwnedCharacters[i];
		UE_LOG(LogTemp, Log, TEXT("   [%d] ID: %-15s | Lv: %d | Awa: %d"),
			i, *Data.CharacterID.ToString(), Data.Level, Data.AwakeningLevel);
	}

	// [퍼밀리어 목록]
	UE_LOG(LogTemp, Log, TEXT("------------------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("2. 보유 퍼밀리어 (총 %d종)"), OwnedFamiliars.Num());
	for (int32 i = 0; i < OwnedFamiliars.Num(); ++i)
	{
		const auto& Data = OwnedFamiliars[i];
		UE_LOG(LogTemp, Log, TEXT("   [%d] ID: %-15s | Lv: %d | Qty: %d"),
			i, *Data.FamiliarID.ToString(), Data.Level, Data.Quantity);
	}

	// [아이템 목록]
	UE_LOG(LogTemp, Log, TEXT("------------------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("3. 보유 아이템 (총 %d종)"), OwnedItems.Num());
	for (int32 i = 0; i < OwnedItems.Num(); ++i)
	{
		const auto& Data = OwnedItems[i];
		UE_LOG(LogTemp, Log, TEXT("   [%d] ID: %-15s | Qty: %d | Reinforce: +%d"),
			i, *Data.ItemID.ToString(), Data.Quantity, Data.EnhancementLevel);
	}

	UE_LOG(LogTemp, Warning, TEXT("=========================================="));
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Count)
{
	return false;
}




int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	return int32();
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

UParadiseGameInstance* UInventoryComponent::GetParadiseGI() const
{
	return Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
}




