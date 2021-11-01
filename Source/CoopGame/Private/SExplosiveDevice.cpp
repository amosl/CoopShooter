// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveDevice.h"
#include "Components/SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplosiveDevice::ASExplosiveDevice()
{
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetSimulatePhysics(true);
	meshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = meshComp;

	healthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	bDidExplode = false;

	radialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	radialForceComp->bAutoActivate = false;
	radialForceComp->SetupAttachment(RootComponent);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASExplosiveDevice::BeginPlay()
{
	Super::BeginPlay();
	
	healthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveDevice::OnHealthChanged);
}


void ASExplosiveDevice::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	
	if (Health <= 0.0f && !bDidExplode)
	{
		//UE_LOG(LogTemp, Log, TEXT("*** Boom ***"));
		// Die
		bDidExplode = true;
		OnRep_Exploded();

		// Apply force
		float launchForce = 400;
		FVector launchDir = FVector::UpVector;
		meshComp->AddImpulse(launchDir * launchForce, NAME_None, true);

		radialForceComp->FireImpulse();
	}
}

void ASExplosiveDevice::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explodeEffect, GetActorLocation());

	// Switch material
	if (explodeMaterial)
	{
		meshComp->SetMaterial(0, explodeMaterial);
	}
}

void ASExplosiveDevice::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveDevice, bDidExplode);
}