#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "JuicyCharacterMovementMode.h"
#include "JuicyCharacterMovementComponent.generated.h"

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
		meta=(ClampMin="0", UIMin="0"))
	float BrakingDecelerationSliding;

	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite)
	uint8 bCanSlideOffLedges : 1;

	UPROPERTY(Category="Character Movement: Dashing", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float DashImpulse;

	UPROPERTY(Category="Character Movement: Dashing", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="s"))
	float DashDuration;

	UPROPERTY(Category="Character Movement: Dashing", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="s"))
	float DashCooldown;

	UPROPERTY(Category="Character Movement: Dashing", EditAnywhere, BlueprintReadWrite)
	float DashGravityScale;

	UPROPERTY(Category="Character Movement: Mantling", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm"))
	float MantleMaxDistance;

	UPROPERTY(Category="Character Movement: Mantling", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm"))
	float MantleReachHeight;

	UPROPERTY(Category="Character Movement: Mantling", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0.0", ClampMax="90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits="degrees"))
	float MantleMinSteepnessAngle;

	UPROPERTY(Category="Character Movement: Mantling", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0.0", ClampMax="90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits="degrees"))
	float MantleMaxSurfaceAngle;

	UPROPERTY(Category="Character Movement: Mantling", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0.0", ClampMax="90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits="degrees"))
	float MantleMaxAlignmentAngle;

	UPROPERTY(Category="Character Movement: Mantling", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0"))
	uint8 MantleWallCheckFrequency;

	UPROPERTY(Category="Character Movement: Wall Run", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float WallRunMinHorizontalSpeed;

	UPROPERTY(Category="Character Movement: Wall Run", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float WallRunMaxVerticalSpeed;

	UPROPERTY(Category="Character Movement: Wall Run", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxWallRunSpeed;

	UPROPERTY(Category="Character Movement: Wall Run", EditAnywhere, BlueprintReadWrite)
	float WallRunGravityScale;

	UPROPERTY(Category="Character Movement: Wall Run", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float WallRunMaxGravityVelocity;

	UPROPERTY(Category="Character Movement: Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxWallHangSpeed;

	UPROPERTY(Category="Character Movement: Wall Hang", EditAnywhere, BlueprintReadWrite)
	float WallHangGravityScale;

	UPROPERTY(Category="Character Movement: Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float WallHangMaxGravityVelocity;

	UPROPERTY(Category="Character Movement: Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0"))
	float BrakingDecelerationWallHanging;

	UPROPERTY(Category="Character Movement: Wall Run / Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm"))
	float MaxWallDistance;

	UPROPERTY(Category="Character Movement: Wall Run / Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm"))
	float MinHeightAboveFloor;

	UPROPERTY(Category="Character Movement: Wall Run / Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0.0", ClampMax="90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits="degrees"))
	float WallMinSteepnessAngle;

	UPROPERTY(Category="Character Movement: Wall Run / Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0.0", ClampMax="90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits="degrees"))
	float WallMinPullAwayAngle;

	UPROPERTY(Category="Character Movement: Wall Run / Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float WallAttractionForce;

	UPROPERTY(Category="Character Movement: Wall Run / Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float JumpOffWallVerticalVelocity;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSlide : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToDash : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToMantle : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bIsMantling : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	FVector WallNormal;

	explicit UJuicyCharacterMovementComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement")
	AJuicyCharacter* GetJuicyCharacterOwner() const;

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	virtual void SetMovementMode(EJuicyCharacterMovementMode NewMovementMode, uint8 NewCustomMode = 0);
	virtual bool IsMovementMode(EMovementMode IsMovementMode, uint8 IsCustomMode = 0) const;
	virtual bool IsMovementMode(EJuicyCharacterMovementMode IsMovementMode, uint8 IsCustomMode = 0) const;

	virtual void Slide();
	virtual void UnSlide();
	virtual bool IsSliding() const;
	virtual bool CanSlideInCurrentState() const;

	virtual void Dash();
	virtual void UnDash();
	virtual bool IsDashing() const;
	virtual bool IsDashingCooldown() const;
	virtual bool CanDashInCurrentState() const;

	virtual void Mantle();
	virtual void UnMantle();
	virtual bool IsMantling() const;
	virtual bool CanMantleInCurrentState() const;
	virtual void ExitMantling();

	virtual bool IsWallRunning() const;
	virtual bool CanWallRunInCurrentState() const;

	virtual bool IsWallHanging() const;
	virtual bool CanWallHangInCurrentState() const;

	virtual bool IsOnWall() const;
	virtual FVector GetWallNormal() const;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetGravityZ() const override;
	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual bool IsMovingOnGround() const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual bool CanWalkOffLedges() const override;
	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity,
	                                float DeltaTime) const override;

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void PhysSlide(float DeltaTime, int32 Iterations);
	virtual void PhysWallRun(float DeltaTime, int32 Iterations);
	virtual void PhysWallHang(float DeltaTime, int32 Iterations);

	virtual bool TryMantle(FHitResult& FrontHit, FHitResult& SurfaceHit) const;
	virtual bool TryWallRun(FHitResult& FloorHit, FHitResult& WallHit) const;
	virtual bool TryWallHang(FHitResult& FloorHit, FHitResult& WallHit) const;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	                               const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode,
	                                   uint8 PreviousCustomMode) override;

private:
	FTimerHandle TimerHandleForDashDuration;
	FTimerHandle TimerHandleForDashCooldown;
	FVector CurrentDashDirection;

	bool HasInput() const;
	void ResetCharacterRotation(const FVector& Forward, bool bSweep);

	void StartSlide();
	void EndSlide();

	void StartDash();
	void EndDash();
	void StartDashCooldown();
	void EndDashCooldown();

	void StartMantle();

	void StartWallRun();
	void StartWallHang();

	bool CheckFloorExists(FHitResult& FloorHit, const FHitResult& WallHit) const;
	bool CheckWallExists(FHitResult& WallHit, const FVector& Direction) const;
};
