// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/PS_InGamePlayerState.h"
#include "Characters/Player/PlayerData.h"

APS_InGamePlayerState::APS_InGamePlayerState()
{
}

void APS_InGamePlayerState::InitSquad(const TArray<UHeroDataAsset*>& StartingHeroes)
{
    // [더미 구현] 인자(StartingHeroes)는 무시하고 강제로 3명 생성
    UE_LOG(LogTemp, Warning, TEXT("🏛️ [PlayerState] 스쿼드 초기화 시작..."));

    for (int32 i = 0; i < 3; i++)
    {
        // 1. 영혼(PlayerData) 액터 스폰
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        APlayerData* NewSoul = GetWorld()->SpawnActor<APlayerData>(APlayerData::StaticClass(), SpawnParams);

        if (NewSoul)
        {
            // 2. 더미 데이터로 초기화
            NewSoul->InitFromDataAsset(nullptr); // nullptr 전달하여 더미 로직 실행

            // 3. 관리 목록에 추가
            SquadMembers.Add(NewSoul);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ [PlayerState] 스쿼드 생성 완료! (멤버 수: %d)"), SquadMembers.Num());
}

APlayerData* APS_InGamePlayerState::GetSquadMemberData(int32 Index) const
{
    if (SquadMembers.IsValidIndex(Index))
    {
        return SquadMembers[Index];
    }
    return nullptr;
}
