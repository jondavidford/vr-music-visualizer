// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicVisualizer.h"
#include "PCMAudioExtractor.h"

void UPCMAudioExtractor::GetAmplitude(USoundWave* SoundWave, int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets, TArray<float>& OutAmplitudes)
{
	OutAmplitudes.Empty();

#if WITH_EDITORONLY_DATA
	if (SoundWave)
	{
		if (Channel >= 0)
		{
			TArray< TArray<float> > Amplitudes;

			GetAmplitude(SoundWave, (Channel != 0), StartTime, TimeLength, AmplitudeBuckets, Amplitudes);

			if (Channel == 0)
			{
				OutAmplitudes = Amplitudes[0];
			}
			else if (Channel <= Amplitudes.Num())
			{
				OutAmplitudes = Amplitudes[Channel - 1];
			}
			else
			{
				//UE_LOG(LogSoundVisualization, Warning, TEXT("Requested channel %d, sound only has %d channels"), SoundWave->NumChannels);
			}
		}
		else
		{
			//UE_LOG(LogSoundVisualization, Warning, TEXT("Invalid Channel (%d)"), Channel);
		}
	}
#else
	UE_LOG(LogSoundVisualization, Warning, TEXT("Get Amplitude does not work for cooked builds yet."));
#endif
}

void UPCMAudioExtractor::GetAmplitude(USoundWave* SoundWave, const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray< TArray<float> >& OutAmplitudes)
{

	OutAmplitudes.Empty();

#if WITH_EDITORONLY_DATA
	const int32 NumChannels = SoundWave->NumChannels;
	if (AmplitudeBuckets > 0 && NumChannels > 0)
	{
		// Setup the output data
		OutAmplitudes.AddZeroed((bSplitChannels ? NumChannels : 1));
		for (int32 ChannelIndex = 0; ChannelIndex < OutAmplitudes.Num(); ++ChannelIndex)
		{
			OutAmplitudes[ChannelIndex].AddZeroed(AmplitudeBuckets);
		}

		// check if there is any raw sound data
		if (SoundWave->RawData.GetBulkDataSize() > 0)
		{
			// Lock raw wave data.
			uint8* RawWaveData = (uint8*)SoundWave->RawData.Lock(LOCK_READ_ONLY);
			int32 RawDataSize = SoundWave->RawData.GetBulkDataSize();
			FWaveModInfo WaveInfo;

			// parse the wave data
			if (WaveInfo.ReadWaveHeader(RawWaveData, RawDataSize, 0))
			{
				uint32 SampleCount = 0;
				uint32 SampleCounts[10] = { 0 };

				uint32 FirstSample = *WaveInfo.pSamplesPerSec * StartTime;
				uint32 LastSample = *WaveInfo.pSamplesPerSec * (StartTime + TimeLength);

				if (NumChannels <= 2)
				{
					SampleCount = WaveInfo.SampleDataSize / (2 * NumChannels);
				}
				else
				{
					for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
					{
						SampleCounts[ChannelIndex] = (SoundWave->ChannelSizes[ChannelIndex] / 2);
						SampleCount = FMath::Max(SampleCount, SampleCounts[ChannelIndex]);
						SampleCounts[ChannelIndex] -= FirstSample;
					}
				}

				FirstSample = FMath::Min(SampleCount, FirstSample);
				LastSample = FMath::Min(SampleCount, LastSample);

				int16* SamplePtr = reinterpret_cast<int16*>(WaveInfo.SampleDataStart);
				if (NumChannels <= 2)
				{
					SamplePtr += FirstSample;
				}

				uint32 SamplesPerAmplitude = (LastSample - FirstSample) / AmplitudeBuckets;
				uint32 ExcessSamples = (LastSample - FirstSample) % AmplitudeBuckets;

				for (int32 AmplitudeIndex = 0; AmplitudeIndex < AmplitudeBuckets; ++AmplitudeIndex)
				{
					if (NumChannels <= 2)
					{
						int64 SampleSum[2] = { 0 };
						uint32 SamplesToRead = SamplesPerAmplitude + (ExcessSamples-- > 0 ? 1 : 0);
						for (uint32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex)
						{
							for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
							{
								SampleSum[ChannelIndex] += FMath::Abs(*SamplePtr);
								SamplePtr++;
							}
						}
						for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
						{
							OutAmplitudes[(bSplitChannels ? ChannelIndex : 0)][AmplitudeIndex] = SampleSum[ChannelIndex] / (float)SamplesToRead;
						}
					}
					else
					{
						uint32 SamplesRead = 0;
						int64 SampleSum = 0;
						uint32 SamplesToRead = SamplesPerAmplitude + (ExcessSamples-- > 0 ? 1 : 0);
						for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
						{
							uint32 SamplesToReadForChannel = FMath::Min(SamplesToRead, SampleCounts[ChannelIndex]);

							if (SamplesToReadForChannel > 0)
							{
								if (bSplitChannels)
								{
									SampleSum = 0;
								}

								for (uint32 SampleIndex = 0; SampleIndex < SamplesToReadForChannel; ++SampleIndex)
								{
									SampleSum += FMath::Abs(*(SamplePtr + FirstSample + SampleIndex + SoundWave->ChannelOffsets[ChannelIndex] / 2));
								}

								if (bSplitChannels)
								{
									OutAmplitudes[ChannelIndex][AmplitudeIndex] = SampleSum / (float)SamplesToReadForChannel;
								}
								SamplesRead += SamplesToReadForChannel;
								SampleCounts[ChannelIndex] -= SamplesToReadForChannel;
							}
						}

						if (!bSplitChannels)
						{
							OutAmplitudes[0][AmplitudeIndex] = SampleSum / (float)SamplesRead;
						}

						FirstSample += SamplesToRead;
					}
				}
			}

			SoundWave->RawData.Unlock();
		}
	}
#else
	UE_LOG(LogSoundVisualization, Warning, TEXT("Get Amplitude does not work for cooked builds yet."));
#endif
}