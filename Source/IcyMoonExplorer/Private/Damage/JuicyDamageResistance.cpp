#include "Damage/JuicyDamageResistance.h"

UJuicyDamageResistance::UJuicyDamageResistance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Multiplier = 1.0f;
}

float UJuicyDamageResistance::ProcessDamage(const FJuicyTakeDamage& DamageToTake) const
{
	if (!CanProcessDamage(DamageToTake))
	{
		return DamageToTake.Damage;
	}
	return DamageToTake.Damage * Multiplier;
}

bool UJuicyDamageResistance::CanProcessDamage(const FJuicyTakeDamage& DamageToTake) const
{
	return DamageToTake.DamageType.IsA(DamageTypeClass);
}

bool UJuicyDamageResistance::HasAnyEffect() const
{
	return Multiplier != 1.0f;
}

bool UJuicyDamageResistance::IsResistantToDamage() const
{
	return Multiplier < 1.0f;
}

bool UJuicyDamageResistance::IsWeakToDamage() const
{
	return Multiplier > 1.0f;
}

bool UJuicyDamageResistance::IsImmuneToDamage() const
{
	return Multiplier == 0.0f;
}

bool UJuicyDamageResistance::CanHealFromDamage() const
{
	return Multiplier < 0.0f;
}
