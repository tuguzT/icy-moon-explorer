#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

class ICYMOONEXPLORER_API FNetworkPredictionData_Client_JuicyCharacter final
	: public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:
	explicit FNetworkPredictionData_Client_JuicyCharacter(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};
