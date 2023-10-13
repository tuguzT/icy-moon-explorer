#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UJuicyCharacterMovementComponent.generated.h"

class AJuicyCharacter;

UCLASS()
class ICYMOONEXPLORER_API UJuicyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual void PostLoad() override;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<AJuicyCharacter> JuicyCharacterOwner;
};
