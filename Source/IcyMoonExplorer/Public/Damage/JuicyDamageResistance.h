#pragma once

#include "JuicyTakeDamage.h"
#include "JuicyDamageResistance.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicyDamageResistance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Resistance")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Resistance")
	float Multiplier;

	FJuicyDamageResistance();
	FJuicyDamageResistance(TSubclassOf<UDamageType> DamageTypeClass, float Multiplier);
	virtual ~FJuicyDamageResistance();

	float ProcessDamage(const FJuicyTakeDamage& DamageToTake) const;

	bool CanProcessDamage(const FJuicyTakeDamage& DamageToTake) const;

	bool HasAnyEffect() const;

	bool IsResistantToDamage() const;

	bool IsWeakToDamage() const;

	bool IsImmuneToDamage() const;

	bool CanHealFromDamage() const;
};
