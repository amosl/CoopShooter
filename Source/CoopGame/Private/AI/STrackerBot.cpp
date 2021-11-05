// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"
#include "SCharacter.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"


// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetCanEverAffectNavigation(false);
	meshComp->SetSimulatePhysics(true);
	RootComponent = meshComp;

	healthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	sphereComp->SetSphereRadius(200);
	sphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	sphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	sphereComp->SetupAttachment(RootComponent);

	m_bUseVelocityChange = false;
	m_MovementForce = 1000;
	m_DistThreshold = 100;
	expDamage = 40;
	expRadius = 200;
	selfDamageInterval = 0.25f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	healthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	if (HasAuthority()) {
		m_NextPathPoint = GetNextPathPoint();
	}
	
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* ownHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on death

	if (matInst == nullptr)
	{
		matInst = meshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, meshComp->GetMaterial(0));
	}

	if (matInst)
		matInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);

	// UE_LOG(LogTemp, Log, TEXT("Health: %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	if (Health <= 0)
		SelfDestruct();
}

void ASTrackerBot::SelfDestruct()
{
	if (bDidExplode)
		return;

	bDidExplode = true;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explodeEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, explodeSound, GetActorLocation());

	meshComp->SetVisibility(false, true);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		TArray<AActor*> ignoreList;
		ignoreList.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, expDamage, GetActorLocation(), expRadius, nullptr, ignoreList, this, GetInstigatorController(), true);

		DrawDebugSphere(GetWorld(), GetActorLocation(), expRadius, 12, FColor::Red, false, 2, 0, 1);
		
		SetLifeSpan(2.0f);
	}
	
}

FVector ASTrackerBot::GetNextPathPoint()
{
	// Hack to get player location
	ACharacter* playerPawn= UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* navPath= UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), playerPawn);

	if (navPath->PathPoints.Num() > 1)
	{
		return navPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !bDidExplode)
	{
		float distToTarget = (GetActorLocation() - m_NextPathPoint).Size();

		if (distToTarget <= m_DistThreshold)
		{
			m_NextPathPoint = GetNextPathPoint();
			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
			//return;
		}
		else {
			// keep moving
			FVector forceDir = m_NextPathPoint - GetActorLocation();
			forceDir.Normalize();
			forceDir *= m_MovementForce;

			meshComp->AddForce(forceDir, NAME_None, m_bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + forceDir, 32, FColor::Yellow, false, 0, 0, 1);
		}


		DrawDebugSphere(GetWorld(), m_NextPathPoint, 20, 12, FColor::Yellow, false, 0, 1);
	}
	
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (startedSelfDestruct || bDidExplode)
		return;

	ASCharacter* playerPawn = Cast<ASCharacter>(OtherActor);
	if (playerPawn)
	{
		if (HasAuthority())
		{
			// Start sd sequence
			GetWorldTimerManager().SetTimer(timerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, selfDamageInterval, true, 0);
		}

		startedSelfDestruct = true;
		UGameplayStatics::SpawnSoundAttached(selfDestructSound, RootComponent);
	}
}



