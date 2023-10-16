#include "Character/FNetworkPredictionData_Client_JuicyCharacter.h"

#include "Character/FSavedMove_JuicyCharacter.h"

FNetworkPredictionData_Client_JuicyCharacter::FNetworkPredictionData_Client_JuicyCharacter(
	const UCharacterMovementComponent& ClientMovement): Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_JuicyCharacter::AllocateNewMove()
{
	return MakeShared<FSavedMove_JuicyCharacter>();
}
