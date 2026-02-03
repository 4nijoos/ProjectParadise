// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/Attributes/GASMacros.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

/**
 * @class UBaseAttributeSet
 * @brief 게임 내 모든 유닛(플레이어, 몬스터, 소환수)이 공유하는 기본 스탯 클래스
 * @details
 * GAS(GameplayAbilitySystem)의 핵심 데이터 컨테이너입니다.
 * * 주요 기능:
 * 1. 기본 스탯(HP, MP, 공격력 등) 정의 및 관리
 * 2. PreAttributeChange를 통한 수치 보정 (Clamping)
 * 3. PostGameplayEffectExecute를 통한 데미지 연산 처리 (Meta Attribute)
 */
UCLASS()
class PARADISE_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBaseAttributeSet();

	/**
	* @brief 속성 값이 변경되기 직전에 호출되는 함수 (값 보정용)
	* @details 주로 현재 체력(Health)이나 마나(Mana)가 0보다 작아지거나 Max값을 넘지 않도록
	* Clamp(범위 제한) 처리를 수행합니다.
	* @param Attribute 변경되려는 속성
	* @param NewValue 변경될 새로운 값 (참조로 전달되어 수정 가능)
	*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	* @brief GE(GameplayEffect)가 실행된 직후 호출되는 함수 (후처리용)
	* @details 'IncomingDamage' 같은 메타 어트리뷰트로 들어온 값을 실제 체력(Health) 차감 로직으로 변환합니다.
	* 데미지 폰트 출력, 사망 처리 트리거 등이 이곳에서 이루어집니다.
	* @param Data 발동된 GE에 대한 상세 정보 (가해자, 효과 스펙 등)
	*/
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
	// =====================================================
	//  기본 스탯 (Base Stats)
	// =====================================================

	/**
	 * @brief 현재 체력 (Current Health)
	 * @details 0 ~ MaxHealth 사이의 값을 유지합니다. 0이 되면 사망 처리됩니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health)

	/**
	* @brief 최대 체력 (Max Health)
	* @details 체력의 상한선입니다. 버프/장비로 증가할 수 있습니다.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)

	/**
	* @brief 현재 마나 (Current Mana)
	* @details 스킬 사용 시 소모되는 자원입니다.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Mana)

	/**
	* @brief 최대 마나 (Max Mana)
	* @details 마나의 상한선입니다.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxMana)

	/**
	* @brief 공격력 (Attack Power)
	* @details 데미지 계산 공식의 기본 계수가 됩니다.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, AttackPower)

	/**
	* @brief 방어력 (Defense)
	* @details 들어오는 데미지를 차감하는 수치입니다.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Defense)

	/**
	* @brief 크리티컬 확률 (CritRate)
	공격 적중 시 치명타가 발생할 확률입니다.
	* - 단위: 비율 (Ratio) (예: 0.1 = 10%, 1.0 = 100%)
	* - 범위: 0.0 ~ 1.0 (음수 불가)
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData CritRate;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CritRate)

	/**
	* @brief 이동 속도 (Movement Speed)
	* @details 캐릭터의 걷기/달리기 속도에 영향을 줍니다.
	* @note 값이 변경되면 CharacterMovementComponent의 MaxWalkSpeed에 동기화해주어야 합니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MoveSpeed)

	// =====================================================
	//  메타 어트리뷰트 (Meta Attributes)
	// =====================================================

	/**
	* @brief 들어오는 데미지 (Incoming Damage)
	* @details
	* 실제 저장되는 스탯이 아닌, 데미지 계산을 위해 잠시 값을 담아두는 '메타 어트리뷰트'입니다.
	* GameplayEffect가 이 값을 변경하면, PostGameplayEffectExecute에서 값을 읽어 Health를 깎고
	* 즉시 0으로 초기화됩니다.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Meta")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, IncomingDamage)
};
