// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParadiseSummonPopup.generated.h"

#pragma region 전방선언
class UButton;
class UWidgetSwitcher;
class UParadiseSummonPanel;
#pragma endregion 전방선언

/**
 * @brief 소환 시스템의 메인 팝업 위젯
 * @details WidgetSwitcher를 사용하여 탭(캐릭터/장비)에 따라 우측 컨텐츠를 교체합니다.
 * UI 최적화를 위해 위젯 스위처를 사용하여 불필요한 위젯 생성/파괴를 방지합니다.
 */
UCLASS()
class PARADISE_API UParadiseSummonPopup : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 생명주기
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
#pragma endregion 생명주기

#pragma region 내부 변수
protected:
	// --- 좌측 패널 (탭 버튼) ---

	/** @brief 캐릭터 소환 탭 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Tab_Character = nullptr;

	/** @brief 장비 소환 탭 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Tab_Equipment = nullptr;

	// --- 상단 패널 (네비게이션) ---

	/** @brief 로비로 돌아가는 뒤로가기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Back = nullptr;

	// --- 우측 패널 (컨텐츠 영역) ---

	/** @brief 탭에 따라 내용을 교체해 보여줄 스위처 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher_Content = nullptr;

	// --- 캐싱된 하위 위젯들 (최적화) ---

	/** @brief 캐릭터 소환 패널 (WBP_SummonPanel_Character) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UParadiseSummonPanel> Panel_Character = nullptr;

	/** @brief 장비 소환 패널 (WBP_SummonPanel_Equipment) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UParadiseSummonPanel> Panel_Equipment = nullptr;

private:
	/** @brief 가독성을 위한 인덱스 상수 (하드코딩 방지) */
	const int32 INDEX_CHARACTER = 0;
	const int32 INDEX_EQUIPMENT = 1;
#pragma endregion 내부 변수

#pragma region 내부 로직
private:
	/** @brief 캐릭터 탭 클릭 핸들러 */
	UFUNCTION()
	void OnCharacterTabClicked();

	/** @brief 장비 탭 클릭 핸들러 */
	UFUNCTION()
	void OnEquipmentTabClicked();

	/** @brief 뒤로가기 버튼 클릭 핸들러 */
	UFUNCTION()
	void OnBackButtonClicked();

	/** * @brief 탭 전환 시 UI 상태 업데이트 (버튼 활성/비활성 처리 등)
	 * @param NewIndex 전환할 스위처 인덱스
	 */
	void SwitchTab(int32 NewIndex);
#pragma endregion 내부 로직
};
