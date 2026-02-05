// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Data/Structs/StageStructs.h"
#include "InGameGameMode.generated.h"

//1. ���� ���� ����
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Ready,		//�������� ���� �� ī��Ʈ�ٿ�(�غ�)
	Combat,		//�������� ���� �� Ÿ�̸� �۵�
	Victory,	//�¸�(���� ����)
	Defeat,		//�й�(Ÿ�ӿ��� �Ǵ� ���)
	Result		//���â ǥ�� �� �κ� �̵� ���
};

/**
 * 
 */
UCLASS()
class PARADISE_API AInGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInGameGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:

	//���ӻ��� �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetGamePhase(EGamePhase NewPhase);

public:
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	void EndStage(bool bIsVictory);

protected:
	//���������̺� �о� �������� ���� �ʱ�ȭ
	void InitializeStageData(FName StageID);

	//�� �ܰ迡 ������ �� ȣ��Ǵ� �Լ���
	void OnPhaseReady();
	void OnPhaseCombat();
	void OnPhaseVictory();
	void OnPhaseDefeat();
	void OnPhaseResult();



protected:
	//�����Ϳ� �������� ���� �Ҵ� 
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* StageInfoTable;

	//ĳ�̵� ���ӽ�����Ʈ ������
	UPROPERTY()
	class AInGameGameState* CachedGameState;

	//���� �������� ��ü ����
	FStageStats CurrentStageData;

	//���� ������ �ܰ�(�ܺο��� �Ǵܰ���)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EGamePhase CurrentPhase;
	
};
