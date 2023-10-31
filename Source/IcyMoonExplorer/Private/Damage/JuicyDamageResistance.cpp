#include "Damage/JuicyDamageResistance.h"

FJuicyDamageResistance::FJuicyDamageResistance()
	: FJuicyDamageResistance{UDamageType::StaticClass(), 1.0f}
{
}

FJuicyDamageResistance::FJuicyDamageResistance(const TSubclassOf<UDamageType> DamageTypeClass,
                                               const float Multiplier)
	: DamageTypeClass{DamageTypeClass},
	  Multiplier{Multiplier}
{
}

FJuicyDamageResistance::~FJuicyDamageResistance()
{
}

float FJuicyDamageResistance::ProcessDamage(const FJuicyTakeDamage& DamageToTake) const
{
	if (!CanProcessDamage(DamageToTake))
	{
		return DamageToTake.Damage;
	}
	return DamageToTake.Damage * Multiplier;
}

bool FJuicyDamageResistance::CanProcessDamage(const FJuicyTakeDamage& DamageToTake) const
{
	return DamageToTake.DamageType.IsA(DamageTypeClass);
}

bool FJuicyDamageResistance::HasAnyEffect() const
{
	return Multiplier != 1.0f;
}

bool FJuicyDamageResistance::IsResistantToDamage() const
{
	return Multiplier < 1.0f;
}

bool FJuicyDamageResistance::IsWeakToDamage() const
{
	return Multiplier > 1.0f;
}

bool FJuicyDamageResistance::IsImmuneToDamage() const
{
	return Multiplier == 0.0f;
}

bool FJuicyDamageResistance::CanHealFromDamage() const
{
	return Multiplier < 0.0f;
}
