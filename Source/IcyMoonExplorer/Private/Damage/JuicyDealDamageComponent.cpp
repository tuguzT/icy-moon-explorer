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

	AActor* const Attacker = GetOwner();
	AController* const AttackerInstigator = Attacker->GetInstigatorController();
	const FDamageEvent DamageEvent;
	ActorToDamage->TakeDamage(Damage, DamageEvent, AttackerInstigator, Attacker);
}
