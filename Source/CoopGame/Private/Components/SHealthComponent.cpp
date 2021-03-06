// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	m_DefaultHealth = 100;
	bIsDead = false;
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

void USHealthComponent::OnRep_Health(float prevHealth)
{
	OnHealthChanged.Broadcast(this, m_Health, m_Health-prevHealth, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0 || bIsDead)
		return;

	m_Health = FMath::Clamp(m_Health - Damage, 0.0f, m_DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Helath Changed: %s"), *FString::SanitizeFloat(m_Health));

	OnHealthChanged.Broadcast(this, m_Health, Damage, DamageType, InstigatedBy, DamageCauser);

	bIsDead = m_Health <= 0;

	if (bIsDead)
	{
		ASGameMode* gm = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (gm)
		{
			gm->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
	
}

void USHealthComponent::Heal(float amount)
{
	if (amount <= 0.0f || m_Health <= 0.0f)
		return;

	m_Health = FMath::Clamp(m_Health + amount, 0.0f, m_DefaultHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(m_Health), *FString::SanitizeFloat(amount));

	OnHealthChanged.Broadcast(this, m_Health, -amount, nullptr, nullptr, nullptr);
}

float USHealthComponent::GetHealth() const
{
	return m_Health;
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, m_Health);
}


