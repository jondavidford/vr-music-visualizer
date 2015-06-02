// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicVisualizer.h"
#include "RectangularPrism.h"


// Sets default values
ARectangularPrism::ARectangularPrism(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Prism = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CubeMesh"));
	//

	static ConstructorHelpers::FObjectFinder<UStaticMesh>StaticMesh(TEXT("/Game/Box_Brush_StaticMesh.Box_Brush_StaticMesh"));
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("/Game/NewMaterial.NewMaterial"));

	Prism->SetMobility(EComponentMobility::Movable);
	//Prism->SetWorldLocation(FVector(100, 100, 100));
	//Prism->SetWorldScale3D(FVector(1, 10, 1));

	Prism->SetStaticMesh(StaticMesh.Object);
	Prism->SetMaterial(0, Material.Object);

	RootComponent = Prism;
}

// Called when the game starts or when spawned
void ARectangularPrism::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARectangularPrism::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

