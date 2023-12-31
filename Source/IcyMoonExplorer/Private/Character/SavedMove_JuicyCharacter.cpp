﻿#include "Character/SavedMove_JuicyCharacter.h"

#include "GameFramework/Character.h"
#include "Character/JuicyCharacterMovementComponent.h"

FSavedMove_JuicyCharacter::FSavedMove_JuicyCharacter()
{
	bWantsToSlide = false;
	bWantsToDash = false;
	bWantsToMantle = false;
	bIsMantling = false;

	WallDotThresholdCombine = 0.996f;
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
	const bool bCombineWallNormal = FVector::Coincident(NewJuicyMove->WallNormal, WallNormal);

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta)
		&& bCombineSlide
		&& bCombineDash
		&& bCombineMantle
		&& bCombineMantling
		&& bCombineWallNormal;
}

void FSavedMove_JuicyCharacter::Clear()
{
	Super::Clear();

	bWantsToSlide = false;
	bWantsToDash = false;
	bWantsToMantle = false;
	bIsMantling = false;

	WallNormal = FVector::ZeroVector;
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
	WallNormal = CharacterMovement->WallNormal;
}

void FSavedMove_JuicyCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	auto* CharacterMovement = Cast<UJuicyCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bWantsToSlide = bWantsToSlide;
	CharacterMovement->bWantsToDash = bWantsToDash;
	CharacterMovement->bWantsToMantle = bWantsToMantle;
	CharacterMovement->bIsMantling = bIsMantling;
	CharacterMovement->WallNormal = WallNormal;
}
