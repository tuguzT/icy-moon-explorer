#include "Character/JuicyCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Character/NetworkPredictionData_Client_JuicyCharacter.h"
#include "Character/JuicyCharacter.h"

namespace Detail
{
	constexpr auto SlideMode = static_cast<uint8>(EJuicyCharacterMovementMode::Slide);
	constexpr auto WallRunMode = static_cast<uint8>(EJuicyCharacterMovementMode::WallRun);
	constexpr auto WallHangMode = static_cast<uint8>(EJuicyCharacterMovementMode::WallHang);

	FCollisionQueryParams CollisionQueryParamsWithoutActor(const AActor* Actor)
	{
		TArray<AActor*> ChildActors;
		Actor->GetAllChildActors(ChildActors);

		FCollisionQueryParams Params;
		Params.AddIgnoredActors(ChildActors);
		Params.AddIgnoredActor(Actor);
		return Params;
	}
}

UJuicyCharacterMovementComponent::UJuicyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	constexpr float DefaultCapsuleHalfHeight = 88.0f;
	constexpr float DefaultCapsuleRadius = 34.0f;

	NavAgentProps.bCanCrouch = true;
	bCanWalkOffLedgesWhenCrouching = true;

	SlideMinHorizontalSpeed = MaxWalkSpeed * 0.5f;
	MaxSlideSpeed = MaxWalkSpeed * 2.0f;
	SlideFriction = GroundFriction;
	BrakingDecelerationSliding = BrakingDecelerationWalking;
	bCanSlideOffLedges = bCanWalkOffLedges;
	bWantsToSlide = false;

	DashImpulse = MaxWalkSpeed * 3.0f;
	DashDuration = 0.5f;
	DashCooldown = 1.0f;
	DashGravityScale = 0.0f;
	bWantsToDash = false;

	MantleMaxDistance = DefaultCapsuleRadius;
	MantleReachHeight = DefaultCapsuleHalfHeight;
	MantleMinSteepnessAngle = GetWalkableFloorAngle();
	MantleMaxSurfaceAngle = 40.0f;
	MantleMaxAlignmentAngle = 45.0f;
	MantleWallCheckFrequency = 5;
	bWantsToMantle = false;
	bIsMantling = false;

	WallRunMinHorizontalSpeed = MaxWalkSpeed * 0.5f;
	WallRunMaxVerticalSpeed = JumpZVelocity * 0.5f;
	MaxWallRunSpeed = MaxWalkSpeed;
	WallRunGravityScale = 0.0f;
	WallRunMaxGravityVelocity = 100.0f;
	WallRunCooldown = 0.5f;

	MaxWallHangSpeed = MaxWalkSpeed;
	WallHangGravityScale = 0.0f;
	WallHangMaxGravityVelocity = 100.0f;
	BrakingDecelerationWallHanging = BrakingDecelerationWalking;
	WallHangCooldown = 0.5f;

	MaxWallDistance = DefaultCapsuleRadius;
	MinHeightAboveFloor = DefaultCapsuleHalfHeight;
	WallMinSteepnessAngle = GetWalkableFloorAngle();
	JumpOffWallVerticalVelocity = JumpZVelocity * 0.5f;
	WallMinPullAwayAngle = 75.0f;
	WallAttractionForce = JumpZVelocity * 0.5f;
}

AJuicyCharacter* UJuicyCharacterMovementComponent::GetJuicyCharacterOwner() const
{
	return Cast<AJuicyCharacter>(GetCharacterOwner());
}

void UJuicyCharacterMovementComponent::SetMovementMode(const EMovementMode NewMovementMode,
                                                       const uint8 NewCustomMode)
{
	Super::SetMovementMode(NewMovementMode, NewCustomMode);
}

void UJuicyCharacterMovementComponent::SetMovementMode(const EJuicyCharacterMovementMode NewMovementMode)
{
	const auto CustomMode = static_cast<uint8>(NewMovementMode);
	Super::SetMovementMode(MOVE_Custom, CustomMode);
}

bool UJuicyCharacterMovementComponent::IsMovementMode(const EMovementMode IsMovementMode,
                                                      const uint8 IsCustomMode) const
{
	return MovementMode == IsMovementMode
		&& (MovementMode != MOVE_Custom || CustomMovementMode == IsCustomMode);
}

bool UJuicyCharacterMovementComponent::IsMovementMode(const EJuicyCharacterMovementMode IsMovementMode) const
{
	const auto CustomMode = static_cast<uint8>(IsMovementMode);
	return MovementMode == MOVE_Custom && CustomMovementMode == CustomMode;
}

void UJuicyCharacterMovementComponent::Slide()
{
	if (!CanSlideInCurrentState())
	{
		return;
	}

	bWantsToSlide = true;
}

void UJuicyCharacterMovementComponent::UnSlide()
{
	bWantsToSlide = false;
}

bool UJuicyCharacterMovementComponent::IsSliding() const
{
	return IsMovementMode(EJuicyCharacterMovementMode::Slide);
}

bool UJuicyCharacterMovementComponent::CanSlideInCurrentState() const
{
	const bool bEnoughVelocity = FVector2D(Velocity).SquaredLength() >= FMath::Pow(SlideMinHorizontalSpeed, 2);
	return HasInput()
		&& bEnoughVelocity
		&& IsMovingOnGround()
		&& !IsMantling()
		&& !IsWallRunning()
		&& !IsWallHanging();
}

void UJuicyCharacterMovementComponent::Dash()
{
	if (!CanDashInCurrentState())
	{
		return;
	}

	bWantsToDash = true;
}

void UJuicyCharacterMovementComponent::UnDash()
{
	bWantsToDash = false;
}

bool UJuicyCharacterMovementComponent::IsDashing() const
{
	if (!CharacterOwner)
	{
		return false;
	}

	const FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForDashDuration);
}

bool UJuicyCharacterMovementComponent::IsDashingCooldown() const
{
	if (!CharacterOwner)
	{
		return false;
	}

	const FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForDashCooldown);
}

bool UJuicyCharacterMovementComponent::CanDashInCurrentState() const
{
	const bool bIsAllowedMode = MovementMode == MOVE_Walking
		|| MovementMode == MOVE_NavWalking
		|| MovementMode == MOVE_Falling
		|| MovementMode == MOVE_Flying
		|| IsWallRunning()
		|| IsWallHanging();
	const bool bIsNotDashing = !IsDashing();
	const bool bIsNotDashingCooldown = !IsDashingCooldown();
	const bool bIsNotMantling = !IsMantling();

	return UpdatedComponent
		&& bIsAllowedMode
		&& bIsNotDashing
		&& bIsNotDashingCooldown
		&& bIsNotMantling;
}

void UJuicyCharacterMovementComponent::Mantle()
{
	if (!CanMantleInCurrentState())
	{
		return;
	}

	bWantsToMantle = true;
}

void UJuicyCharacterMovementComponent::UnMantle()
{
	bWantsToMantle = false;
}

bool UJuicyCharacterMovementComponent::IsMantling() const
{
	return bIsMantling;
}

bool UJuicyCharacterMovementComponent::CanMantleInCurrentState() const
{
	const bool bIsAllowedMode = MovementMode == MOVE_Walking
		|| MovementMode == MOVE_NavWalking
		|| MovementMode == MOVE_Falling
		|| MovementMode == MOVE_Flying
		|| MovementMode == MOVE_Swimming
		|| IsWallHanging();
	const bool bIsNotDashing = !IsDashing();
	const bool bIsNotMantling = !IsMantling();

	return UpdatedComponent
		&& bIsAllowedMode
		&& bIsNotDashing
		&& bIsNotMantling;
}

void UJuicyCharacterMovementComponent::ExitMantling()
{
	if (!IsMantling())
	{
		return;
	}

	bIsMantling = false;
	Super::SetMovementMode(MOVE_Walking);
	GetJuicyCharacterOwner()->OnEndMantle();
}

bool UJuicyCharacterMovementComponent::IsWallRunning() const
{
	return IsMovementMode(EJuicyCharacterMovementMode::WallRun);
}

bool UJuicyCharacterMovementComponent::IsWallRunningCooldown() const
{
	if (!CharacterOwner)
	{
		return false;
	}

	const FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForWallRunCooldown);
}

bool UJuicyCharacterMovementComponent::CanWallRunInCurrentState() const
{
	return HasInput()
		&& IsFalling()
		&& !IsDashing()
		&& !IsMantling()
		&& !IsWallRunningCooldown();
}

bool UJuicyCharacterMovementComponent::IsWallHanging() const
{
	return IsMovementMode(EJuicyCharacterMovementMode::WallHang);
}

bool UJuicyCharacterMovementComponent::IsWallHangingCooldown() const
{
	if (!CharacterOwner)
	{
		return false;
	}

	const FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForWallHangCooldown);
}

bool UJuicyCharacterMovementComponent::CanWallHangInCurrentState() const
{
	return HasInput()
		&& IsFalling()
		&& !IsDashing()
		&& !IsMantling()
		&& !IsWallHangingCooldown();
}

bool UJuicyCharacterMovementComponent::IsOnWall() const
{
	return IsWallRunning() || IsWallHanging();
}

FVector UJuicyCharacterMovementComponent::GetWallNormal() const
{
	if (!IsOnWall())
	{
		return FVector::ZeroVector;
	}
	return WallNormal;
}

FNetworkPredictionData_Client* UJuicyCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		auto* MutableThis = const_cast<UJuicyCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_JuicyCharacter(*this);
	}
	return ClientPredictionData;
}

float UJuicyCharacterMovementComponent::GetGravityZ() const
{
	const float BaseGravityZ = Super::GetGravityZ();
	if (IsDashing())
	{
		return BaseGravityZ * DashGravityScale;
	}
	if (IsWallRunning())
	{
		return BaseGravityZ * WallRunGravityScale;
	}
	if (IsWallHanging())
	{
		return BaseGravityZ * WallHangGravityScale;
	}
	return BaseGravityZ;
}

bool UJuicyCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed()
		&& !IsDashing()
		&& !IsMantling()
		&& (IsMovingOnGround() || IsFalling() || IsWallRunning() || IsWallHanging());
}

bool UJuicyCharacterMovementComponent::DoJump(const bool bReplayingMoves)
{
	const bool bWasOnWall = IsOnWall();
	if (!Super::DoJump(bReplayingMoves))
	{
		return false;
	}

	if (bWasOnWall)
	{
		Velocity += WallNormal * JumpOffWallVerticalVelocity;
	}
	return true;
}

void UJuicyCharacterMovementComponent::UpdateCharacterStateBeforeMovement(const float DeltaSeconds)
{
	// Sliding
	if (!IsSliding() && bWantsToSlide && CanSlideInCurrentState())
	{
		StartSlide();
	}
	else if (IsSliding() && (!bWantsToSlide || !CanSlideInCurrentState()))
	{
		EndSlide();
	}

	// Dashing
	if (bWantsToDash && CanDashInCurrentState())
	{
		StartDash();
	}

	// Mantling
	if (bWantsToMantle && CanMantleInCurrentState())
	{
		StartMantle();
	}

	// Wall running
	if (CanWallRunInCurrentState())
	{
		StartWallRun();
	}

	// Wall hanging
	if (CanWallHangInCurrentState())
	{
		StartWallHang();
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

bool UJuicyCharacterMovementComponent::IsMovingOnGround() const
{
	const bool bIsAllowedMode = MovementMode == MOVE_Walking
		|| MovementMode == MOVE_NavWalking
		|| IsSliding();
	return bIsAllowedMode && UpdatedComponent;
}

float UJuicyCharacterMovementComponent::GetMaxSpeed() const
{
	if (MOVE_Custom != MovementMode)
	{
		return Super::GetMaxSpeed();
	}

	switch (CustomMovementMode)
	{
	case Detail::SlideMode:
		return MaxSlideSpeed;
	case Detail::WallRunMode:
		return MaxWallRunSpeed;
	case Detail::WallHangMode:
		return MaxWallHangSpeed;
	default:
		return MaxCustomMovementSpeed;
	}
}

float UJuicyCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MOVE_Custom != MovementMode)
	{
		return Super::GetMaxBrakingDeceleration();
	}

	switch (CustomMovementMode)
	{
	case Detail::SlideMode:
		return BrakingDecelerationSliding;
	case Detail::WallHangMode:
		return BrakingDecelerationWallHanging;
	default:
		return 0.0f;
	}
}

bool UJuicyCharacterMovementComponent::CanWalkOffLedges() const
{
	if (IsSliding())
	{
		return bCanSlideOffLedges;
	}
	return Super::CanWalkOffLedges();
}

FVector UJuicyCharacterMovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity,
                                                          const float DeltaTime) const
{
	FVector Result = Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);

	if (DeltaTime > 0.0f)
	{
		// Calculate limit, if exists.
		float Limit;
		if (IsWallRunning())
		{
			Limit = FMath::Abs(WallRunMaxGravityVelocity);
		}
		else if (IsWallHanging())
		{
			Limit = FMath::Abs(WallHangMaxGravityVelocity);
		}
		else
		{
			return Result;
		}

		// Don't exceed limit.
		if (Result.SizeSquared() > FMath::Square(Limit))
		{
			// ReSharper disable once CppTooWideScopeInitStatement
			const FVector GravityDir = Gravity.GetSafeNormal();
			if ((Result | GravityDir) > Limit)
			{
				Result = FVector::PointPlaneProject(Result, FVector::ZeroVector, GravityDir) + GravityDir * Limit;
			}
		}
	}

	return Result;
}

void UJuicyCharacterMovementComponent::PhysCustom(const float DeltaTime, const int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case Detail::SlideMode:
		PhysSlide(DeltaTime, Iterations);
		break;
	case Detail::WallRunMode:
		PhysWallRun(DeltaTime, Iterations);
		break;
	case Detail::WallHangMode:
		PhysWallHang(DeltaTime, Iterations);
		break;
	default:
		// user can fall back to custom implementation
		break;
	}
}

void UJuicyCharacterMovementComponent::PhysSlide(const float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion()
		&& !CurrentRootMotion.HasOverrideVelocity() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	if (!UpdatedComponent->IsQueryCollisionEnabled())
	{
		SetMovementMode(MOVE_Walking);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float RemainingTime = DeltaTime;

	// Perform the move
	while (RemainingTime >= MIN_TICK_TIME && Iterations < MaxSimulationIterations && CharacterOwner
		&& (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion()
			|| CurrentRootMotion.HasOverrideVelocity() || CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy))
	{
		Iterations++;
		bJustTeleported = false;
		const float TimeTick = GetSimulationTimeStep(RemainingTime, Iterations);
		RemainingTime -= TimeTick;

		// Save current values
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = OldBase != nullptr ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		RestorePreAdditiveRootMotionVelocity();

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;
		Acceleration = FVector::VectorPlaneProject(Acceleration, -GetGravityDirection());

		// Apply acceleration
		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(TimeTick, SlideFriction, false, GetMaxBrakingDeceleration());
		}

		ApplyRootMotionToVelocity(TimeTick);

		if (IsFalling())
		{
			// Root motion could have put us into Falling.
			// No movement has taken place this movement tick so we pass on full time/past iteration count
			StartNewPhysics(RemainingTime + TimeTick, Iterations - 1);
			return;
		}

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = TimeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;

		if (bZeroDelta)
		{
			RemainingTime = 0.0f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, TimeTick, &StepDownResult);

			if (IsFalling())
			{
				// pawn decided to jump up
				if (const float DesiredDist = Delta.Size();
					DesiredDist > UE_KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					RemainingTime += TimeTick * (1.0f - FMath::Min(1.0f, ActualDist / DesiredDist));
				}
				StartNewPhysics(RemainingTime, Iterations);
				return;
			}
			// just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, OldVelocity, TimeTick, RemainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, nullptr);
		}

		// check for ledges here
		if (const bool bCheckLedges = !CanWalkOffLedges();
			bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = GetGravityDirection();
			if (const FVector NewDelta = bTriedLedgeMove
				                             ? FVector::ZeroVector
				                             : GetLedgeMove(OldLocation, Delta, GravDir);
				!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / TimeTick;
				RemainingTime += TimeTick;
				continue;
			}

			// see if it is OK to jump
			if (bool bMustJump = bZeroDelta || (OldBase == nullptr
					|| (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				(bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation,
				                                          RemainingTime, TimeTick, Iterations, bMustJump))
			{
				return;
			}
			bCheckedFall = true;

			// revert this move
			RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
			RemainingTime = 0.0f;
			break;
		}

		// Validate the floor check
		if (CurrentFloor.IsWalkableFloor())
		{
			if (ShouldCatchAir(OldFloor, CurrentFloor))
			{
				HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal,
				                      OldLocation, TimeTick);
				if (IsMovingOnGround())
				{
					// If still walking, then fall. If not, assume the user set a different mode they want to keep.
					StartFalling(Iterations, RemainingTime, TimeTick, Delta, OldLocation);
				}
				return;
			}

			AdjustFloorHeight();
			SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
		}
		else if (CurrentFloor.HitResult.bStartPenetrating && RemainingTime <= 0.0f)
		{
			// The floor check failed because it started in penetration
			// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
			FHitResult Hit(CurrentFloor.HitResult);
			Hit.TraceEnd = Hit.TraceStart + FVector(0.0f, 0.0f, MAX_FLOOR_DIST);
			const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
			ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
			bForceNextFloorCheck = true;
		}

		// check if just entered water
		if (IsSwimming())
		{
			StartSwimming(OldLocation, Velocity, TimeTick, RemainingTime, Iterations);
			return;
		}

		// See if we need to start falling.
		if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
		{
			if (const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == nullptr
					|| (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				(bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation,
				                                          RemainingTime, TimeTick, Iterations, bMustJump))
			{
				return;
			}
			bCheckedFall = true;
		}

		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity()
				&& TimeTick >= MIN_TICK_TIME)
			{
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / TimeTick;
				if (FVector2D(Velocity).IsZero())
				{
					Velocity = Acceleration.GetSafeNormal2D() * MaxSlideSpeed;
				}
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			RemainingTime = 0.0f;
			break;
		}
	}

	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}

void UJuicyCharacterMovementComponent::PhysWallRun(const float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion()
		&& !CurrentRootMotion.HasOverrideVelocity() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float RemainingTime = DeltaTime;

	// Perform the move
	while (RemainingTime >= MIN_TICK_TIME && Iterations < MaxSimulationIterations && CharacterOwner
		&& (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion()
			|| CurrentRootMotion.HasOverrideVelocity() || CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy))
	{
		Iterations++;
		bJustTeleported = false;
		const float TimeTick = GetSimulationTimeStep(RemainingTime, Iterations);
		RemainingTime -= TimeTick;

		// Save current values
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		// Check if wall exists
		FHitResult WallHit;
		CheckWallExists(WallHit, -WallNormal);
		const float SinMinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallMinPullAwayAngle));
		// ReSharper disable once CppTooWideScopeInitStatement
		const bool bWantsToPullAway = WallHit.IsValidBlockingHit()
			&& (Acceleration.IsNearlyZero() || (Acceleration.GetSafeNormal() | WallHit.Normal) > SinMinPullAwayAngle);
		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(RemainingTime, Iterations);
			return;
		}
		WallNormal = WallHit.Normal;

		// Check floor
		FHitResult FloorHit;
		CheckFloorExists(FloorHit, WallHit);
		if (FloorHit.IsValidBlockingHit())
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(RemainingTime, Iterations);
			return;
		}

		// Clamp acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.0f;

		// Apply acceleration
		CalcVelocity(TimeTick, 0.0f, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		if (Velocity.SizeSquared2D() < pow(WallRunMinHorizontalSpeed, 2)
			|| FMath::Abs(Velocity.Z) > WallRunMaxVerticalSpeed)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(RemainingTime, Iterations);
			return;
		}

		// Apply gravity
		const FVector Gravity = -GetGravityDirection() * GetGravityZ();
		Velocity = NewFallVelocity(Velocity, Gravity, TimeTick);
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);

		// Apply actual movement
		if (const FVector Delta = Velocity * TimeTick;
			Delta.IsNearlyZero())
		{
			RemainingTime = 0.0f;
		}
		else
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
			const FVector WallAttractionDelta = -WallHit.Normal * WallAttractionForce * TimeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
		}

		// Make velocity reflect actual move
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / TimeTick;

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			RemainingTime = 0.0f;
			break;
		}
	}
}

void UJuicyCharacterMovementComponent::PhysWallHang(const float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion()
		&& !CurrentRootMotion.HasOverrideVelocity() && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float RemainingTime = DeltaTime;

	// Perform the move
	while (RemainingTime >= MIN_TICK_TIME && Iterations < MaxSimulationIterations && CharacterOwner
		&& (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion()
			|| CurrentRootMotion.HasOverrideVelocity() || CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy))
	{
		Iterations++;
		bJustTeleported = false;
		const float TimeTick = GetSimulationTimeStep(RemainingTime, Iterations);
		RemainingTime -= TimeTick;

		// Save current values
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		// Check if wall exists
		// ReSharper disable once CppTooWideScopeInitStatement
		FHitResult WallHit;
		CheckWallExists(WallHit, -WallNormal);
		const float SinMinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallMinPullAwayAngle));
		// ReSharper disable once CppTooWideScopeInitStatement
		const bool bWantsToPullAway = WallHit.IsValidBlockingHit()
			&& (Acceleration.IsNearlyZero() || (Acceleration.GetSafeNormal() | WallHit.Normal) > SinMinPullAwayAngle);
		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(RemainingTime, Iterations);
			return;
		}
		WallNormal = WallHit.Normal;

		// Check floor
		FHitResult FloorHit;
		CheckFloorExists(FloorHit, WallHit);
		if (FloorHit.IsValidBlockingHit())
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(RemainingTime, Iterations);
			return;
		}

		// Clamp acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.0f;

		// Apply acceleration
		CalcVelocity(TimeTick, 0.0f, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);

		// Apply gravity
		const FVector Gravity = -GetGravityDirection() * GetGravityZ();
		Velocity = NewFallVelocity(Velocity, Gravity, TimeTick);
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);

		// Apply actual movement
		if (const FVector Delta = Velocity * TimeTick;
			Delta.IsNearlyZero())
		{
			RemainingTime = 0.0f;
		}
		else
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
			const FVector WallAttractionDelta = -WallHit.Normal * WallAttractionForce * TimeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
		}

		// Make velocity reflect actual move
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / TimeTick;

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			RemainingTime = 0.0f;
			break;
		}
	}
}

bool UJuicyCharacterMovementComponent::TryMantle(FHitResult& FrontHit, FHitResult& SurfaceHit) const
{
	if (!CanMantleInCurrentState())
	{
		return false;
	}

	const UWorld* World = GetWorld();
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float Radius = Capsule->GetScaledCapsuleRadius();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float MaxReachHeight = HalfHeight * 2.0f + MantleReachHeight;
	const FVector FrontDirection = UpdatedComponent->GetForwardVector();
	const FVector FeetLocation = UpdatedComponent->GetComponentLocation() + FVector::DownVector * HalfHeight;
	const auto ProfileName = TEXT("BlockAll");
	const auto Params = Detail::CollisionQueryParamsWithoutActor(CharacterOwner);

	// Check wall
	FVector FrontStart = FeetLocation + FVector::UpVector * (MaxStepHeight - 1);
	const float CheckStepHeight = (HalfHeight * 2.0f - (MaxStepHeight - 1)) / MantleWallCheckFrequency;
	const FVector CheckStep = FVector::UpVector * CheckStepHeight;
	for (decltype(MantleWallCheckFrequency) i = 0; i < MantleWallCheckFrequency + 1; ++i)
	{
		const FVector FrontEnd = FrontStart + FrontDirection * (Radius + MantleMaxDistance);
		// ReSharper disable once CppTooWideScope
		const bool bFrontWasHit = World->LineTraceSingleByProfile(
			FrontHit, FrontStart, FrontEnd, ProfileName, Params);
		if (bFrontWasHit)
		{
			break;
		}
		FrontStart += CheckStep;
	}
	if (!FrontHit.IsValidBlockingHit())
	{
		return false;
	}
	const float CosMinSteepnessAngle = FMath::Cos(FMath::DegreesToRadians(MantleMinSteepnessAngle));
	const float CosMaxAlignmentAngle = FMath::Cos(FMath::DegreesToRadians(MantleMaxAlignmentAngle));
	const float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	// ReSharper disable once CppTooWideScopeInitStatement
	const float CosWallAlignmentAngle = FrontDirection | -FrontHit.Normal;
	if (FMath::Abs(CosWallSteepnessAngle) > CosMinSteepnessAngle || CosWallAlignmentAngle < CosMaxAlignmentAngle)
	{
		return false;
	}

	// Check wall surface
	TArray<FHitResult> HeightHits;
	const FVector WallUpDirection = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.Normal).GetSafeNormal();
	const float WallCos = FVector::UpVector | FrontHit.Normal;
	const float WallSin = FMath::Sqrt(1 - WallCos * WallCos);
	const FVector SurfaceStart = FrontHit.Location + FrontDirection
		+ WallUpDirection * (MaxReachHeight - (MaxStepHeight - 1)) / WallSin;
	const FVector SurfaceEnd = FrontHit.Location + FrontDirection;
	// ReSharper disable once CppTooWideScopeInitStatement
	const bool bSurfaceWasHit = World->LineTraceMultiByProfile(
		HeightHits, SurfaceStart, SurfaceEnd, ProfileName, Params);
	if (!bSurfaceWasHit)
	{
		return false;
	}
	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}
	const float CosMaxSurfaceAngle = FMath::Cos(FMath::DegreesToRadians(MantleMaxSurfaceAngle));
	// ReSharper disable once CppTooWideScopeInitStatement
	const float CosWallSurfaceAngle = SurfaceHit.Normal | FVector::UpVector;
	if (!SurfaceHit.IsValidBlockingHit() || CosWallSurfaceAngle < CosMaxSurfaceAngle)
	{
		return false;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	const float Height = (SurfaceHit.Location - FeetLocation) | FVector::UpVector;
	if (Height > MaxReachHeight)
	{
		return false;
	}

	// Check if character can fit
	const float SurfaceCos = FVector::UpVector | SurfaceHit.Normal;
	const float SurfaceSin = FMath::Sqrt(1 - SurfaceCos * SurfaceCos);
	const FVector CapsuleLocation = SurfaceHit.Location + FrontDirection * Radius
		+ FVector::UpVector * (HalfHeight + 1 + Radius * 2 * SurfaceSin);
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
	// ReSharper disable once CppTooWideScope
	const bool ShapeOverlaps = World->OverlapAnyTestByProfile(
		CapsuleLocation, FQuat::Identity, ProfileName, CapsuleShape, Params);
	if (ShapeOverlaps)
	{
		return false;
	}

	return true;
}

bool UJuicyCharacterMovementComponent::TryWallRun(FHitResult& FloorHit, FHitResult& WallHit) const
{
	if (!CanWallRunInCurrentState())
	{
		return false;
	}
	// Check if horizontal velocity is enough to start wall run
	if (Velocity.SizeSquared2D() < FMath::Pow(WallRunMinHorizontalSpeed, 2))
	{
		return false;
	}
	// Check if vertical velocity is not too high to start wall run
	if (FMath::Abs(Velocity.Z) > WallRunMaxVerticalSpeed)
	{
		return false;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	const bool bWallWasHit = CheckWallExists(WallHit, Acceleration.GetSafeNormal2D())
		&& WallHit.IsValidBlockingHit()
		&& (Velocity | WallHit.Normal) < 0
		&& (Acceleration | WallHit.Normal) < 0;
	if (!bWallWasHit)
	{
		return false;
	}
	// ReSharper disable once CppTooWideScopeInitStatement
	const FVector Forward = UpdatedComponent->GetForwardVector();
	if ((Forward | Acceleration) < 0)
	{
		return false;
	}

	const float CosMinSteepnessAngle = FMath::Cos(FMath::RadiansToDegrees(WallMinSteepnessAngle));
	// ReSharper disable once CppTooWideScopeInitStatement
	const float CosWallSteepnessAngle = WallHit.Normal.GetSafeNormal2D() | WallHit.Normal;
	if (FMath::Abs(CosWallSteepnessAngle) < CosMinSteepnessAngle)
	{
		return false;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	const bool bFloorWasHit = CheckFloorExists(FloorHit, WallHit);
	if (bFloorWasHit || FloorHit.IsValidBlockingHit())
	{
		return false;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	const FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	// Check if projected velocity is enough to start wall run
	if (ProjectedVelocity.SizeSquared2D() < FMath::Pow(WallRunMinHorizontalSpeed, 2))
	{
		return false;
	}

	return true;
}

bool UJuicyCharacterMovementComponent::TryWallHang(FHitResult& FloorHit, FHitResult& WallHit) const
{
	if (!CanWallHangInCurrentState())
	{
		return false;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	const bool bWallWasHit = CheckWallExists(WallHit, Acceleration.GetSafeNormal2D())
		&& WallHit.IsValidBlockingHit()
		&& (Velocity | WallHit.Normal) <= 0
		&& (Acceleration | WallHit.Normal) < 0;
	if (!bWallWasHit)
	{
		return false;
	}

	const float CosMinSteepnessAngle = FMath::Cos(FMath::RadiansToDegrees(WallMinSteepnessAngle));
	// ReSharper disable once CppTooWideScopeInitStatement
	const float CosWallSteepnessAngle = WallHit.Normal.GetSafeNormal2D() | WallHit.Normal;
	if (FMath::Abs(CosWallSteepnessAngle) < CosMinSteepnessAngle)
	{
		return false;
	}

	// ReSharper disable once CppTooWideScopeInitStatement
	const bool bFloorWasHit = CheckFloorExists(FloorHit, WallHit);
	if (bFloorWasHit || FloorHit.IsValidBlockingHit())
	{
		return false;
	}

	return true;
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

void UJuicyCharacterMovementComponent::OnMovementModeChanged(const EMovementMode PreviousMovementMode,
                                                             const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	const auto JuicyCharacterOwner = GetJuicyCharacterOwner();
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == Detail::SlideMode)
	{
		bWantsToCrouch = false;
		JuicyCharacterOwner->OnEndSlide();
	}
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == Detail::WallRunMode)
	{
		JuicyCharacterOwner->OnEndWallRun();
		StartWallRunCooldown();
	}
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == Detail::WallHangMode)
	{
		JuicyCharacterOwner->OnEndWallHang();
		StartWallHangCooldown();
	}

	if (IsSliding())
	{
		JuicyCharacterOwner->OnStartSlide();
		bCrouchMaintainsBaseLocation = true;
	}
}

void UJuicyCharacterMovementComponent::SetPostLandedPhysics(const FHitResult& Hit)
{
	if (CharacterOwner)
	{
		if (CanEverSwim() && IsInWater())
		{
			SetMovementMode(MOVE_Swimming);
		}
		else
		{
			const FVector PreImpactAccel = Acceleration
				+ (IsFalling() ? -GetGravityDirection() * GetGravityZ() : FVector::ZeroVector);
			const FVector PreImpactVelocity = Velocity;

			if (bWantsToSlide)
			{
				StartSlide();
			}
			else if (DefaultLandMovementMode == MOVE_Walking ||
				DefaultLandMovementMode == MOVE_NavWalking ||
				DefaultLandMovementMode == MOVE_Falling)
			{
				SetMovementMode(GetGroundMovementMode());
			}
			else
			{
				SetDefaultMovementMode();
			}

			ApplyImpactPhysicsForces(Hit, PreImpactAccel, PreImpactVelocity);
		}
	}
}

bool UJuicyCharacterMovementComponent::HasInput() const
{
	return !Acceleration.GetSafeNormal2D().IsNearlyZero();
}

void UJuicyCharacterMovementComponent::StartSlide()
{
	bWantsToCrouch = true;
	SetMovementMode(EJuicyCharacterMovementMode::Slide);
}

void UJuicyCharacterMovementComponent::EndSlide()
{
	bWantsToCrouch = false;
	Super::SetMovementMode(MOVE_Walking);
}

void UJuicyCharacterMovementComponent::StartDash()
{
	if (DashDuration <= 0.0f)
	{
		return;
	}

	CurrentDashDirection = HasInput()
		                       ? Acceleration.GetSafeNormal2D()
		                       : GetController()->GetControlRotation().Vector().GetSafeNormal2D();
	Super::SetMovementMode(MOVE_Falling);
	if (DashGravityScale == 0.0f)
	{
		Velocity = CurrentDashDirection * DashImpulse;
	}
	else
	{
		Velocity += CurrentDashDirection * DashImpulse;
	}

	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandleForDashDuration, this,
	                      &UJuicyCharacterMovementComponent::EndDash,
	                      DashDuration);
	GetJuicyCharacterOwner()->OnStartDash();
}

void UJuicyCharacterMovementComponent::EndDash()
{
	Super::SetMovementMode(MOVE_Falling);
	Velocity = CurrentDashDirection.GetSafeNormal() * MaxWalkSpeed;

	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.ClearTimer(TimerHandleForDashDuration);
	GetJuicyCharacterOwner()->OnEndDash();

	StartDashCooldown();
}

void UJuicyCharacterMovementComponent::StartDashCooldown()
{
	if (DashCooldown <= 0.0f)
	{
		return;
	}

	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandleForDashCooldown, this,
	                      &UJuicyCharacterMovementComponent::EndDashCooldown,
	                      DashCooldown);
	GetJuicyCharacterOwner()->OnStartDashCooldown();
}

void UJuicyCharacterMovementComponent::EndDashCooldown()
{
	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.ClearTimer(TimerHandleForDashCooldown);
	GetJuicyCharacterOwner()->OnEndDashCooldown();
}

void UJuicyCharacterMovementComponent::StartMantle()
{
	const auto JuicyCharacterOwner = GetJuicyCharacterOwner();

	FHitResult FrontHit, SurfaceHit;
	if (!TryMantle(FrontHit, SurfaceHit))
	{
		return;
	}

	bIsMantling = true;
	JuicyCharacterOwner->OnStartMantle(FrontHit, SurfaceHit);

	JuicyCharacterOwner->StopJumping();
	Acceleration = FVector::ZeroVector;
	Velocity = FVector::ZeroVector;
	Super::SetMovementMode(MOVE_Flying);
}

void UJuicyCharacterMovementComponent::StartWallRun()
{
	FHitResult FloorHit, WallHit;
	if (!TryWallRun(FloorHit, WallHit))
	{
		return;
	}

	WallNormal = WallHit.Normal;
	Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.0f, WallRunMaxVerticalSpeed);
	SetMovementMode(EJuicyCharacterMovementMode::WallRun);
	GetJuicyCharacterOwner()->OnStartWallRun(FloorHit, WallHit);
}

void UJuicyCharacterMovementComponent::StartWallRunCooldown()
{
	if (WallRunCooldown <= 0.0f)
	{
		return;
	}

	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandleForWallRunCooldown, this,
	                      &UJuicyCharacterMovementComponent::EndWallRunCooldown,
	                      WallRunCooldown);
	GetJuicyCharacterOwner()->OnStartWallRunCooldown();
}

void UJuicyCharacterMovementComponent::EndWallRunCooldown()
{
	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.ClearTimer(TimerHandleForWallRunCooldown);
	GetJuicyCharacterOwner()->OnEndWallRunCooldown();
}

void UJuicyCharacterMovementComponent::StartWallHang()
{
	FHitResult FloorHit, WallHit;
	if (!TryWallHang(FloorHit, WallHit))
	{
		return;
	}

	WallNormal = WallHit.Normal;
	Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	SetMovementMode(EJuicyCharacterMovementMode::WallHang);
	GetJuicyCharacterOwner()->OnStartWallHang(FloorHit, WallHit);
}

void UJuicyCharacterMovementComponent::StartWallHangCooldown()
{
	if (WallHangCooldown <= 0.0f)
	{
		return;
	}

	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandleForWallHangCooldown, this,
	                      &UJuicyCharacterMovementComponent::EndWallHangCooldown,
	                      WallHangCooldown);
	GetJuicyCharacterOwner()->OnStartWallHangCooldown();
}

void UJuicyCharacterMovementComponent::EndWallHangCooldown()
{
	FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	TimerManager.ClearTimer(TimerHandleForWallHangCooldown);
	GetJuicyCharacterOwner()->OnEndWallHangCooldown();
}

bool UJuicyCharacterMovementComponent::CheckFloorExists(FHitResult& FloorHit, const FHitResult& WallHit) const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector WallDown = FVector::VectorPlaneProject(FVector::DownVector, WallHit.Normal).GetSafeNormal();
	const FVector End = Start + WallDown * (HalfHeight + MinHeightAboveFloor);
	const auto ProfileName = TEXT("BlockAll");
	const auto Params = Detail::CollisionQueryParamsWithoutActor(CharacterOwner);

	return GetWorld()->LineTraceSingleByProfile(FloorHit, Start, End, ProfileName, Params);
}

bool UJuicyCharacterMovementComponent::CheckWallExists(FHitResult& WallHit, const FVector& Direction) const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float Radius = Capsule->GetScaledCapsuleRadius();

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector DistanceToWall = Direction * (Radius + MaxWallDistance);
	const FVector End = Start + DistanceToWall;
	const auto ProfileName = TEXT("BlockAll");
	const auto Params = Detail::CollisionQueryParamsWithoutActor(CharacterOwner);

	return GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, ProfileName, Params);
}
