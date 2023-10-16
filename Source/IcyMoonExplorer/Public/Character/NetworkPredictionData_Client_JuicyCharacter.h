#pragma once

#include "GameFramework/CharacterMovementComponent.h"

// ReSharper disable once CppClassCanBeFinal
class ICYMOONEXPLORER_API FNetworkPredictionData_Client_JuicyCharacter
	: public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:
	explicit FNetworkPredictionData_Client_JuicyCharacter(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};
