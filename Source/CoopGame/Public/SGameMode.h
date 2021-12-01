// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	FTimerHandle timer_BotSpawner;
	FTimerHandle timer_NextWaveStart;

	int numToSpawn;
	int waveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	int timeBetweenWaves;

protected:

	// Hook to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	void StartWave();
	void EndWave();
	void PrepareNextWave();
	void CheckWaveState();

public:
	ASGameMode();
	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
