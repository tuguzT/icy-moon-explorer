#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/EJuicyCharacterMovementMode.h"
#include "UJuicyCharacterMovementComponent.generated.h"

class AJuicyCharacter;

UCLASS()
class ICYMOONEXPLORER_API UJuicyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxSlideSpeed;

	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0"))
	float SlideFriction;

	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm"))
	float SlideHalfHeight;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSlide : 1;

	explicit UJuicyCharacterMovementComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement")
	AJuicyCharacter* GetJuicyCharacterOwner() const;

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	virtual void SetMovementMode(EJuicyCharacterMovementMode NewMovementMode, uint8 NewCustomMode = 0);
	virtual bool IsMovementMode(EJuicyCharacterMovementMode IsMovementMode, uint8 NewCustomMode = 0) const;

	virtual void Slide();
	virtual void UnSlide();
	virtual bool IsSliding() const;
	virtual bool CanSlideInCurrentState() const;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual bool IsMovingOnGround() const override;

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void PhysSlide(float DeltaTime, int32 Iterations);
	virtual void PhysJuicyCustom(float DeltaTime, int32 Iterations);

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	                               const FVector& OldVelocity) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual bool CanAttemptJump() const override;

private:
	bool GetSlideSurface(FHitResult& OutHit) const;

	void ChangeHalfHeightBeforeSliding();
	void RestoreHalfHeightAfterSliding();
};
