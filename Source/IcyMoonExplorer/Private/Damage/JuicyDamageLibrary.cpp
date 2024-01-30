#include "Damage/JuicyDamageLibrary.h"

bool UJuicyDamageLibrary::CanDealDamageTo(const AActor* Dealer,
                                          const AActor* Target,
                                          const AController* DealerInstigator,
                                          const AController* TargetInstigator,
                                          const bool bCanDealDamageToSelf,
                                          const TEnumAsByte<ETeamAttitude::Type> CanDealDamageByTeamAttitude)
{
	// ReSharper disable once CppTooWideScopeInitStatement
	const bool bIsSelf =
		Dealer == Target ||
		DealerInstigator == TargetInstigator;
	if (!bCanDealDamageToSelf && bIsSelf)
	{
		return false;
	}

	const auto TeamAttitude = FGenericTeamId::GetAttitude(Dealer, Target);
	const auto InstigatorTeamAttitude = FGenericTeamId::GetAttitude(DealerInstigator, TargetInstigator);
	// ReSharper disable once CppTooWideScopeInitStatement
	const bool bCanDealDamageByTeamAttitude =
		TeamAttitude >= CanDealDamageByTeamAttitude &&
		InstigatorTeamAttitude >= CanDealDamageByTeamAttitude;
	if (!bCanDealDamageByTeamAttitude)
	{
		return false;
	}

	return true;
}

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
                                             const FJuicyTakeDamage& DamageToTake,
                                             const AActor* DamagedActor)
{
	return DamageResistance.ProcessDamage(DamageToTake, DamagedActor);
}

float UJuicyDamageLibrary::ProcessResistances(const TArray<FJuicyDamageResistance>& DamageResistances,
                                              const FJuicyTakeDamage& DamageToTake,
                                              const AActor* DamagedActor)
{
	FJuicyTakeDamage DamageToProcess = DamageToTake;
	for (const auto& DamageResistance : DamageResistances)
	{
		const float ProcessedDamage = DamageResistance.ProcessDamage(DamageToProcess, DamagedActor);
		DamageToProcess.Damage = ProcessedDamage;
	}
	return DamageToProcess.Damage;
}

bool UJuicyDamageLibrary::CanProcessResistance(const FJuicyDamageResistance& DamageResistance,
                                               const FJuicyTakeDamage& DamageToTake,
                                               const AActor* DamagedActor)
{
	return DamageResistance.CanProcessDamage(DamageToTake, DamagedActor);
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
