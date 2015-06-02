// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RectangularPrism.generated.h"

UCLASS()
class MUSICVISUALIZER_API ARectangularPrism : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARectangularPrism(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	UPROPERTY(Category = Meshes, VisibleAnywhere)
	UStaticMeshComponent* Prism;
	
};
