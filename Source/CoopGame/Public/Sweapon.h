// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sweapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShakeBase;


// Info for single weapon line trace
USTRUCT()
struct FhitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> surfaceType;

	UPROPERTY()
	FVector_NetQuantize traceTo;
};

UCLASS()
class COOPGAME_API ASweapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASweapon();

protected:
	
	virtual void BeginPlay() override;
	

	void PlayFireFX(FVector endPoint);
	void PlayImpactFX(EPhysicalSurface surface, FVector endPoint);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> damageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName muzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName tracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* meshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float baseDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* defImpactEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* fleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* muzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* tracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShakeBase> fireCamShake;

	FTimerHandle timerh_shot;
	float lastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float rateOfFire;
	float timeBetweenShots;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FhitScanTrace hitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

public:	
	
	

	void StartFire();
	void StopFire();
};
