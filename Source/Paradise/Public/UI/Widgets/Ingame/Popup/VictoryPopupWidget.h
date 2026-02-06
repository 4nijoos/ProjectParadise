// Copyright (C) Project Paradise. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Ingame/Popup/GameResultWidgetBase.h"
#include "VictoryPopupWidget.generated.h"

#pragma region 전방 선언
class UTextBlock;
class UImage;
class UTexture2D;
#pragma endregion 전방 선언

/**
 * @class UVictoryPopupWidget
 * @brief 게임 승리 시 표시되는 팝업 위젯.
 * @details
 * 1. 외부 데이터를 주입받아(SetVictoryData) UI를 갱신하는 데이터 주도 방식.
 * 2. 별점(1~3성)에 따른 이미지 교체 로직 포함.
 * 3. 다음 스테이지 이동 기능 제공.
 */
UCLASS()
class PARADISE_API UVictoryPopupWidget : public UGameResultWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region 외부 인터페이스
public:
	/**
	 * @brief 승리 결과 데이터를 설정하고 UI를 갱신합니다.
	 * @param InStarCount 획득한 별 개수 (1~3)
	 * @param InEarnedGold 획득 골드량
	 * @param InEarnedExp 획득 경험치량
	 */
	UFUNCTION(BlueprintCallable, Category = "Paradise|UI|Result")
	void SetVictoryData(int32 InStarCount, int32 InEarnedGold, int32 InEarnedExp);
#pragma endregion 외부 인터페이스

#pragma region 승리 전용 UI
protected:
	/** @brief 다음 스테이지로 이동 버튼. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_NextStage = nullptr;

	/** @brief 획득 골드 표시 텍스트. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_GoldValue = nullptr;

	/** @brief 획득 경험치 표시 텍스트. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ExpValue = nullptr;

	/** @brief 첫 번째 별 이미지. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Star1 = nullptr;

	/** @brief 두 번째 별 이미지. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Star2 = nullptr;

	/** @brief 세 번째 별 이미지. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Star3 = nullptr;
#pragma endregion 승리 전용 UI

#pragma region 리소스 설정 (Data-Driven)
protected:
	/** @brief 별 활성화 시 사용할 텍스처 (노란 별). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paradise|Resource")
	TObjectPtr<UTexture2D> StarOnTexture = nullptr;

	/** @brief 별 비활성화 시 사용할 텍스처 (회색 별). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paradise|Resource")
	TObjectPtr<UTexture2D> StarOffTexture = nullptr;

	/** @brief 다음 스테이지 레벨 이름 (기획 데이터 연동 필요). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paradise|Config")
	FName NextStageLevelName = NAME_None;
#pragma endregion 리소스 설정

#pragma region 내부 로직
private:
	/** @brief 다음 스테이지 버튼 클릭 핸들러. */
	UFUNCTION()
	void OnNextStageClicked();
#pragma endregion 내부 로직
};