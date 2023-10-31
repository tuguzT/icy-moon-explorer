#include "Damage/JuicyDealDamage.h"

FJuicyDealDamage::FJuicyDealDamage()
	: FJuicyDealDamage{0.0f, UDamageType::StaticClass()}
{
}

FJuicyDealDamage::FJuicyDealDamage(const float Damage,
                                   const TSubclassOf<UDamageType> DamageTypeClass)
	: Damage{Damage},
	  DamageTypeClass{DamageTypeClass}
{
}

FJuicyDealDamage::~FJuicyDealDamage()
{
}

bool FJuicyDealDamage::IsHealing() const
{
	return Damage < 0.0f;
}

bool FJuicyDealDamage::HasAnyEffect(const float Tolerance) const
{
	return FMath::Abs(Damage) <= Tolerance;
}
