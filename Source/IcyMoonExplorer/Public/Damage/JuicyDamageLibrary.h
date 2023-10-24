#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "JuicyDealDamage.h"
#include "JuicyTakeDamage.h"
#include "JuicyDamageLibrary.generated.h"

UCLASS(Abstract)
class ICYMOONEXPLORER_API UJuicyDamageLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category="Deal Damage")
	static bool IsDealingHeal(const FJuicyDealDamage& DamageToDeal);

	UFUNCTION(BlueprintPure, Category="Deal Damage")
	static bool HasAnyEffectToDeal(const FJuicyDealDamage& DamageToDeal, float Tolerance = 1.e-4f);

	UFUNCTION(BlueprintPure, Category="Take Damage")
	static bool IsTakingHeal(const FJuicyTakeDamage& DamageToTake);

	UFUNCTION(BlueprintPure, Category="Take Damage")
	static bool HasAnyEffectToTake(const FJuicyTakeDamage& DamageToTake, float Tolerance = 1.e-4f);
};
