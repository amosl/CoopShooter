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
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

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
	baseDamage = 20;
	rateOfFire = 600;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ASweapon::BeginPlay()
{
	Super::BeginPlay();

	timeBetweenShots = 60 / rateOfFire;
}




void ASweapon::Fire()
{
	// Trace the world, from pawn eye to target

	if (!HasAuthority() )
	{
		ServerFire();
	}

	AActor* owner = GetOwner();

	if (owner)
	{
		FVector eyeLocation;
		FRotator eyeRotation;
		owner->GetActorEyesViewPoint(eyeLocation, eyeRotation);

		FVector shortDirection = eyeRotation.Vector();
		FVector traceEnd = eyeLocation + (shortDirection * 10000);
		FVector tracerEndPoint = traceEnd;
		EPhysicalSurface surfaceType = SurfaceType_Default;

		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(owner);
		queryParams.AddIgnoredActor(this);
		queryParams.bTraceComplex = true;
		queryParams.bReturnPhysicalMaterial = true;

		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, eyeLocation, traceEnd, COLLISION_WEAPON, queryParams))
		{
			AActor* hitActor = hit.GetActor();

			surfaceType = UPhysicalMaterial::DetermineSurfaceType(hit.PhysMaterial.Get());
			float actualDamage = baseDamage;
			if (surfaceType == Surface_FleshVunerable)
			{
				actualDamage *= 4.0f;
			}


			UGameplayStatics::ApplyPointDamage(hitActor, actualDamage, shortDirection, hit, owner->GetInstigatorController(), this, damageType);

			PlayImpactFX(surfaceType, hit.ImpactPoint);
			

			tracerEndPoint = hit.ImpactPoint;
			hitScanTrace.surfaceType = surfaceType;
		}
	
		if (debugWeaponDraw > 0)
		{
			DrawDebugLine(GetWorld(), eyeLocation, traceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
		
		PlayFireFX(tracerEndPoint);

		if (HasAuthority())
		{
			hitScanTrace.traceTo = tracerEndPoint;
			hitScanTrace.surfaceType = surfaceType;
		}

		lastFireTime = GetWorld()->TimeSeconds;
	}
	
}

void ASweapon::OnRep_HitScanTrace()
{
	// Play cosmetic fx
	PlayFireFX(hitScanTrace.traceTo);
	PlayImpactFX(hitScanTrace.surfaceType, hitScanTrace.traceTo);
}

void ASweapon::ServerFire_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT(">>> ServerFire"));
	Fire();
}

bool ASweapon::ServerFire_Validate()
{
	return true;
}

void ASweapon::StartFire()
{
	float firstDelay = FMath::Max(0.f, lastFireTime + timeBetweenShots - GetWorld()->TimeSeconds);
	GetWorldTimerManager().SetTimer(timerh_shot, this, &ASweapon::Fire, timeBetweenShots, true, firstDelay);
}

void ASweapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(timerh_shot);
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

void ASweapon::PlayImpactFX(EPhysicalSurface surfaceType, FVector impactPoint)
{
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
		FVector muzzlePos = meshComp->GetSocketLocation(muzzleSocketName);
		FVector shotDirection = impactPoint - muzzlePos;
		shotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), selEffect, impactPoint, shotDirection.Rotation());
	}
}

void ASweapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASweapon, hitScanTrace, COND_SkipOwner);
}
