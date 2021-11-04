// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	m_DefaultHealth = 100;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// only hook on server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* owner = GetOwner();
		if (owner)
		{
			owner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeDamage);
		}
	}

	
	
	m_Health = m_DefaultHealth;
}

void USHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0)
		return;

	m_Health = FMath::Clamp(m_Health - Damage, 0.0f, m_DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Helath Changed: %s"), *FString::SanitizeFloat(m_Health));

	OnHealthChanged.Broadcast(this, m_Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, m_Health);
}


