// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Ingame/InGameHUDWidget.h"

#include "UI/Panel/Ingame/ActionControlPanel.h"
#include "UI/Panel/Ingame/PartyStatusPanel.h"
#include "UI/Panel/Ingame/SummonControlPanel.h"
#include "UI/Widgets/Ingame/ParadiseCommonButton.h"
#include "UI/Widgets/Ingame/VirtualJoystickWidget.h"
#include "UI/Widgets/Ingame/CharacterStatusWidget.h"
#include "UI/Widgets/Ingame/GameTimerWidget.h"

#include "UI/Widgets/Ingame/Popup/VictoryPopupWidget.h"
#include "UI/Widgets/Ingame/Popup/DefeatPopupWidget.h"

#include "Framework/InGame/InGameGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region 초기화 (Initialization)
	// 1. 결과 팝업 초기화 (숨김 상태로 시작)
	if (Widget_VictoryPopup) Widget_VictoryPopup->SetVisibility(ESlateVisibility::Collapsed);
	if (Widget_DefeatPopup) Widget_DefeatPopup->SetVisibility(ESlateVisibility::Collapsed);

	// 2. 버튼 이벤트 바인딩
	if (Btn_Setting)
	{
		Btn_Setting->OnClicked().AddUObject(this, &UInGameHUDWidget::OnSettingButtonClicked);
	}

	if (Btn_AutoMode)
	{
		Btn_AutoMode->OnClicked().AddUObject(this, &UInGameHUDWidget::OnAutoModeButtonClicked);
		// 초기 텍스트 설정 (필요 시)
		// Btn_AutoMode->SetButtonText(FText::FromString(TEXT("MANUAL")));
	}

	// 3. 조이스틱 바인딩
	if (VirtualJoystick)
	{
		VirtualJoystick->OnJoystickInput.AddDynamic(this, &UInGameHUDWidget::OnJoystickInput);
	}

	// 4. GameState 연결 및 델리게이트 구독 (핵심)
	InitializeHUD();
#pragma endregion 초기화

	// 5. 타이머 UI 갱신 (0.5초마다 남은 시간만 갱신)
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			HUDUpdateTimerHandle,
			this,
			&UInGameHUDWidget::OnUpdateHUD,
			0.5f,
			true
		);
	}
}

void UInGameHUDWidget::NativeDestruct()
{
	// 1. 타이머 정지
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HUDUpdateTimerHandle);
	}

	// 2. 델리게이트 안전 해제
	if (CachedGameState.IsValid())
	{
		CachedGameState->OnGamePhaseChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

#pragma region 내부 로직 구현
void UInGameHUDWidget::InitializeHUD()
{
	// GameState를 가져와서 캐싱 (매번 Cast하지 않기 위함)
	AInGameGameState* GS = Cast<AInGameGameState>(UGameplayStatics::GetGameState(this));
	if (GS)
	{
		CachedGameState = GS;

		// 페이즈 변경 이벤트 바인딩 ("상태 바뀌면 연락해!")
		GS->OnGamePhaseChanged.AddUniqueDynamic(this, &UInGameHUDWidget::HandleGamePhaseChanged);

		// 현재 상태 즉시 반영 (이미 게임 진행 중일 경우 대비)
		HandleGamePhaseChanged(GS->CurrentPhase);

		UE_LOG(LogTemp, Log, TEXT("[InGameHUD] GameState 연결 완료."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameHUD] GameState를 찾을 수 없습니다."));
	}
}

void UInGameHUDWidget::HandleGamePhaseChanged(EGamePhase NewPhase)
{
	UE_LOG(LogTemp, Log, TEXT("[InGameHUD] 페이즈 변경 감지: %d"), (int32)NewPhase);

	// 결과 화면인가? (승리 혹은 패배)
	bool bIsResultPhase = (NewPhase == EGamePhase::Victory || NewPhase == EGamePhase::Defeat);

	// 1. 인게임 UI (조이스틱, 타이머 등) 제어
	// 결과창이 뜨면 조작 UI는 숨긴다.
	ESlateVisibility InGameUIVisibility = bIsResultPhase ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;

	if (VirtualJoystick) VirtualJoystick->SetVisibility(InGameUIVisibility);
	if (GameTimerWidget) GameTimerWidget->SetVisibility(InGameUIVisibility);
	if (ActionControlPanel) ActionControlPanel->SetVisibility(InGameUIVisibility);
	if (SummonControlPanel) SummonControlPanel->SetVisibility(InGameUIVisibility);

	// 2. 팝업 표시 로직
	switch (NewPhase)
	{
	case EGamePhase::Victory:
		// 승리 팝업 표시 및 데이터 설정
		if (Widget_VictoryPopup && CachedGameState.IsValid())
		{
			// ★ 중요: 여기서 GameState의 최종 데이터를 가져와서 한 번만 설정
			int32 EarnedGold = CachedGameState->AcquiredGold;
			int32 EarnedExp = CachedGameState->AcquiredExp;

			// 별 개수 계산 로직이 있다면 여기서 수행 (예시: 3개 고정)
			Widget_VictoryPopup->SetVictoryData(3, EarnedGold, EarnedExp);
			Widget_VictoryPopup->SetVisibility(ESlateVisibility::Visible);

			// 패배 팝업은 확실히 끄기
			if (Widget_DefeatPopup) Widget_DefeatPopup->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;

	case EGamePhase::Defeat:
		// 패배 팝업 표시
		if (Widget_DefeatPopup)
		{
			Widget_DefeatPopup->SetVisibility(ESlateVisibility::Visible);

			// 승리 팝업은 확실히 끄기
			if (Widget_VictoryPopup) Widget_VictoryPopup->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;

	default:
		// 전투 중이거나 준비 상태 등에서는 팝업을 모두 숨김
		if (Widget_VictoryPopup) Widget_VictoryPopup->SetVisibility(ESlateVisibility::Collapsed);
		if (Widget_DefeatPopup) Widget_DefeatPopup->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}
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

void UInGameHUDWidget::OnJoystickInput(FVector2D InputVector)
{
	// 조이스틱 입력이 오면 폰(캐릭터)에게 이동 명령 전달
	if (APawn* OwnedPawn = GetOwningPlayerPawn())
	{
		const FRotator ControlRot = GetOwningPlayer()->GetControlRotation();
		const FRotator YawRot(0, ControlRot.Yaw, 0);

		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		// InputVector.Y에 -1.0f를 곱해서 '위로 드래그' = '전진'이 되도록 수정
		OwnedPawn->AddMovementInput(ForwardDir, InputVector.Y * -1.0f);
		OwnedPawn->AddMovementInput(RightDir, InputVector.X);
	}
}

void UInGameHUDWidget::OnUpdateHUD()
{
	// [최적화] 타이머에 의해 0.5초마다 호출됨
	if (CachedGameState.IsValid())
	{
		// 타이머 위젯이 있고, 게임 상태가 타이머가 돌아가는 상태일 때만 갱신
		if (GameTimerWidget)
		{
			// GameState에 남은 시간 변수가 public이라고 가정
			GameTimerWidget->UpdateTime(CachedGameState->RemainingTime);
		}
	}
}
#pragma endregion 내부 로직 구현