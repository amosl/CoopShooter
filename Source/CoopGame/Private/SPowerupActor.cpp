// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	powerupInterval = 0;
	totalNumOfTicks = 0;

}

void ASPowerupActor::ActivatePowerup()
{
	OnActivated();

	if (powerupInterval > 0)
	{
		GetWorldTimerManager().SetTimer(timerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, powerupInterval, true);
	}
	else
		OnTickPowerup();
}


// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}
	

void ASPowerupActor::OnTickPowerup()
{
	ticksProcessed++;
	OnPowerupTicked();

	if (ticksProcessed >= totalNumOfTicks)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(timerHandle_PowerupTick);
	}
}



