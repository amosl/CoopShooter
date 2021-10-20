// Fill out your copyright notice in the Description page of Project Settings.


#include "Sweapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASweapon::ASweapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = meshComp;

	muzzleSocketName = "MuzzleSocket";
	tracerTargetName = "BeamEnd";
}

// Called when the game starts or when spawned
void ASweapon::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASweapon::Fire()
{
	// Trace the world, from pawn eye to target

	AActor* owner = GetOwner();

	if (owner)
	{
		FVector eyeLocation;
		FRotator eyeRotation;
		owner->GetActorEyesViewPoint(eyeLocation, eyeRotation);

		FVector shortDirection = eyeRotation.Vector();
		FVector traceEnd = eyeLocation + (shortDirection * 10000);
		FVector tracerEndPoint = traceEnd;

		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(owner);
		queryParams.AddIgnoredActor(this);
		queryParams.bTraceComplex = true;

		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, eyeLocation, traceEnd, ECC_Visibility, queryParams))
		{
			AActor* hitActor = hit.GetActor();
			UGameplayStatics::ApplyPointDamage(hitActor, 20.f, shortDirection, hit, owner->GetInstigatorController(), this, damageType);

			if (impactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactEffect, hit.ImpactPoint, hit.ImpactNormal.Rotation());
			}	
			tracerEndPoint = hit.ImpactPoint;
		}
	

		DrawDebugLine(GetWorld(), eyeLocation, traceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		if (muzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(muzzleEffect, meshComp, muzzleSocketName);
		}	

		if (tracerEffect)
		{
			FVector muzzlePos = meshComp->GetSocketLocation(muzzleSocketName);
			UParticleSystemComponent* tracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), tracerEffect, muzzlePos);
			if (tracerComp)
			{
				tracerComp->SetVectorParameter(tracerTargetName, tracerEndPoint);
			}
		}
		
	}
	
}

// Called every frame
void ASweapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

