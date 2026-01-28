#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Enums/GameEnums.h"
#include "ItemStructs.generated.h"

/**
 * @struct FItemBaseStats
 * @brief 아이템의 수치(Data)와 로직적인 속성을 담당하는 최상위 부모 구조체입니다.
 * @details 모든 아이템(무기, 방어구 등)이 공통적으로 가지는 이름, 등급, 설명 등을 포함합니다.
 */
USTRUCT(BlueprintType)
struct FItemBaseStats
{
	GENERATED_BODY()

public:
	/**
	 * @brief 인게임 UI에 표시될 아이템의 이름
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	FText DisplayName;

	/**
	 * @brief 아이템 설명 텍스트
	 * @details 툴팁이나 상세 정보창에 표시됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	FText Description;

	/**
	 * @brief 아이템 희귀도 (Rarity)
	 * @note 1(Common) ~ 5(Legendary). 드랍 확률 및 UI 테두리 색상 결정에 사용됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	EItemRarity Rarity;

	/**
	 * @brief 세트 효과 ID (Set Bonus ID)
	 * @note DT_SetBonus 테이블의 RowName과 일치해야 합니다. (세트 효과가 없으면 None)
	 * @see DT_SetBonus
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	FName SetID;

	/**
	 * @brief 아이템 타입 (필터링 용도)
	 * @note 인벤토리 정렬이나 장착 슬롯 제한 확인에 사용됩니다. (예: Weapon, Armor, Consumable)
	 * Enums/GameEnums.h 에 정의된 Enum을 사용하는 것을 권장합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Info")
	EItemMainType ItemType;
};
