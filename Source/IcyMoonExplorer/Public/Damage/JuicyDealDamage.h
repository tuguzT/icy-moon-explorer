#pragma once

#include "CoreMinimal.h"
#include "JuicyDealDamage.generated.h"

USTRUCT(BlueprintType)
struct FJuicyDealDamage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deal Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deal Damage")
	TSubclassOf<UDamageType> DamageTypeClass;
};
