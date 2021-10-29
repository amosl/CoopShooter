// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveDevice.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class URadialForceComponent;
class UParticleSystem;
class UMaterialInterface;

UCLASS()
class COOPGAME_API ASExplosiveDevice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveDevice();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* meshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USHealthComponent* healthComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent* radialForceComp;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* ownHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "Barrel")
	bool bDidExplode;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* explodeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UMaterialInterface* explodeMaterial;
	

public:	
	

};
