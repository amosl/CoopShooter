// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Tick interval
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float powerupInterval;

	// Total times we apply powerups
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 totalNumOfTicks;
	int32 ticksProcessed;

	FTimerHandle timerHandle_PowerupTick;

	
	void OnTickPowerup();
public:	
	
	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();
};
