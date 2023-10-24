#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "JuicyDealDamage.h"
#include "JuicyDamageResistance.h"
#include "JuicyDamageLibrary.generated.h"

UCLASS(Abstract)
class ICYMOONEXPLORER_API UJuicyDamageLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Deal Damage")
	static bool IsDealingHeal(const FJuicyDealDamage& DamageToDeal);

	UFUNCTION(BlueprintPure, Category="Deal Damage")
	static bool HasAnyDamageToDeal(const FJuicyDealDamage& DamageToDeal, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintPure, Category="Take Damage")
	static bool IsTakingHeal(const FJuicyTakeDamage& DamageToTake);

	UFUNCTION(BlueprintPure, Category="Take Damage")
	static bool HasAnyDamageToTake(const FJuicyTakeDamage& DamageToTake, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static float ProcessResistance(const FJuicyDamageResistance& DamageResistance,
	                               const FJuicyTakeDamage& DamageToTake);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static float ProcessResistances(const TArray<FJuicyDamageResistance>& DamageResistances,
	                                const FJuicyTakeDamage& DamageToTake);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static bool CanProcessResistance(const FJuicyDamageResistance& DamageResistance,
	                                 const FJuicyTakeDamage& DamageToTake);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static bool HasAnyResistance(const FJuicyDamageResistance& DamageResistance);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static bool IsResistantToDamage(const FJuicyDamageResistance& DamageResistance);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static bool IsWeakToDamage(const FJuicyDamageResistance& DamageResistance);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static bool IsImmuneToDamage(const FJuicyDamageResistance& DamageResistance);

	UFUNCTION(BlueprintPure, Category="Damage Resistance")
	static bool CanHealFromDamage(const FJuicyDamageResistance& DamageResistance);
};
