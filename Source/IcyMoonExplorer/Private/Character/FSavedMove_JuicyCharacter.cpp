#include "Character/FSavedMove_JuicyCharacter.h"

bool FSavedMove_JuicyCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                               const float MaxDelta) const
{
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_JuicyCharacter::Clear()
{
	Super::Clear();
}

uint8 FSavedMove_JuicyCharacter::GetCompressedFlags() const
{
	return Super::GetCompressedFlags();
}

void FSavedMove_JuicyCharacter::SetMoveFor(ACharacter* C, const float InDeltaTime, const FVector& NewAccel,
                                           FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
}

void FSavedMove_JuicyCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
}
