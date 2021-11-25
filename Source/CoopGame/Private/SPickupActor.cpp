// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "SPowerupActor.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	sphereComp->SetSphereRadius(75);
	RootComponent = sphereComp;

	decalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	decalComp->SetRelativeRotation(FRotator(90.f, 0.0f, 0.0f));
	decalComp->DecalSize = FVector(64, 75, 75);
	decalComp->SetupAttachment(RootComponent);

	cooldownDuration = 10.0f;
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	Respawn();
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (powerupInstance)
	{
		powerupInstance->ActivatePowerup();
		powerupInstance = nullptr;
	}

	// Set timer to respawn
	GetWorldTimerManager().SetTimer(timerHandle_Respawn, this, &ASPickupActor::Respawn, cooldownDuration);
}

void ASPickupActor::Respawn()
{
	if (powerupClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Powerup Class must be set."));
		return;
	}

	FActorSpawnParameters sParams;
	sParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	powerupInstance= GetWorld()->SpawnActor<ASPowerupActor>(powerupClass, GetTransform(), sParams);
}


