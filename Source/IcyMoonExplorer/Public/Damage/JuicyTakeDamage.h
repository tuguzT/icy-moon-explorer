#pragma once

#include "CoreMinimal.h"
#include "JuicyTakeDamage.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicyTakeDamage
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

	FJuicyTakeDamage();
	FJuicyTakeDamage(float Damage, const UDamageType* DamageType,
	                 AActor* DamageDealer, AController* InstigatedBy);
	virtual ~FJuicyTakeDamage();

	bool IsHealing() const;

	bool HasAnyEffect(float Tolerance = UE_KINDA_SMALL_NUMBER) const;
};
