#pragma once

#include "CoreMinimal.h"
#include "NestedActors.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct ICYMOONEXPLORER_API FNestedActors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Actors;

	FNestedActors();
	virtual ~FNestedActors();
};
