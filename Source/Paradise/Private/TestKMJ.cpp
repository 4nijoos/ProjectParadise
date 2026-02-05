// Fill out your copyright notice in the Description page of Project Settings.

#include "TestKMJ.h"
#include "GameFramework/DefaultPawn.h"

ATestGameMode::ATestGameMode()
{
    DefaultPawnClass = ADefaultPawn::StaticClass();
}