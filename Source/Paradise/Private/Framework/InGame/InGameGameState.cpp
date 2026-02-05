// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGameGameState.h"

void AInGameGameState::SetCurrentPhase(EGamePhase NewPhase)
{
    // 중복 호출 방지
    if (CurrentPhase == NewPhase) return;

    // 1. 값 변경 (기록)
    CurrentPhase = NewPhase;

    // 2. 방송 송출 (알림) -> 이걸 듣고 UI가 Gold랑 Exp도 확인하러 옴!
    if (OnGamePhaseChanged.IsBound())
    {
        OnGamePhaseChanged.Broadcast(CurrentPhase);
    }
}
