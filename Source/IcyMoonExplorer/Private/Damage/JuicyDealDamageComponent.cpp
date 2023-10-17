#include "Damage/JuicyDealDamageComponent.h"

UJuicyDealDamageComponent::UJuicyDealDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
}

// ReSharper disable once CppPassValueParameterByConstReference
void UJuicyDealDamageComponent::DealDamage(AActor* ActorToDamage, const float Damage,
                                           const FDamageEvent DamageEvent) const
{
	if (ActorToDamage == nullptr)
	{
		return;
	}

	AActor* const Attacker = GetOwner();
	AController* const AttackerInstigator = Attacker->GetInstigatorController();
	ActorToDamage->TakeDamage(Damage, DamageEvent, AttackerInstigator, Attacker);
}
