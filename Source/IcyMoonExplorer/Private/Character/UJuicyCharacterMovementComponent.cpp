#include "Character/UJuicyCharacterMovementComponent.h"

#include "Character/FNetworkPredictionData_Client_JuicyCharacter.h"
#include "Character/JuicyCharacter.h"

void UJuicyCharacterMovementComponent::PostLoad()
{
	Super::PostLoad();

	JuicyCharacterOwner = Cast<AJuicyCharacter>(CharacterOwner);
}

FNetworkPredictionData_Client* UJuicyCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UJuicyCharacterMovementComponent* MutableThis = const_cast<UJuicyCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_JuicyCharacter(*this);
	}
	return ClientPredictionData;
}

void UJuicyCharacterMovementComponent::UpdateFromCompressedFlags(const uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

void UJuicyCharacterMovementComponent::OnMovementUpdated(const float DeltaSeconds, const FVector& OldLocation,
                                                         const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}
