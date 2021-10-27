// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	m_DefaultHealth = 100;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	AActor* owner = GetOwner();
	if (owner)
	{
		owner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeDamage);
	}
	
	m_Health = m_DefaultHealth;
}

void USHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0)
		return;

	m_Health = FMath::Clamp(m_Health - Damage, 0.0f, m_DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Helath Changed: %s"), *FString::SanitizeFloat(m_Health));
}



