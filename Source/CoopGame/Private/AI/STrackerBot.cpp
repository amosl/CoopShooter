// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"



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

	m_bUseVelocityChange = false;
	m_MovementForce = 1000;
	m_DistThreshold = 100;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	healthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);
	m_NextPathPoint = GetNextPathPoint();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* ownHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on death

	// TODO: pulse the material on hit

	UE_LOG(LogTemp, Log, TEXT("Health: %s of %s"), *FString::SanitizeFloat(Health), *GetName());
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

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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



