﻿#pragma once

#include "CoreMinimal.h"
#include "JuicyTakeDamage.generated.h"

USTRUCT(BlueprintType)
struct FJuicyTakeDamage final
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Take Damage")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Take Damage")
	TObjectPtr<const UDamageType> DamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Take Damage")
	TObjectPtr<AActor> DamageDealer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Take Damage")
	TObjectPtr<AController> InstigatedBy;

	bool IsHealing() const;

	bool HasAnyEffect(float Tolerance = UE_KINDA_SMALL_NUMBER) const;
};