// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseUnit.h"

/**
 * @brief 생성자: 기본 스탯(HP, TeamID)을 초기화합니다.
 */
ABaseUnit::ABaseUnit()
{
    PrimaryActorTick.bCanEverTick = false;

    TeamID = 0;
    MaxHP = 100.f;
    HP = MaxHP;
}

/**
 * @brief 데미지 처리 및 사망 판정을 수행합니다.
 */
float ABaseUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 부모 함수의 기본 로직 실행
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0 && HP > 0)
    {
        // 체력 감소
        HP -= ActualDamage;
        if (HP <= 0)
        {
            HP = 0;
            // 죽음 처리 로직
            UE_LOG(LogTemp, Error, TEXT("%s has been DESTROYED!"), *GetName());
        }

        // 로그로 실시간 체력 확인
        UE_LOG(LogTemp, Warning, TEXT("[%s] Took %.1f Damage! Current HP: %.1f / %.1f"),
            *GetName(), ActualDamage, HP, MaxHP);
    }

    return ActualDamage;
}