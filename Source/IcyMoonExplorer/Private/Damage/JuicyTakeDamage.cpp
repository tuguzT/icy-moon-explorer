#include "Damage/JuicyTakeDamage.h"

bool FJuicyTakeDamage::IsHealing() const
{
	return Damage < 0.0f;
}

bool FJuicyTakeDamage::HasAnyEffect(const float Tolerance) const
{
	return FMath::Abs(Damage) <= Tolerance;
}
