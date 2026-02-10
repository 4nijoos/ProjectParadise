// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "Framework/Core/ParadiseGameInstance.h"
#include "Characters/Base/PlayerBase.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UEquipmentComponent::SetLinkedInventory(UInventoryComponent* InInventory)
{
    if (InInventory)
    {
        LinkedInventory = InInventory;
        UE_LOG(LogTemp, Log, TEXT("🔗 [Equipment] 인벤토리 연결 성공!"));
    }
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UEquipmentComponent::EquipItem(FGuid TargetItemUID)
{
    //[유효성 검사]
    if (!LinkedInventory)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ [Equip] 인벤토리 연결 안됨."));
        return;
    }
    if (!TargetItemUID.IsValid()) return;

    //[데이터 조회]
    FOwnedItemData* ItemData = LinkedInventory->GetItemByGUID(TargetItemUID);
    if (!ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ [Equip] 인벤토리에 없는 아이템 (UID: %s)"), *TargetItemUID.ToString());
        return;
    }

    //[슬롯 결정] 헬퍼 함수 호출로 로직 분리!
    EEquipmentSlot TargetSlot = FindEquipmentSlot(ItemData->ItemID);

    if (TargetSlot == EEquipmentSlot::Unknown)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ [Equip] 장착 불가(테이블/태그 오류): %s"), *ItemData->ItemID.ToString());
        return;
    }

    //[장착 실행]
    EquippedItems.Add(TargetSlot, TargetItemUID);

    UE_LOG(LogTemp, Log, TEXT("⚔️ [Equip] 성공! Slot: %d | Item: %s (+%d강)"),
        (int32)TargetSlot, *ItemData->ItemID.ToString(), ItemData->EnhancementLevel);

    //[갱신]
    if (APlayerBase* Player = Cast<APlayerBase>(GetOwner()))
    {
        UpdateVisuals(Player);
    }

    if (OnEquipmentUpdated.IsBound()) OnEquipmentUpdated.Broadcast();
}

void UEquipmentComponent::UnEquipItem(EEquipmentSlot Slot)
{
    if (EquippedItems.Remove(Slot) > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("🛡️ [UnEquip] 장착 해제: Slot %d"), (int32)Slot);

        // 비주얼 갱신
        if (APlayerBase* Player = Cast<APlayerBase>(GetOwner()))
        {
            UpdateVisuals(Player);
        }

        if (OnEquipmentUpdated.IsBound()) OnEquipmentUpdated.Broadcast();
    }
}

FName UEquipmentComponent::GetEquippedItemID(EEquipmentSlot Slot) const
{
    //해당 슬롯에 GUID가 없으면 None
    if (!EquippedItems.Contains(Slot)) return NAME_None;

    FGuid TargetUID = EquippedItems[Slot];

    //인벤토리가 없으면 조회를 못하므로 None
    if (!LinkedInventory) return NAME_None;

    //인벤토리에게 물어봐서 데이터 가져오기
    if (FOwnedItemData* ItemData = LinkedInventory->GetItemByGUID(TargetUID))
    {
        //인벤토리에서 찾아서 FName 반환
        return ItemData->ItemID;
    }

    return NAME_None;
}

bool UEquipmentComponent::GetEquippedItemData(EEquipmentSlot Slot, FOwnedItemData& OutData) const
{
    if (!LinkedInventory) return false;

    if (const FGuid* FoundGUID = EquippedItems.Find(Slot))
    {
        if (FOwnedItemData* RealData = LinkedInventory->GetItemByGUID(*FoundGUID))
        {
            OutData = *RealData;
            return true;
        }
    }
    return false;
}

void UEquipmentComponent::UpdateVisuals(APlayerBase* TargetCharacter)
{
	// 타겟이 없으면 컴포넌트 소유자를 사용
	APlayerBase* Char = TargetCharacter ? TargetCharacter : Cast<APlayerBase>(GetOwner());
	if (!Char || !LinkedInventory) return;

	UE_LOG(LogTemp, Log, TEXT("🎨 [Visual] 캐릭터 외형 업데이트 시작..."));

	//무기 처리 (Weapon Slot)
	FOwnedItemData WeaponData;
	if (GetEquippedItemData(EEquipmentSlot::Weapon, WeaponData))
	{
		// 무기 데이터가 있으면 액터 생성 및 부착
		AttachWeaponActor(Char, WeaponData.ItemID);
	}
	else
	{
		// 무기가 없으면 기존 액터 파괴
		if (SpawnedWeaponActor)
		{
			SpawnedWeaponActor->Destroy();
			SpawnedWeaponActor = nullptr;
		}
	}

	//방어구 처리 (Armor Slots)
	// (필요한 모든 방어구 슬롯을 순회합니다)
	const TArray<EEquipmentSlot> ArmorSlots = {
		EEquipmentSlot::Helmet,
		EEquipmentSlot::Chest,
		EEquipmentSlot::Gloves,
		EEquipmentSlot::Boots
	};

	for (EEquipmentSlot Slot : ArmorSlots)
	{
		FOwnedItemData ArmorData;
		// 해당 슬롯에 장착된 아이템이 있는지 확인
		if (GetEquippedItemData(Slot, ArmorData))
		{
			SetArmorMesh(Char, Slot, ArmorData.ItemID);
		}
		else
		{
			// 장착 해제 상태라면 빈 이름(NAME_None)을 전달하여 메쉬를 비움
			SetArmorMesh(Char, Slot, NAME_None);
		}
	}
}


EEquipmentSlot UEquipmentComponent::FindEquipmentSlot(FName ItemID) const
{
	if (ItemID.IsNone()) return EEquipmentSlot::Unknown;

	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI) return EEquipmentSlot::Unknown;

	//무기 테이블 확인
	if (GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, ItemID))
	{
		return EEquipmentSlot::Weapon;
	}

	//방어구 테이블 확인
	if (FArmorAssets* ArmorRow = GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, ItemID))
	{
		// 태그 비교 로직
		const FGameplayTag& Tag = ArmorRow->ArmorTag;

		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Type.Armor.Helmet"))) return EEquipmentSlot::Helmet;
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Type.Armor.Chest")))  return EEquipmentSlot::Chest;
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Type.Armor.Gloves"))) return EEquipmentSlot::Gloves;
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Type.Armor.Boots")))  return EEquipmentSlot::Boots;

		// 매칭되는 태그가 없으면 경고
		UE_LOG(LogTemp, Warning, TEXT("⚠️ [FindSlot] 알 수 없는 방어구 태그: %s"), *Tag.ToString());
	}

	return EEquipmentSlot::Unknown;
}

void UEquipmentComponent::AttachWeaponActor(APlayerBase* Char, FName ItemID)
{
	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI || !Char) return;

	//데이터 테이블 조회
	FWeaponAssets* WeaponAssets = GI->GetDataTableRow<FWeaponAssets>(GI->WeaponAssetsDataTable, ItemID);
	if (!WeaponAssets) return;

	//기존 무기 제거 (교체 시)
	if (SpawnedWeaponActor)
	{
		SpawnedWeaponActor->Destroy();
		SpawnedWeaponActor = nullptr;
	}

	//메쉬 로드 (SoftReference -> Hard Load)
	USkeletalMesh* LoadedMesh = WeaponAssets->ItemMesh.LoadSynchronous();
	if (!LoadedMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ [Visual] 무기 메쉬 로드 실패: %s"), *ItemID.ToString());
		return;
	}

	//무기 액터 스폰 (SkeletalMeshActor 사용)
	FActorSpawnParameters Params;
	Params.Owner = Char;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASkeletalMeshActor* NewWeapon = GetWorld()->SpawnActor<ASkeletalMeshActor>(
		ASkeletalMeshActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params);

	if (NewWeapon)
	{
		USkeletalMeshComponent* WeaponComp = NewWeapon->GetSkeletalMeshComponent();
		if (WeaponComp)
		{
			WeaponComp->SetSkeletalMesh(LoadedMesh);
			WeaponComp->SetMobility(EComponentMobility::Movable);
			WeaponComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 캐릭터 충돌 방지
		}

		// 소켓 부착
		// 데이터 테이블에 지정된 소켓이 있으면 사용, 없으면 기본값 "hand_r"
		FName SocketName = WeaponAssets->AttachmentSocket.IsNone() ? TEXT("hand_r") : WeaponAssets->AttachmentSocket;

		NewWeapon->AttachToComponent(Char->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

		// 관리 변수에 저장
		SpawnedWeaponActor = NewWeapon;

		UE_LOG(LogTemp, Log, TEXT("⚔️ [Visual] 무기 장착 완료: %s (Socket: %s)"),
			*ItemID.ToString(), *SocketName.ToString());
	}
}

void UEquipmentComponent::SetArmorMesh(APlayerBase* Char, EEquipmentSlot Slot, FName ItemID)
{
	if (!Char) return;

	//캐릭터에서 해당 슬롯의 메쉬 컴포넌트 가져오기
	USkeletalMeshComponent* TargetMeshComp = Char->GetArmorComponent(Slot);

	if (!TargetMeshComp)
	{
		// UE_LOG(LogTemp, Warning, TEXT("⚠️ [Visual] 캐릭터에 해당 슬롯(%d) 컴포넌트가 없습니다."), (int32)Slot);
		return;
	}

	// 2. 장착 해제 (ItemID가 None일 경우)
	if (ItemID.IsNone())
	{
		TargetMeshComp->SetSkeletalMesh(nullptr);
		return;
	}

	UParadiseGameInstance* GI = Cast<UParadiseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI) return;

	//데이터 테이블 조회
	FArmorAssets* ArmorAssets = GI->GetDataTableRow<FArmorAssets>(GI->ArmorAssetsDataTable, ItemID);
	if (!ArmorAssets) return;

	//메쉬 로드 및 적용
	USkeletalMesh* LoadedMesh = ArmorAssets->ItemMesh.LoadSynchronous();
	if (LoadedMesh)
	{
		TargetMeshComp->SetSkeletalMesh(LoadedMesh);

		//마스터 포즈 설정 (Master Pose Component)
		//방어구 메쉬가 몸통(Mesh)의 애니메이션을 따라가도록 설정합니다.
		TargetMeshComp->SetMasterPoseComponent(Char->GetMesh());

		UE_LOG(LogTemp, Log, TEXT("🛡️ [Visual] 방어구 교체 완료: %s (Slot: %d)"), *ItemID.ToString(), (int32)Slot);
	}
}




