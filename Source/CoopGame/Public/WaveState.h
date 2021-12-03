#pragma once



UENUM(BlueprintType)
enum class EWaveState : uint8
{
	Idle,
	WaveInProgress,
	WaitToComplete, // Done spawning, wait until all are killed
	WaveComplete,
	GameOver,
};
