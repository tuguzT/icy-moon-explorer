#include "Character/SavedMove_JuicyCharacter.h"

#include "GameFramework/Character.h"
#include "Character/JuicyCharacterMovementComponent.h"

FSavedMove_JuicyCharacter::FSavedMove_JuicyCharacter()
{
	bWantsToSlide = false;
	bWantsToDash = false;
	bWantsToMantle = false;
	bIsMantling = false;
	bIsOnRightWall = false;
}

FSavedMove_JuicyCharacter::~FSavedMove_JuicyCharacter()
{
}

bool FSavedMove_JuicyCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                               const float MaxDelta) const
{
	const auto* NewJuicyMove = static_cast<FSavedMove_JuicyCharacter*>(NewMove.Get());

	const bool bCombineSlide = NewJuicyMove->bWantsToSlide == bWantsToSlide;
	const bool bCombineDash = NewJuicyMove->bWantsToDash == bWantsToDash;
	const bool bCombineMantle = NewJuicyMove->bWantsToMantle == bWantsToMantle;
	const bool bCombineMantling = NewJuicyMove->bIsMantling == bIsMantling;
	const bool bCombineOnRightWall = NewJuicyMove->bIsOnRightWall == bIsOnRightWall;

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta)
		&& bCombineSlide
		&& bCombineDash
		&& bCombineMantle
		&& bCombineMantling
		&& bCombineOnRightWall;
}

void FSavedMove_JuicyCharacter::Clear()
{
	Super::Clear();

	bWantsToSlide = false;
	bWantsToDash = false;
	bWantsToMantle = false;
	bIsMantling = false;
	bIsOnRightWall = false;
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
	bIsMantling = CharacterMovement->bIsMantling;
	bIsOnRightWall = CharacterMovement->bIsOnRightWall;
}

void FSavedMove_JuicyCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	auto* CharacterMovement = Cast<UJuicyCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bWantsToSlide = bWantsToSlide;
	CharacterMovement->bWantsToDash = bWantsToDash;
	CharacterMovement->bWantsToMantle = bWantsToMantle;
	CharacterMovement->bIsMantling = bIsMantling;
	CharacterMovement->bIsOnRightWall = bIsOnRightWall;
}
