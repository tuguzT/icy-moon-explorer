#include "Damage/JuicyDamageLibrary.h"

FORCEINLINE bool UJuicyDamageLibrary::IsDealingHeal(const FJuicyDealDamage& DamageToDeal)
{
	return DamageToDeal.IsHealing();
}

FORCEINLINE bool UJuicyDamageLibrary::HasAnyEffectToDeal(const FJuicyDealDamage& DamageToDeal, const float Tolerance)
{
	return DamageToDeal.HasAnyEffect(Tolerance);
}

FORCEINLINE bool UJuicyDamageLibrary::IsTakingHeal(const FJuicyTakeDamage& DamageToTake)
{
	return DamageToTake.IsHealing();
}

FORCEINLINE bool UJuicyDamageLibrary::HasAnyEffectToTake(const FJuicyTakeDamage& DamageToTake, const float Tolerance)
{
	return DamageToTake.HasAnyEffect(Tolerance);
}
