// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Sweapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	armComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	armComp->bUsePawnControlRotation = true;
	armComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	cameraComp->SetupAttachment(armComp);

	targetZoomFOV = 65.f;
	interpSpeed = 20.0f;
	wepAttachSocketName = "weapon_r_socket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	defaultFOV = cameraComp->FieldOfView;

	// Spawn a def weapon
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	currentWeapon = GetWorld()->SpawnActor<ASweapon>(starterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, params);
	if (currentWeapon)
	{
		currentWeapon->SetOwner(this);
		currentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, wepAttachSocketName);
	}

}

void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ASCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bWantToZoom = true;
}


void ASCharacter::EndZoom()
{
	bWantToZoom = false;
}

void ASCharacter::Fire()
{
	if (currentWeapon)
	{
		currentWeapon->Fire();
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float targetFOV = bWantToZoom ? targetZoomFOV : defaultFOV;
	float newFOV = FMath::FInterpTo(cameraComp->FieldOfView, targetFOV, DeltaTime, interpSpeed);
	cameraComp->SetFieldOfView(newFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::Fire);
}


FVector ASCharacter::GetPawnViewLocation() const
{
	if (cameraComp)
	{
		return cameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}


