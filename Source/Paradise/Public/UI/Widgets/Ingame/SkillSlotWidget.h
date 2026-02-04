// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSlotWidget.generated.h"

#pragma region 전방 선언
class UImage;
class UTextBlock;
#pragma endregion 전방 선언

/**
 * @class USkillSlotWidget
 * @brief 개별 스킬의 아이콘 표시 및 쿨타임 오버레이 로직을 전담합니다.
 */
UCLASS()
class PARADISE_API USkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
#pragma region 데이터 주입
	/**
	 * @brief 쿨타임 수치를 시각화합니다.
	 * @param Ratio 쿨타임 비율 (0.0 ~ 1.0)
	 * @param RemainingTime 남은 시간(초)
	 */
	void SetCooldownStatus(float Ratio, float RemainingTime);

	/**
	 * @brief 데이터 테이블에서 가져온 스킬 아이콘을 설정합니다..
	 * @param IconTexture 위젯의 메인 슬롯에 표시할 스킬 아이콘 이미지입니다.
	 */
	void SetSkillIcon(UTexture2D* IconTexture);
#pragma endregion 데이터 주입

private:
#pragma region 위젯 바인딩
	/** @brief 스킬 메인 아이콘 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon = nullptr;

	/** @brief 쿨타임 시각화를 위한 마스크 이미지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_CooldownMask = nullptr;

	/** @brief 남은 시간 표시 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CooldownValue = nullptr;
#pragma endregion 위젯 바인딩
};
