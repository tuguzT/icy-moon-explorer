#pragma once

#include "CoreMinimal.h"
#include "JuicySaveGameItemInventory.h"
#include "JuicySaveGameNestedItems.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicySaveGameNestedItems
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	TArray<FJuicySaveGameItemInventory> ItemIndices;

	explicit FJuicySaveGameNestedItems(const TArray<FJuicySaveGameItemInventory>& ItemIndices = {});

	~FJuicySaveGameNestedItems();
};
