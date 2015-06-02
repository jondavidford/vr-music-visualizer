// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicVisualizer.h"
#include "VRGameMode.h"
#include "SongContainer.h"

AVRGameMode::AVRGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AVRGameMode::StartPlay()
{
	Super::StartPlay();

	StartMatch();

	UWorld* const World = GetWorld();

	if (GEngine && World)
	{
		ASongContainer* SongContainer = World->SpawnActor<ASongContainer>();
	}
}


