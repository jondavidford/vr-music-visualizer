// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PCMAudioExtractor.h"
#include "fftw3.h"
#include "RectangularPrism.h"
#include <iostream>
#include <fstream>

using namespace std;
#include "SongContainer.generated.h"


UCLASS()
class MUSICVISUALIZER_API ASongContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASongContainer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	UAudioComponent* Song;
	USoundWave* Sound;
	UPCMAudioExtractor* AudioExtractor;

	float currentTime;
	int nPeriods;
	int iP;
	TArray<float> p;
	int idx;
	float debuggingRMS[600];
	float pMin;
	float rmsThreshold;
	float lastBeatTime;
	float nextBeatTime;
	bool beatNotInitialized;

	ofstream myfile;
	bool fileNotClosed;


	int bufferSize;
	TArray<float> Buffer;

	double *in;
	fftw_complex *out;
	fftw_plan plan;
	double maxIntensity;

	ARectangularPrism* Cubes[1024];
};
