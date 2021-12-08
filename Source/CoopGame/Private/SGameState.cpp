// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"

void ASGameState::OnRep_WaveState(EWaveState fromState)
{
	WaveStateChanged(waveState, fromState);
}

void ASGameState::SetWaveState(EWaveState toState)
{
	if (HasAuthority())
	{
		EWaveState oldState = waveState;
		waveState = toState;
		OnRep_WaveState(oldState);
	}
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, waveState);
}
