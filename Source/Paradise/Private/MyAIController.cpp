// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MonsterAI.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BaseUnit.h"
#include "Kismet/GameplayStatics.h"

AMyAIController::AMyAIController()
{
    SightConfig->SightRadius = 800.f;
    SightConfig->LoseSightRadius = 1000.f;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetDetected);
}

void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BTAsset && BBAsset)
    {
        UBlackboardComponent* BBRawPtr = Blackboard.Get();

        if (UseBlackboard(BBAsset, BBRawPtr))
        {
            // 1. 적군 기지 찾기 로직
            TArray<AActor*> OutActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseUnit::StaticClass(), OutActors);

            ABaseUnit* SelfUnit = Cast<ABaseUnit>(InPawn);
            for (AActor* Actor : OutActors)
            {
                ABaseUnit* Unit = Cast<ABaseUnit>(Actor);
                // 팀이 다르고 "Base" 태그가 있는 액터 찾기
                if (Unit && SelfUnit && Unit->TeamID != SelfUnit->TeamID && Actor->ActorHasTag(TEXT("Base")))
                {
                    Blackboard->SetValueAsObject(TEXT("BaseActor"), Actor);
                    break;
                }
            }

            RunBehaviorTree(BTAsset);
        }
    }
}

void AMyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    if (Blackboard == nullptr) return;

    AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(BB_KEYS::TargetActor));

    // 이미 타겟이 있는 경우
    if (CurrentTarget && CurrentTarget->IsValidLowLevel())
    {
        // 현재 타겟을 놓친 경우에만 초기화
        if (CurrentTarget == Actor && !Stimulus.WasSuccessfullySensed())
        {
            Blackboard->ClearValue(BB_KEYS::TargetActor);
        }
        return;
    }

    // 타겟이 없는 상태에서 새로 감지했을 때
    if (Stimulus.WasSuccessfullySensed())
    {
        ABaseUnit* TargetUnit = Cast<ABaseUnit>(Actor);
        ABaseUnit* SelfUnit = Cast<ABaseUnit>(GetPawn());

        // 적군일 때만 타겟으로 등록
        if (TargetUnit && SelfUnit && TargetUnit->TeamID != SelfUnit->TeamID)
        {
            Blackboard->SetValueAsObject(BB_KEYS::TargetActor, Actor);
        }
    }
}