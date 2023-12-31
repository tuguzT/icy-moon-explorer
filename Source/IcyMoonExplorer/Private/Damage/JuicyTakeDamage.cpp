﻿#include "Damage/JuicyTakeDamage.h"

namespace Detail
{
	static const UDamageType* DefaultDamageType()
	{
		return UDamageType::StaticClass()->GetDefaultObject<UDamageType>();
	}
}

FJuicyTakeDamage::FJuicyTakeDamage()
	: FJuicyTakeDamage{0.0f, Detail::DefaultDamageType(), nullptr, nullptr}
{
}

FJuicyTakeDamage::FJuicyTakeDamage(const float Damage,
                                   const UDamageType* DamageType,
                                   AActor* DamageDealer,
                                   AController* InstigatedBy)
	: Damage{Damage},
	  DamageType{DamageType},
	  DamageDealer{DamageDealer},
	  InstigatedBy{InstigatedBy}
{
}

FJuicyTakeDamage::~FJuicyTakeDamage()
{
}

bool FJuicyTakeDamage::IsHealing() const
{
	return Damage < 0.0f;
}

bool FJuicyTakeDamage::HasAnyEffect(const float Tolerance) const
{
	return FMath::Abs(Damage) <= Tolerance;
}
