#pragma once

#include "JuicyTakeDamage.h"
#include "JuicyDamageResistance.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ICYMOONEXPLORER_API UJuicyDamageResistance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Resistance")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Resistance")
	float Multiplier;

	explicit UJuicyDamageResistance(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Damage Resistance")
	virtual float ProcessDamage(const FJuicyTakeDamage& DamageToTake) const;

	UFUNCTION(BlueprintCallable, Category="Damage Resistance")
	virtual bool CanProcessDamage(const FJuicyTakeDamage& DamageToTake) const;

	UFUNCTION(BlueprintCallable, Category="Damage Resistance")
	virtual bool HasAnyEffect() const;

	UFUNCTION(BlueprintCallable, Category="Damage Resistance")
	virtual bool IsResistantToDamage() const;

	UFUNCTION(BlueprintCallable, Category="Damage Resistance")
	virtual bool IsWeakToDamage() const;

	UFUNCTION(BlueprintCallable, Category="Damage Resistance")
	virtual bool IsImmuneToDamage() const;

	UFUNCTION(BlueprintCallable, Category="Damage Resistance")
	virtual bool CanHealFromDamage() const;
};
