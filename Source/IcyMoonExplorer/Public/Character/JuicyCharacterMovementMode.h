#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EJuicyCharacterMovementMode : uint8
{
	Slide UMETA(DisplayName = "Slide"),
	WallRun UMETA(DisplayName = "Wall Run"),
	WallHang UMETA(DisplayName = "Wall Hang"),
};
