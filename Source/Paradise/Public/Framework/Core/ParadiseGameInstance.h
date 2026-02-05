// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "ParadiseGameInstance.generated.h"

#pragma region 전방 선언
class ULoadingWidget;
#pragma endregion 전방 선언

/**
 * @class UParadiseGameInstance
 * @brief 게임의 수명 주기 동안 유지되는 전역 데이터 및 시스템 관리 클래스.
 * @details
 * 1. 레벨 간 비동기 로딩 (Async Loading) 및 로딩 화면(Loading Screen) 관리. (추후 세이브 관리는 그때 담당자가 추가할 것)
 */
UCLASS()
class PARADISE_API UParadiseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UParadiseGameInstance();
	virtual void Init() override;

#pragma region 레벨 관리 (Level & Loading System)
public:
	/** @brief 전역 에셋 로더 (비동기 로딩 수행 담당). */
	FStreamableManager StreamableManager;

	/**
	 * @brief 지정된 레벨로 이동하기 전, 필요한 에셋들을 비동기로 로딩합니다.
	 * @details 로딩 위젯을 띄우고, 에셋 로딩이 완료되면 OpenLevel을 호출하여 렉을 방지합니다.
	 * @param LevelName 이동할 레벨의 이름 (예: "L_InGame", "L_Lobby")
	 * @param AssetsToLoad 미리 메모리에 올릴 에셋들의 경로(SoftObjectPtr) 목록
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|System|Level")
	void OpenLevelWithAsyncLoad(FName LevelName, const TArray<TSoftObjectPtr<UObject>>& AssetsToLoad);

protected:
	/** @brief 로딩 화면 위젯 클래스 (BP_LoadingWidget 할당). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|UI")
	TSubclassOf<UUserWidget> LoadingWidgetClass; // ULoadingWidget이 없다면 UUserWidget으로 대체 가능

	/** @brief 최소 로딩 시간 (초). 로딩이 빨라도 이 시간만큼은 연출을 보여줍니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|Config")
	float MinLoadingTime = 2.0f;

private:
	/** @brief 현재 화면에 떠 있는 로딩 위젯 인스턴스. */
	UPROPERTY()
	TObjectPtr<ULoadingWidget> CurrentLoadingWidget = nullptr;

	/** @brief 이동할 목표 레벨 이름 임시 저장. */
	FName NextLevelName;

	/** @brief 비동기 로딩 핸들. (진행률 확인 및 생명주기 관리용) */
	TSharedPtr<FStreamableHandle> CurrentLoadHandle = nullptr;

	/** @brief 진행률 체크 타이머 핸들. */
	FTimerHandle ProgressTimerHandle;

	/** @brief 로딩 시작 후 경과 시간. */
	float TotalElapsedTime = 0.0f;

	/** @brief 주기적으로 호출되어 로딩 바를 갱신하고 완료를 체크하는 함수. */
	void UpdateLoadingProgress();
#pragma endregion 레벨 관리

#pragma region 게임 데이터
public:
	/** @brief 현재까지 클리어한 최고 스테이지 인덱스 (0: 1-1 도전 중). */
	UPROPERTY(BlueprintReadWrite, Category = "Paradise|Progress")
	int32 MaxClearedStageIndex = 0;

#pragma endregion 게임 데이터

};
