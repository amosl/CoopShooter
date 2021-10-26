// Fill out your copyright notice in the Description page of Project Settings.


#include "Sweapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "../CoopGame.h"


static int32 debugWeaponDraw = 0;

FAutoConsoleVariableRef  CVARDebugWeaponDraw(
	TEXT("COOP.DebugWeapons"),
	debugWeaponDraw,
	TEXT("Draw Debug lines for weapons"),
	ECVF_Cheat);

const int Surface_FleshDefault = 1;
const int Surface_FleshVunerable = 2;

// Sets default values
ASweapon::ASweapon()
{
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = meshComp;

	muzzleSocketName = "MuzzleSocket";
	tracerTargetName = "BeamEnd";
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
		queryParams.bReturnPhysicalMaterial = true;

		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, eyeLocation, traceEnd, COLLISION_WEAPON, queryParams))
		{
			AActor* hitActor = hit.GetActor();
			UGameplayStatics::ApplyPointDamage(hitActor, 20.f, shortDirection, hit, owner->GetInstigatorController(), this, damageType);

			EPhysicalSurface surfaceType = UPhysicalMaterial::DetermineSurfaceType(hit.PhysMaterial.Get());

			UParticleSystem* selEffect = nullptr;
			switch (surfaceType)
			{
			case Surface_FleshDefault:
			case Surface_FleshVunerable:
				selEffect = fleshImpactEffect;
				break;
			default:
				selEffect = defImpactEffect;
				break;
			}

			if (selEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), selEffect, hit.ImpactPoint, hit.ImpactNormal.Rotation());
			}

			tracerEndPoint = hit.ImpactPoint;
		}
	
		if (debugWeaponDraw > 0)
		{
			DrawDebugLine(GetWorld(), eyeLocation, traceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
		
		PlayFireFX(tracerEndPoint);
	}
	
}

void ASweapon::PlayFireFX(FVector endPoint)
{
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
			tracerComp->SetVectorParameter(tracerTargetName, endPoint);
		}
	}

	APawn* owner = Cast<APawn>(GetOwner());
	if (owner)
	{
		APlayerController* pc = Cast<APlayerController>(owner->GetController());
		if (pc)
		{
			pc->ClientStartCameraShake(fireCamShake);
		}
	}
}


