// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Components/SHealthComponent.h"
#include "SGameState.h"
#include "SPlayerState.h"


ASGameMode::ASGameMode()
{
	timeBetweenWaves = 2.0f;

	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ASGameMode::StartWave()
{
	waveCount++;
	numToSpawn = 2 * waveCount;
	GetWorldTimerManager().SetTimer(timer_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(timer_BotSpawner);
	SetWaveState(EWaveState::WaitToComplete);
}

void ASGameMode::PrepareNextWave()
{
	SetWaveState(EWaveState::Idle);
	GetWorldTimerManager().SetTimer(timer_NextWaveStart, this, &ASGameMode::StartWave, timeBetweenWaves, false);
}

void ASGameMode::CheckWaveState()
{
	bool waveUnderway = GetWorldTimerManager().IsTimerActive(timer_NextWaveStart);
	if (numToSpawn > 0 || waveUnderway)
	{
		return;
	}

	bool anyBotAlive = false;

	for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; it++)
	{
		APawn* testPawn = it->Get();
		if (testPawn == nullptr || testPawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* healthComp = Cast<USHealthComponent>( testPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (healthComp && healthComp->GetHealth() > 0)
		{
			anyBotAlive = true;
			break;
		}
	}

	if (!anyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareNextWave();
	}
	
}

void ASGameMode::CheckAnyPlayersAlive()
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pcont = it->Get();
		if (pcont && pcont->GetPawn())
		{
			APawn* pawn = pcont->GetPawn();
			USHealthComponent* healthComp = Cast<USHealthComponent>(pawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(healthComp))
			{
				return;
			}
		}
	}
	
	// No player alive
	GameOver();

}

void ASGameMode::GameOver()
{
	EndWave();
	SetWaveState(EWaveState::GameOver);
	UE_LOG(LogTemp, Log, TEXT("Game Over!!!!"));
}

void ASGameMode::SetWaveState(EWaveState newState)
{
	ASGameState* gs = GetGameState<ASGameState>();
	if (ensureAlways(gs))
	{
		gs->SetWaveState(newState);
	}
}


void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	CheckWaveState();
	CheckAnyPlayersAlive();
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	numToSpawn--;
	if (numToSpawn <= 0)
	{
		EndWave();
	}
}
