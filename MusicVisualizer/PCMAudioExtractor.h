// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PCMAudioExtractor.generated.h"

/**
 * 
 */
UCLASS()
class MUSICVISUALIZER_API UPCMAudioExtractor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static void GetAmplitude(USoundWave* SoundWave, const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray< TArray<float> >& OutAmplitudes);

	/** Gathers the amplitude of the wave data for a window of time for the SoundWave
	* @param SoundWave - The wave to get samples from
	* @param Channel - The channel of the sound to get.  Specify 0 to combine channels together
	* @param StartTime - The beginning of the window to get the amplitude from
	* @param TimeLength - The duration of the window to get the amplitude from
	* @param AmplitudeBuckets - How many samples to divide the data in to.  The amplitude is averaged from the wave samples for each bucket
	* @return OutAmplitudes - The resulting amplitudes
	*/
	UFUNCTION(BlueprintCallable, Category = "SoundVisualization")
	static void GetAmplitude(USoundWave* SoundWave, int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets, TArray<float>& OutAmplitudes);
};
