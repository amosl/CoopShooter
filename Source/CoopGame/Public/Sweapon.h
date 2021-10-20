// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sweapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

UCLASS()
class COOPGAME_API ASweapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASweapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> damageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName muzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName tracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* meshComp;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* impactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* muzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* tracerEffect;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
