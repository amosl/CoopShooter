// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;
class UParticleSystem;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* meshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* healthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* sphereComp;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* ownHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
		class AController* InstigatedBy, AActor* DamageCauser);

	void SelfDestruct();

	FVector GetNextPathPoint();

	FVector m_NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float m_MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float m_DistThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool m_bUseVelocityChange;
	
	// Dynamic material to pulse damage
	UMaterialInstanceDynamic* matInst;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* explodeEffect;

	//UPROPERTY(ReplicatedUsing = OnRep_Exploded)
	bool bDidExplode;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float expRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float expDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float selfDamageInterval;

	FTimerHandle timerHandle_SelfDamage;
	bool startedSelfDestruct;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* selfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* explodeSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
