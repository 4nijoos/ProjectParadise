// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParadiseStageSelectWidget.generated.h"

#pragma region 전방 선언
class UTileView;
class UDataTable;
#pragma endregion 전방 선언

/**
 * @class UParadiseStageSelectWidget
 * @brief 스테이지 선택 화면 메인 위젯.
 * @details 두 개의 데이터 테이블(Stats, Assets)을 읽어 리스트를 초기화합니다.
 */
UCLASS()
class PARADISE_API UParadiseStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

#pragma region UI 컴포넌트
protected:
	/** @brief 스테이지 노드들을 나열할 타일 뷰 (Tile View) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TileView_Stages = nullptr;
#pragma endregion UI 컴포넌트

#pragma region 데이터 설정
protected:
	/** @brief 기획 데이터 테이블 (RowType: FStageStats) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|Data")
	TObjectPtr<UDataTable> DT_StageStats = nullptr;

	/** @brief 리소스 데이터 테이블 (RowType: FStageAssets) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Paradise|Data")
	TObjectPtr<UDataTable> DT_StageAssets = nullptr;
#pragma endregion 데이터 설정

#pragma region 초기화 로직
private:
	/** @brief 데이터 테이블을 순회하며 리스트 뷰를 채웁니다. */
	void InitStageList();
#pragma endregion 초기화 로직
};
