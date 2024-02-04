#pragma once

#include "CoreMinimal.h"
#include "JuicySaveGameItem.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FJuicySaveGameItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	TSubclassOf<AActor> Class;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	int32 CurrentStack;

	explicit FJuicySaveGameItem(TSubclassOf<AActor> Class = {},
	                            int32 CurrentStack = 1);

	virtual ~FJuicySaveGameItem();
};
