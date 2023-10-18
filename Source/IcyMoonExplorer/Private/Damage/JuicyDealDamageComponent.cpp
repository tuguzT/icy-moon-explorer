#include "Damage/JuicyDealDamageComponent.h"

#include "Engine/DamageEvents.h"

UJuicyDealDamageComponent::UJuicyDealDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
}

void UJuicyDealDamageComponent::DealDamage(AActor* ActorToDamage, const float Damage) const
{
	if (ActorToDamage == nullptr)
	{
		return;
	}

	const FDamageEvent DamageEvent;
	AActor* const Attacker = GetOwner();
	AController* const Instigator = Attacker->GetInstigatorController();
	ActorToDamage->TakeDamage(Damage, DamageEvent, Instigator, Attacker);
}
