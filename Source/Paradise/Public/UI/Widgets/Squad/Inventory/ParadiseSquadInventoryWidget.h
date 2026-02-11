// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Data/SquadUITypes.h"
#include "ParadiseSquadInventoryWidget.generated.h"

#pragma region 전방 선언
class UWidgetSwitcher;
class UWrapBox;
class UScrollBox;
class UParadiseItemSlot; // 개별 슬롯 위젯 (기존에 있다면 재사용, 없다면 생성 필요)
#pragma endregion 전방 선언
/**
 * 
 */
UCLASS()
class PARADISE_API UParadiseSquadInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
};
