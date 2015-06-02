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
	// variables for getting audio samples
	UAudioComponent* Song;
	USoundWave* Sound;
	UPCMAudioExtractor* AudioExtractor;

	// variables for beat tracker
	float currentTime;
	int nPeriods;
	int iP;
	TArray<float> p;
	float pMin;
	float rmsThreshold;
	float lastBeatTime;
	float nextBeatTime;
	bool beatNotInitialized;

	
	//for debugging
	int idx;
	float debuggingRMS[600];
	ofstream myfile;
	ofstream beatFile;
	bool fileNotClosed;
	int beatIdx;
	float lastPrintedBeat;
	

	// variables for FFT
	int bufferSize;
	int nOutputBins;
	TArray<float> Buffer;
	double *in;
	fftw_complex *out;
	fftw_plan plan;
	double maxIntensity;

	// visualization elements
	int nFreqBins;
	int nFFTBinsInPillar;
	TArray<ARectangularPrism*> Pillars;
	TArray<ARectangularPrism*> Floor;

	bool thisBeatVisualized;
};
