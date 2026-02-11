// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Squad/ParadiseSquadDetailWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

#pragma region 생명주기
void UParadiseSquadDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_SwapCharacter)   Btn_SwapCharacter->OnClicked.AddDynamic(this, &UParadiseSquadDetailWidget::HandleSwapChar);
	if (Btn_SwapEquipment)   Btn_SwapEquipment->OnClicked.AddDynamic(this, &UParadiseSquadDetailWidget::HandleSwapEquip);
	if (Btn_CancelEquipMode) Btn_CancelEquipMode->OnClicked.AddDynamic(this, &UParadiseSquadDetailWidget::HandleCancel);
}

void UParadiseSquadDetailWidget::NativeDestruct()
{
	if (Btn_SwapCharacter)   Btn_SwapCharacter->OnClicked.RemoveAll(this);
	if (Btn_SwapEquipment)   Btn_SwapEquipment->OnClicked.RemoveAll(this);
	if (Btn_CancelEquipMode) Btn_CancelEquipMode->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}
#pragma endregion 생명주기

#pragma region 공개 함수
void UParadiseSquadDetailWidget::ShowInfo(const FSquadItemUIData& InData)
{
	// 1. 이름
	if (Text_Name)
	{
		Text_Name->SetText(InData.Name.IsEmpty() ? FText::FromString(TEXT("-")) : InData.Name);
	}

	// 2. 설명/레벨
	if (Text_Desc)
	{
		FString LevelText = (InData.LevelOrCount > 0) ? FString::Printf(TEXT("Lv.%d"), InData.LevelOrCount) : TEXT("");
		Text_Desc->SetText(FText::FromString(LevelText));
	}

	// 3. 아이콘
	/*if (Img_Icon)
	{
		if (InData.Icon)
		{
			Img_Icon->SetBrushFromTexture(InData.Icon);
			Img_Icon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Img_Icon->SetVisibility(ESlateVisibility::Hidden);
		}
	}*/
}

void UParadiseSquadDetailWidget::UpdateButtonState(ESquadUIState CurrentState, bool bIsUnitTab)
{
	// 장비 교체 모드 (EquipMode)
	if (CurrentState == ESquadUIState::EquipMode)
	{
		if (Btn_SwapCharacter)   Btn_SwapCharacter->SetVisibility(ESlateVisibility::Collapsed);
		if (Btn_SwapEquipment)   Btn_SwapEquipment->SetVisibility(ESlateVisibility::Collapsed);
		if (Btn_CancelEquipMode) Btn_CancelEquipMode->SetVisibility(ESlateVisibility::Visible);
	}
	// 일반 모드 (Normal)
	else
	{
		if (Btn_CancelEquipMode) Btn_CancelEquipMode->SetVisibility(ESlateVisibility::Collapsed);

		if (bIsUnitTab)
		{
			// 유닛은 장비 교체 불가
			if (Btn_SwapCharacter) Btn_SwapCharacter->SetVisibility(ESlateVisibility::Visible);
			if (Btn_SwapEquipment) Btn_SwapEquipment->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			// 캐릭터는 둘 다 가능
			if (Btn_SwapCharacter) Btn_SwapCharacter->SetVisibility(ESlateVisibility::Visible);
			if (Btn_SwapEquipment) Btn_SwapEquipment->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UParadiseSquadDetailWidget::ClearInfo()
{
	FSquadItemUIData EmptyData;
	ShowInfo(EmptyData);
}
#pragma endregion 공개 함수

#pragma region 핸들러
void UParadiseSquadDetailWidget::HandleSwapChar()
{
	if (OnSwapCharacterClicked.IsBound()) OnSwapCharacterClicked.Broadcast();
}

void UParadiseSquadDetailWidget::HandleSwapEquip()
{
	if (OnSwapEquipmentClicked.IsBound()) OnSwapEquipmentClicked.Broadcast();
}

void UParadiseSquadDetailWidget::HandleCancel()
{
	if (OnCancelClicked.IsBound()) OnCancelClicked.Broadcast();
}
#pragma endregion 핸들러