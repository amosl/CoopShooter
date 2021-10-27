// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASweapon;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void BeginCrouch(); 
	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* cameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* armComp;

	USHealthComponent* healthComp;

	bool bWantToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float targetZoomFOV;
	
	float defaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta= (ClampMin=0.1, ClampMax= 100))
	float interpSpeed;

	void BeginZoom();
	void EndZoom();

	ASweapon* currentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASweapon> starterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName wepAttachSocketName;
	
	void StartFire();
	void StopFire();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bIsDead;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual FVector GetPawnViewLocation() const override;

};
