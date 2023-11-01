#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EJuicyCharacterMovementMode : uint8
{
	None UMETA(Hidden),
	Slide UMETA(DisplayName = "Slide"),
	WallRun UMETA(DisplayName = "Wall Run"),
	Custom UMETA(DisplayName = "Custom"),
	Max UMETA(Hidden),
};
