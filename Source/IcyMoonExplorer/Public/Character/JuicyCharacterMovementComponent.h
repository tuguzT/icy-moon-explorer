#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "JuicyCharacterMovementMode.h"
#include "JuicyCharacterMovementComponent.generated.h"

class AJuicyCharacter;

UCLASS(Blueprintable)
class ICYMOONEXPLORER_API UJuicyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float SlideMinHorizontalSpeed;

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

	UPROPERTY(Category="Character Movement: Wall Run", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="s"))
	float WallRunCooldown;

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

	UPROPERTY(Category="Character Movement: Wall Hang", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="s"))
	float WallHangCooldown;

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

	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite)
	uint8 bCanEverSlide : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSlide : 1;

	UPROPERTY(Category="Character Movement: Dashing", EditAnywhere, BlueprintReadWrite)
	uint8 bCanEverDash : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToDash : 1;

	UPROPERTY(Category="Character Movement: Mantling", EditAnywhere, BlueprintReadWrite)
	uint8 bCanEverMantle : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToMantle : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bIsMantling : 1;

	UPROPERTY(Category="Character Movement: Wall Run", EditAnywhere, BlueprintReadWrite)
	uint8 bCanEverWallRun : 1;

	UPROPERTY(Category="Character Movement: Wall Hang", EditAnywhere, BlueprintReadWrite)
	uint8 bCanEverWallHang : 1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	FVector WallNormal;

	explicit UJuicyCharacterMovementComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement")
	AJuicyCharacter* GetJuicyCharacterOwner() const;

	/* MOVEMENT MODE */

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	virtual void SetMovementMode(EJuicyCharacterMovementMode NewMovementMode);

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement")
	virtual bool IsMovementMode(EMovementMode IsMovementMode, uint8 IsCustomMode = 0) const;

	virtual bool IsMovementMode(EJuicyCharacterMovementMode IsMovementMode) const;

	/* SLIDING */

	virtual void Slide();
	virtual void UnSlide();

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Sliding")
	virtual bool IsSliding() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Sliding")
	virtual bool CanSlideInCurrentState() const;

	/* DASHING */

	virtual void Dash();
	virtual void UnDash();

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Dashing")
	virtual bool IsDashing() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Dashing")
	virtual bool IsDashingCooldown() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Dashing")
	virtual bool CanDashInCurrentState() const;

	/* MANTLING */

	virtual void Mantle();
	virtual void UnMantle();

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Mantling")
	virtual bool IsMantling() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Mantling")
	virtual bool CanMantleInCurrentState() const;

	virtual void ExitMantling();

	/* WALL RUN */

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Run")
	virtual bool IsWallRunning() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Run")
	virtual bool IsWallRunningCooldown() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Run")
	virtual bool CanWallRunInCurrentState() const;

	/* WALL HANG */

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Hang")
	virtual bool IsWallHanging() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Hang")
	virtual bool IsWallHangingCooldown() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Hang")
	virtual bool CanWallHangInCurrentState() const;

	/* WALL RUN / WALL HANG */

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Run / Wall Hang")
	virtual bool IsOnWall() const;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement|Wall Run / Wall Hang")
	virtual FVector GetWallNormal() const;

	/* OVERRIDDEN METHODS */

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
	virtual void SetPostLandedPhysics(const FHitResult& Hit) override;

private:
	FTimerHandle TimerHandleForDashDuration;
	FTimerHandle TimerHandleForDashCooldown;
	FVector CurrentDashDirection;

	FTimerHandle TimerHandleForWallRunCooldown;
	FTimerHandle TimerHandleForWallHangCooldown;

	bool HasInput() const;

	void StartSlide();
	void EndSlide();

	void StartDash();
	void EndDash();
	void StartDashCooldown();
	void EndDashCooldown();

	void StartMantle();

	void StartWallRun();
	void StartWallRunCooldown();
	void EndWallRunCooldown();

	void StartWallHang();
	void StartWallHangCooldown();
	void EndWallHangCooldown();

	bool CheckFloorExists(FHitResult& FloorHit, const FHitResult& WallHit) const;
	bool CheckWallExists(FHitResult& WallHit, const FVector& Direction) const;
};
