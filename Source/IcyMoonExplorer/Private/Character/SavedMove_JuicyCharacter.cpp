#include "Character/SavedMove_JuicyCharacter.h"

#include "GameFramework/Character.h"
#include "Character/JuicyCharacterMovementComponent.h"

FSavedMove_JuicyCharacter::FSavedMove_JuicyCharacter()
{
	bWantsToSlide = false;
	bWantsToDash = false;
	bWantsToMantle = false;
}

FSavedMove_JuicyCharacter::~FSavedMove_JuicyCharacter()
{
}

bool FSavedMove_JuicyCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                               const float MaxDelta) const
{
	const auto* NewJuicyMove = static_cast<FSavedMove_JuicyCharacter*>(NewMove.Get());

	const bool CanCombineSlide = NewJuicyMove->bWantsToSlide == bWantsToSlide;
	const bool CanCombineDash = NewJuicyMove->bWantsToDash == bWantsToDash;
	const bool CanCombineMantle = NewJuicyMove->bWantsToMantle == bWantsToMantle;

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta)
		&& CanCombineSlide
		&& CanCombineDash
		&& CanCombineMantle;
}

void FSavedMove_JuicyCharacter::Clear()
{
	Super::Clear();

	bWantsToSlide = false;
	bWantsToDash = false;
	bWantsToMantle = false;
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
	bWantsToDash = CharacterMovement->bWantsToDash;
	bWantsToMantle = CharacterMovement->bWantsToMantle;
}

void FSavedMove_JuicyCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	auto* CharacterMovement = Cast<UJuicyCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bWantsToSlide = bWantsToSlide;
	CharacterMovement->bWantsToDash = bWantsToDash;
	CharacterMovement->bWantsToMantle = bWantsToMantle;
}
