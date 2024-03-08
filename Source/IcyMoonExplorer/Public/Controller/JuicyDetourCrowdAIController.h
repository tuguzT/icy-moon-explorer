#pragma once

#include "CoreMinimal.h"
#include "JuicyAIController.h"
#include "JuicyDetourCrowdAIController.generated.h"

UCLASS()
class ICYMOONEXPLORER_API AJuicyDetourCrowdAIController : public AJuicyAIController
{
	GENERATED_BODY()

public:
	explicit AJuicyDetourCrowdAIController(
    	const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
