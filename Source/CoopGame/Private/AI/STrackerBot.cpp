// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"



// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetCanEverAffectNavigation(false);
	//meshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = meshComp;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
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

}



