#pragma once

#include "CoreMinimal.h"
#include "JuicyDealDamage.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicyDealDamage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deal Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deal Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	FJuicyDealDamage();
	FJuicyDealDamage(float Damage, TSubclassOf<UDamageType> DamageTypeClass);
	virtual ~FJuicyDealDamage();

	bool IsHealing() const;

	bool HasAnyEffect(float Tolerance = UE_KINDA_SMALL_NUMBER) const;
};
