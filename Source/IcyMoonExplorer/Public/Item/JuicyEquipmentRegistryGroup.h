#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "JuicyEquipmentRegistryGroup.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicyEquipmentRegistryGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="AGR")
	FGameplayTag TagSlotType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="AGR")
	FGameplayTagContainer Labels;

	FJuicyEquipmentRegistryGroup();
	virtual ~FJuicyEquipmentRegistryGroup();

	FJuicyEquipmentRegistryGroup(const FGameplayTag& TagSlotType,
	                             const FGameplayTagContainer& Labels);

	friend bool operator==(const FJuicyEquipmentRegistryGroup& Lhs,
	                       const FJuicyEquipmentRegistryGroup& Rhs) = default;

	friend uint32 GetTypeHash(const FJuicyEquipmentRegistryGroup& Group);
};
