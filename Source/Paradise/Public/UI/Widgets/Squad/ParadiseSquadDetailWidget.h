// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Data/SquadUITypes.h"
#include "ParadiseSquadDetailWidget.generated.h"

#pragma region 전방 선언
class UTextBlock;
class UButton;
class UImage;
#pragma endregion 전방 선언

/** @brief 상세창 액션 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDetailAction);

/**
 * @class UParadiseSquadDetailWidget
 * @brief 선택된 유닛/캐릭터의 상세 정보를 보여주고, 교체/장착 액션을 트리거하는 뷰 위젯.
 */
UCLASS()
class PARADISE_API UParadiseSquadDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 로직
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * @brief 받은 데이터를 UI에 표시합니다.
	 * @param InData 표시할 UI 데이터 구조체
	 */
	UFUNCTION(BlueprintCallable, Category = "DetailView")
	void ShowInfo(const FSquadItemUIData& InData);

	/**
	 * @brief 현재 상태에 따라 버튼의 가시성을 갱신합니다.
	 * @param CurrentState 현재 UI 상태 (Normal / EquipMode)
	 * @param bIsUnitTab 현재 유닛(소환수) 탭인지 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "DetailView")
	void UpdateButtonState(ESquadUIState CurrentState, bool bIsUnitTab);

	/** @brief 정보를 비웁니다. */
	void ClearInfo();
#pragma endregion 로직

#pragma region 핸들러
private:
	UFUNCTION() 
	void HandleSwapChar();
	UFUNCTION() 
	void HandleSwapEquip();
	UFUNCTION() 
	void HandleCancel();
#pragma endregion 핸들러

#pragma region UI 바인딩
protected:
	/** @brief 대상 이름 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Name = nullptr;

	/** @brief 대상 설명 또는 스탯 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Desc = nullptr;

	/** @brief 캐릭터 교체 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_SwapCharacter = nullptr;

	/** @brief 장비 교체 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_SwapEquipment = nullptr;

	/** @brief 장비 교체 모드 취소/완료 버튼 (초기엔 숨김) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_CancelEquipMode = nullptr;
#pragma endregion UI 바인딩

#pragma region 이벤트
public:
	/** @brief 캐릭터 교체 버튼 클릭 알림 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDetailAction OnSwapCharacterClicked;

	/** @brief 장비 교체 버튼 클릭 알림 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDetailAction OnSwapEquipmentClicked;

	/** @brief 취소 버튼 클릭 알림 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDetailAction OnCancelClicked;
#pragma endregion 이벤트
};
