#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "JuicyAILibrary.generated.h"

UCLASS(Abstract)
class ICYMOONEXPLORER_API UJuicyAILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="AI|Team")
	static ETeamAttitude::Type GetTeamAttitudeTowards(const AActor* Target, const AActor* Other);
};
