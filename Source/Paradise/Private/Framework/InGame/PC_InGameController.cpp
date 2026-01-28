// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/PC_InGameController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Characters/Base/PlayerBase.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
void APC_InGameController::BeginPlay()
{
	Super::BeginPlay();

    //입력 매핑 컨텍스트 연결
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

}

void APC_InGameController::SetupInputComponent()
{
	Super::SetupInputComponent();

}

void APC_InGameController::RequestSwitchPlayer(int32 PlayerIndex)
{
	
}

void APC_InGameController::InitializeSquadPawns()
{
}

void APC_InGameController::PossessAI(APlayerBase* TargetCharacter)
{
}

void APC_InGameController::OnInputSwitchHero1(const FInputActionValue& Value)
{
    //입력 액션 바인딩 함수 후에 UI 모바일 버튼으로 바인딩예정
}

void APC_InGameController::OnInputSwitchHero2(const FInputActionValue& Value)
{
    //입력 액션 바인딩 함수 후에 UI 모바일 버튼으로 바인딩예정
}

void APC_InGameController::OnInputSwitchHero3(const FInputActionValue& Value)
{
    //입력 액션 바인딩 함수 후에 UI 모바일 버튼으로 바인딩예정
}


