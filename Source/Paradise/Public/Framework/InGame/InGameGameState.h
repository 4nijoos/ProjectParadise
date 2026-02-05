// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/Enums/GameEnums.h"
#include "InGameGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);

UCLASS()
class PARADISE_API AInGameGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	//게임 진행 단계 설정 함수
	void SetCurrentPhase(EGamePhase NewPhase);

	//스테이지 타이머 시간(UI 타이머용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	float RemainingTime;

	//스테이지 타이머 작동 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	bool bIsTimerActive = false;

	//스테이지 ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	FString DisplayStageName;

	// [보상] 획득한 골드 (결과창 UI용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	int32 AcquiredGold;

	// [보상] 획득한 경험치 (결과창 UI용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	int32 AcquiredExp;

	//다음 스테이지 ID (결과창 UI용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	FName NextStageID;

	//게임 진행 단계 변경 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGamePhaseChanged OnGamePhaseChanged;

	//현재 스테이지 진행 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage Data")
	EGamePhase CurrentPhase;
};
