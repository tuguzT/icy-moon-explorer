#include "Damage/JuicyDamageLibrary.h"

bool UJuicyDamageLibrary::IsDealingHeal(const FJuicyDealDamage& DamageToDeal)
{
	return DamageToDeal.IsHealing();
}

bool UJuicyDamageLibrary::HasAnyDamageToDeal(const FJuicyDealDamage& DamageToDeal, const float Tolerance)
{
	return DamageToDeal.HasAnyEffect(Tolerance);
}

bool UJuicyDamageLibrary::IsTakingHeal(const FJuicyTakeDamage& DamageToTake)
{
	return DamageToTake.IsHealing();
}

bool UJuicyDamageLibrary::HasAnyDamageToTake(const FJuicyTakeDamage& DamageToTake, const float Tolerance)
{
	return DamageToTake.HasAnyEffect(Tolerance);
}

float UJuicyDamageLibrary::ProcessResistance(const FJuicyDamageResistance& DamageResistance,
                                             const FJuicyTakeDamage& DamageToTake)
{
	return DamageResistance.ProcessDamage(DamageToTake);
}

bool UJuicyDamageLibrary::CanProcessResistance(const FJuicyDamageResistance& DamageResistance,
                                               const FJuicyTakeDamage& DamageToTake)
{
	return DamageResistance.CanProcessDamage(DamageToTake);
}

bool UJuicyDamageLibrary::HasAnyResistance(const FJuicyDamageResistance& DamageResistance)
{
	return DamageResistance.HasAnyEffect();
}

bool UJuicyDamageLibrary::IsResistantToDamage(const FJuicyDamageResistance& DamageResistance)
{
	return DamageResistance.IsResistantToDamage();
}

bool UJuicyDamageLibrary::IsWeakToDamage(const FJuicyDamageResistance& DamageResistance)
{
	return DamageResistance.IsWeakToDamage();
}

bool UJuicyDamageLibrary::IsImmuneToDamage(const FJuicyDamageResistance& DamageResistance)
{
	return DamageResistance.IsImmuneToDamage();
}

bool UJuicyDamageLibrary::CanHealFromDamage(const FJuicyDamageResistance& DamageResistance)
{
	return DamageResistance.CanHealFromDamage();
}
