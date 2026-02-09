// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Lobby/LobbyPlayerController.h"
#include "UI/HUD/Lobby/ParadiseLobbyHUDWidget.h"
#include "Blueprint/UserWidget.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//1. 마우스 커서 보이게 설정
	bShowMouseCursor = true;

	//2. UI 전용 입력 모드 설정
	FInputModeUIOnly InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputModeData);

	UE_LOG(LogTemp, Log, TEXT("LobbyController: Mouse Cursor On"));

	// 2. HUD 위젯 생성 및 부착
	if (LobbyHUDClass)
	{
		// 위젯 생성
		UParadiseLobbyHUDWidget* LobbyHUD = CreateWidget<UParadiseLobbyHUDWidget>(this, LobbyHUDClass);

		if (LobbyHUD)
		{
			// 화면에 띄우기
			LobbyHUD->AddToViewport();

			// 캐싱 (이미 HUD NativeConstruct에서 SetLobbyHUD를 호출하지만, 확실하게 한번 더 해도 무방)
			SetLobbyHUD(LobbyHUD);

			UE_LOG(LogTemp, Log, TEXT("[LobbyController] WBP_LobbyHUD 생성 및 부착 성공!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LobbyController] LobbyHUDClass가 설정되지 않았습니다! BP를 확인하세요."));
	}
}

void ALobbyPlayerController::SetLobbyHUD(UParadiseLobbyHUDWidget* InHUD)
{
    CachedLobbyHUD = InHUD;
}

void ALobbyPlayerController::SetLobbyMenu(EParadiseLobbyMenu InNewMenu)
{
    if (CurrentMenu == InNewMenu) return;

    CurrentMenu = InNewMenu;
    UE_LOG(LogTemp, Log, TEXT("[Controller] 메뉴 변경: %d"), (int32)CurrentMenu);

    // HUD에게 UI 변경 지시
    if (CachedLobbyHUD)
    {
        CachedLobbyHUD->UpdateMenuStats(CurrentMenu);
    }
}