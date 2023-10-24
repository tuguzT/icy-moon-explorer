#pragma once

#include "CoreMinimal.h"
#include "JuicyDealDamage.generated.h"

USTRUCT(BlueprintType)
struct FJuicyDealDamage final
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deal Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deal Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	bool IsHealing() const;

	bool HasAnyEffect(float Tolerance = UE_KINDA_SMALL_NUMBER) const;
};
