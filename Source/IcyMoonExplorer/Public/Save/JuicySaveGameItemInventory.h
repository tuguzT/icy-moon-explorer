#pragma once

#include "CoreMinimal.h"
#include "JuicySaveGameItemInventory.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicySaveGameItemInventory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	int32 ItemIndex;

	explicit FJuicySaveGameItemInventory(int32 ItemIndex = -1);

	virtual ~FJuicySaveGameItemInventory();
};
