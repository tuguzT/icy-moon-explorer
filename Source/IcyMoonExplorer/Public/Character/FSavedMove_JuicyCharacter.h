#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

class ICYMOONEXPLORER_API FSavedMove_JuicyCharacter final : public FSavedMove_Character
{
	using Super = FSavedMove_Character;

public:
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, const FVector& NewAccel,
	                        FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;
};
