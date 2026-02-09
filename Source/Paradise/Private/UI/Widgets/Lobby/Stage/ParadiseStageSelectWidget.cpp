// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/Stage/ParadiseStageSelectWidget.h"
#include "UI/Data/ParadiseStageItemObject.h"
#include "Components/TileView.h"
#include "Data/Structs/StageStructs.h"

void UParadiseStageSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitStageList();
}

#pragma region 초기화 로직 (Initialization Logic)

void UParadiseStageSelectWidget::InitStageList()
{
	if (!TileView_Stages) return;
	if (!DT_StageStats || !DT_StageAssets)
	{
		UE_LOG(LogTemp, Error, TEXT("[StageSelect] 데이터 테이블이 연결되지 않았습니다."));
		return;
	}

	TileView_Stages->ClearListItems();

	// 1. Stats 테이블의 모든 RowName을 가져옵니다. (기준이 되는 테이블)
	TArray<FName> RowNames = DT_StageStats->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		// 2. Stats 데이터 찾기
		FStageStats* Stats = DT_StageStats->FindRow<FStageStats>(RowName, TEXT("InitStats"));

		// 3. Assets 데이터 찾기 (같은 RowName 사용)
		FStageAssets* Assets = DT_StageAssets->FindRow<FStageAssets>(RowName, TEXT("InitAssets"));

		// 두 데이터가 모두 존재해야 유효한 스테이지로 간주
		if (Stats && Assets)
		{
			// 4. 브릿지 객체 생성 (데이터 주입)
			UParadiseStageItemObject* NewItem = UParadiseStageItemObject::Create(this, RowName, *Stats, *Assets);

			// 5. 리스트 뷰에 추가
			TileView_Stages->AddItem(NewItem);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[StageSelect] RowName '%s'에 대한 데이터 쌍이 일치하지 않습니다."), *RowName.ToString());
		}
	}
}

#pragma endregion 초기화 로직 (Initialization Logic)