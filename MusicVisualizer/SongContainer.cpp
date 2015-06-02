// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicVisualizer.h"
#include "SongContainer.h"
#include "RectangularPrism.h"
#include "fftw3.h"
#include <math.h>

using namespace std;

// Sets default values
ASongContainer::ASongContainer()
	: currentTime(0),
	bufferSize(1024),
	maxIntensity(0),
	nFreqBins(15),
	pMin(0.2),
	nPeriods(20),
	lastBeatTime(0),
	nextBeatTime(0),
	beatNotInitialized(true),
	thisBeatVisualized(false),
	lastPrintedBeat(0),
	rmsThreshold(10000)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<USoundWave> Sound;
		FConstructorStatics()
			: //Sound(TEXT("/Game/Cheers_Elephant_-_Leaves.Cheers_Elephant_-_Leaves"))
			Sound(TEXT("/Game/We_Will_Rock_You.We_Will_Rock_You"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Property initialization
	Sound = ConstructorStatics.Sound.Object;
	Song = (UAudioComponent*) UAudioComponent::StaticClass()->GetDefaultObject();
	AudioExtractor = (UPCMAudioExtractor*)UPCMAudioExtractor::StaticClass()->GetDefaultObject();
	Buffer.AddZeroed(bufferSize);

	// nOutputBins as defined by the definition of an fft
	nOutputBins = bufferSize / 2 + 1;
	nFFTBinsInPillar = nOutputBins / nFreqBins;
	
	// create fft plans
	in = (double*) malloc(sizeof(double) * bufferSize);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nOutputBins);
	plan = fftw_plan_dft_r2c_1d(bufferSize, in, out, FFTW_MEASURE);

	// create children actors (the cubes)
	UWorld* const World = GetWorld();

	p.Init(0, nPeriods);
	if (World)
	{
		// initialize Floor
		Floor.Add(World->SpawnActor<ARectangularPrism>(ARectangularPrism::StaticClass(), FVector(-500, -500, 100), FRotator(0, 0, 0)));
		// initialize Pillars
		for (int i = 0; i < nFreqBins; i++)
			Pillars.Add(World->SpawnActor<ARectangularPrism>(ARectangularPrism::StaticClass(), FVector(210 * i, 100, 100), FRotator(0,0,0)));
	}

	
	//for debugging
	myfile.open("C:\\Users\\Jon\\rms.txt", ios::out);
	beatFile.open("C:\\Users\\Jon\\beat.txt", ios::out);
	fileNotClosed = true;
	idx = 0;
	beatIdx = 0;
}

// Called when the game starts or when spawned
void ASongContainer::BeginPlay()
{
	Super::BeginPlay();

	Song->SetSound(Sound);
	Song->Play(0);
	currentTime = 0;
}

// Called every frame
void ASongContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float sampleRate = 44100; // samples per second
	const int nSamplesInFrame = (int) (sampleRate * DeltaTime);

	// actually just process 1 frame for now
	//int nBuffersToProcess = (int) (nSamplesInFrame / 1024); // round down
	AudioExtractor->GetAmplitude(Sound, 0, currentTime, DeltaTime, nSamplesInFrame, Buffer);

	// copy elements into fft buffer
	// calculate rms
	float squaredSum = 0;
	float nSamples = 0;
	for (int i = 0; i < bufferSize; ++i)
	{
		if (i >= nSamplesInFrame)
		{
			in[i] = 0;
		}
		else 
		{
			in[i] = Buffer[i];
			nSamples += 1;
			squaredSum += pow(in[i], 2);
		}
	}
	float rms = sqrt(squaredSum / nSamples);
	
	
	//print out rms data so we know where to set the threshold
	myfile << rms << "\t" << currentTime << endl;

	// ===================
	// simple beat tracker
	// ===================
	//
	// onset determined by rms
	float d, p_i, pSum, weightSum;
	const float m = 0.95;
	const float k = 1;
	const float e = 0.3;
	const float timeToDisplayBeat = 0.1;

	// get the first two beats
	if (rms > rmsThreshold)
	{
		if (lastBeatTime && beatNotInitialized)
		{
			p[0] = currentTime - lastBeatTime;
			nextBeatTime = currentTime + p[0];
			beatNotInitialized = false;
		}

		lastBeatTime = currentTime;
	}

	// calculation of beat period according to Pardo 2004 paper
	if (nextBeatTime)
	{
		d = (currentTime - nextBeatTime) / p[0];
		if (abs(d) < e && rms > rmsThreshold)
		{
			pSum = 0;
			weightSum = 0;
			for (int i = 0; i < nPeriods; i++)
			{
				const float weight = pow(m, i);
				pSum += p[i] * weight;
				float bleh = p[i];
				weightSum += weight;
			}
			p_i = (pSum / weightSum) * (1 + k*d);
			p.RemoveAt(nPeriods - 1);
			p.Push(p_i);

			nextBeatTime = lastBeatTime + p[0];
		}
		else if (currentTime > nextBeatTime && rms < rmsThreshold)
		{
			nextBeatTime = lastBeatTime + p[0];
		}

	}

	// execute the fft
	fftw_execute(plan);

	// get magnitudes
	double magTot = 0;
	int nTot = 0;
	int pillarIdx = 0;
	for (int i = 0; i < (bufferSize / 2 + 1); i++)
	{
		double real = out[i][0];
		double imag = out[i][1];
		double mag = sqrt((real * real) + (imag * imag));
		
		magTot += mag;
		nTot += 1;

		// if we have summed up all the bins for this pillar
		// or we are on the last pillar
		if (nTot == nFFTBinsInPillar)
		{
			Pillars[pillarIdx]->SetActorScale3D(FVector(1, 1, magTot / 100000));
			
			magTot = 0;
			nTot = 0;
			pillarIdx += 1;
		}
		//Cubes[i]->SetActorScale3D(FVector(1, 1, mag / 1000.0));
	}

	// determine if a beat is currently happening
	if (abs(currentTime - nextBeatTime) < timeToDisplayBeat)
	{
		// beat has occurred
		Floor[0]->SetActorScale3D(FVector(1, 1, 2));
		if (beatIdx < 20 && lastPrintedBeat != nextBeatTime)
		{
			lastPrintedBeat = nextBeatTime;
			beatFile << nextBeatTime << endl;
			beatIdx++;
		}
		else if (beatIdx >= 20)
		{
			myfile.close();
			beatFile.close();
		}
	}
	else
	{
		// beat has not occurred
		Floor[0]->SetActorScale3D(FVector(1, 1, 1));
	}

	currentTime += DeltaTime;
}

