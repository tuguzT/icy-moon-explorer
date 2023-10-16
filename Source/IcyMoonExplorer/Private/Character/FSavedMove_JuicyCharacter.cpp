#include "Character/FSavedMove_JuicyCharacter.h"

#include "GameFramework/Character.h"
#include "Character/UJuicyCharacterMovementComponent.h"

FSavedMove_JuicyCharacter::FSavedMove_JuicyCharacter()
{
	bWantsToSlide = false;
}

FSavedMove_JuicyCharacter::~FSavedMove_JuicyCharacter()
{
}

bool FSavedMove_JuicyCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                               const float MaxDelta) const
{
	const auto* NewJuicyMove = static_cast<FSavedMove_JuicyCharacter*>(NewMove.Get());

	const bool CanCombineSlide = NewJuicyMove->bWantsToSlide == bWantsToSlide;

	return CanCombineSlide
		&& Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_JuicyCharacter::Clear()
{
	Super::Clear();
	bWantsToSlide = false;
}

uint8 FSavedMove_JuicyCharacter::GetCompressedFlags() const
{
	return Super::GetCompressedFlags();
}

void FSavedMove_JuicyCharacter::SetMoveFor(ACharacter* C, const float InDeltaTime, const FVector& NewAccel,
                                           FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const auto* CharacterMovement = Cast<UJuicyCharacterMovementComponent>(C->GetCharacterMovement());
	bWantsToSlide = CharacterMovement->bWantsToSlide;
}

void FSavedMove_JuicyCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	auto* CharacterMovement = Cast<UJuicyCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bWantsToSlide = bWantsToSlide;
}
