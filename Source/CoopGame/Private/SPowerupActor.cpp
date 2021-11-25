// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	powerupInterval = 0;
	totalNumOfTicks = 0;
	bIsPowerupActive = false;

	SetReplicates(true);
}

void ASPowerupActor::ActivatePowerup()
{
	OnActivated();

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (powerupInterval > 0)
	{
		GetWorldTimerManager().SetTimer(timerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, powerupInterval, true);
	}
	else
		OnTickPowerup();
}

	

void ASPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void ASPowerupActor::OnTickPowerup()
{
	ticksProcessed++;
	OnPowerupTicked();

	if (ticksProcessed >= totalNumOfTicks)
	{
		OnExpired();

		bIsPowerupActive = false;
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(timerHandle_PowerupTick);
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
	
}

