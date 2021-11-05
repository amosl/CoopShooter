// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"

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

}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// TODO: grant powerup is available
}


