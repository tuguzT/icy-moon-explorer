#pragma once

#include "GameFramework/CharacterMovementComponent.h"

// ReSharper disable once CppClassCanBeFinal
class ICYMOONEXPLORER_API FSavedMove_JuicyCharacter : public FSavedMove_Character
{
	using Super = FSavedMove_Character;

public:
	uint8 bWantsToSlide : 1;

	FSavedMove_JuicyCharacter();
	virtual ~FSavedMove_JuicyCharacter() override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, const FVector& NewAccel,
	                        FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;
};
