#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "JuicySaveGameItemEquipment.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicySaveGameItemEquipment
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	int32 ItemIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FName SlotId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FGameplayTagContainer AcceptableSlots;

	explicit FJuicySaveGameItemEquipment(int32 ItemIndex = -1,
	                                     FName SlotId = {},
	                                     const FGameplayTagContainer& AcceptableSlots = {});

	virtual ~FJuicySaveGameItemEquipment();
};
