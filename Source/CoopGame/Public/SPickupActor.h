// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerupActor;


UCLASS()
class COOPGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* sphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* decalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<ASPowerupActor> powerupClass;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
	ASPowerupActor* powerupInstance;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float cooldownDuration;

	FTimerHandle timerHandle_Respawn;

	UFUNCTION()
	void Respawn();
};
