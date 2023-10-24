#include "Damage/JuicyDealDamage.h"

bool FJuicyDealDamage::IsHealing() const
{
	return Damage < 0.0f;
}

bool FJuicyDealDamage::HasAnyEffect(const float Tolerance) const
{
	return FMath::Abs(Damage) <= Tolerance;
}
