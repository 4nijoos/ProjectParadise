// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Ingame/InGameHUDWidget.h"

#include "UI/Widgets/Ingame/ParadiseCommonButton.h"
#include "UI/Panel/Ingame/ActionControlPanel.h"
#include "UI/Panel/Ingame/PartyStatusPanel.h"
#include "UI/Panel/Ingame/SummonControlPanel.h"
#include "UI/Widgets/Ingame/CharacterStatusWidget.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 설정 버튼 이벤트 바인딩
	if (Btn_Setting)
	{
		Btn_Setting->OnClicked().AddUObject(this, &UInGameHUDWidget::OnSettingButtonClicked);
	}

	// 자동 전투 버튼 이벤트 바인딩
	if (Btn_AutoMode)
	{
		Btn_AutoMode->OnClicked().AddUObject(this, &UInGameHUDWidget::OnAutoModeButtonClicked);

		// 초기 상태 설정 (예: OFF 텍스트 또는 회색 아이콘)
		// Btn_AutoMode->SetButtonText(FText::FromString(TEXT("MANUAL")));
	}

}

void UInGameHUDWidget::InitializeHUD()
{
	UE_LOG(LogTemp, Log, TEXT("InGameHUD Initialized"));
}

void UInGameHUDWidget::OnSettingButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Setting Button Clicked"));
}

void UInGameHUDWidget::OnAutoModeButtonClicked()
{
	// 1. 상태 토글
	bIsAutoMode = !bIsAutoMode;

	// 2. UI 텍스트/아이콘 변경 (피드백)
	if (Btn_AutoMode)
	{
		// 텍스트 변경 예시 (아이콘 교체로 해도 됨)
		FText StatusText = bIsAutoMode ? FText::FromString(TEXT("AUTO")) : FText::FromString(TEXT("MANUAL"));
		Btn_AutoMode->SetButtonText(StatusText);
	}

	// 3. 실제 게임플레이 로직에 알림 (Controller or PlayerState)
	// GetOwningPlayer()->GetComponent<UAutoCombatComponent>()->SetAutoMode(bIsAutoMode);
	UE_LOG(LogTemp, Log, TEXT("Auto Mode Toggled: %s"), bIsAutoMode ? TEXT("ON") : TEXT("OFF"));
}
