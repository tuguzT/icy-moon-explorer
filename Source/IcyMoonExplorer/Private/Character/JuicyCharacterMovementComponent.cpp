#include "Character/JuicyCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Character/NetworkPredictionData_Client_JuicyCharacter.h"
#include "Character/JuicyCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogJuicyCharacterMovement, Log, All);

namespace Detail
{
	constexpr auto NoneMode = static_cast<uint8>(EJuicyCharacterMovementMode::None);
	constexpr auto SlideMode = static_cast<uint8>(EJuicyCharacterMovementMode::Slide);
	constexpr auto WallRunMode = static_cast<uint8>(EJuicyCharacterMovementMode::WallRun);
	constexpr auto WallHangMode = static_cast<uint8>(EJuicyCharacterMovementMode::WallHang);
	constexpr auto CustomMode = static_cast<uint8>(EJuicyCharacterMovementMode::Custom);

	constexpr uint8 PrefixForMovementMode(const EJuicyCharacterMovementMode MovementMode)
	{
		if (MovementMode != EJuicyCharacterMovementMode::Custom)
		{
			return 0;
		}
		return CustomMode;
	}

	constexpr uint8 NewCustomModeWithPrefix(const EJuicyCharacterMovementMode MovementMode,
	                                        const uint8 NewCustomMode)
	{
		const uint8 Prefix = PrefixForMovementMode(MovementMode);
		const auto MovementModeRaw = static_cast<uint8>(MovementMode);
		return MovementModeRaw + Prefix + NewCustomMode;
	}

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
	MantleMinSteepnessAngle = 75.0f;
	MantleMaxSurfaceAngle = 40.0f;
	MantleMaxAlignmentAngle = 45.0f;
	MantleWallCheckFrequency = 5;
	bWantsToMantle = false;
	bIsMantling = false;

	WallRunMinHorizontalSpeed = MaxWalkSpeed * 0.5f;
	WallRunMaxVerticalSpeed = JumpZVelocity * 0.5f;
	MaxWallRunSpeed = MaxWalkSpeed;
	WallRunMinPullAwayAngle = 75.0f;
	WallRunGravityScale = 0.0f;

	MaxWallHangSpeed = MaxWalkSpeed;
	WallHangGravityScale = 0.0f;
	WallHangCheckAroundCount = 4;

	MaxWallDistance = DefaultCapsuleRadius;
	MinHeightAboveFloor = DefaultCapsuleHalfHeight;
	JumpOffWallVerticalVelocity = JumpZVelocity * 0.5f;
	WallAttractionForce = JumpZVelocity * 0.5f;

	bIsOnRightWall = false;
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

void UJuicyCharacterMovementComponent::SetMovementMode(const EJuicyCharacterMovementMode NewMovementMode,
                                                       const uint8 NewCustomMode)
{
	const uint8 CustomMode = Detail::NewCustomModeWithPrefix(NewMovementMode, NewCustomMode);
	Super::SetMovementMode(MOVE_Custom, CustomMode);
}

bool UJuicyCharacterMovementComponent::IsMovementMode(const EMovementMode IsMovementMode,
                                                      const uint8 IsCustomMode) const
{
	if (MovementMode != IsMovementMode)
	{
		return false;
	}
	return MovementMode != MOVE_Custom || CustomMovementMode == IsCustomMode;
}

bool UJuicyCharacterMovementComponent::IsMovementMode(const EJuicyCharacterMovementMode IsMovementMode,
                                                      const uint8 IsCustomMode) const
{
	const uint8 CustomMode = Detail::NewCustomModeWithPrefix(IsMovementMode, IsCustomMode);
	return MovementMode == MOVE_Custom && CustomMovementMode == CustomMode;
}

void UJuicyCharacterMovementComponent::Slide()
{
	if (!CanSlideInCurrentState())
	{
		return;
	}

	bWantsToSlide = true;
	bWantsToCrouch = true;
}

void UJuicyCharacterMovementComponent::UnSlide()
{
	bWantsToSlide = false;
	bWantsToCrouch = false;
}

bool UJuicyCharacterMovementComponent::IsSliding() const
{
	return IsMovementMode(EJuicyCharacterMovementMode::Slide);
}

bool UJuicyCharacterMovementComponent::CanSlideInCurrentState() const
{
	// const bool HasMinSpeed = Velocity.SquaredLength() >= FMath::Sqrt(MinSlideSpeed);
	return HasInput()
		&& IsMovingOnGround()
		&& !IsMantling();
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

bool UJuicyCharacterMovementComponent::CanWallRunInCurrentState() const
{
	return HasInput() && IsFalling();
}

bool UJuicyCharacterMovementComponent::IsRunningOnRightWall() const
{
	return IsWallRunning() && bIsOnRightWall;
}

bool UJuicyCharacterMovementComponent::IsWallHanging() const
{
	return IsMovementMode(EJuicyCharacterMovementMode::WallHang);
}

bool UJuicyCharacterMovementComponent::CanWallHangInCurrentState() const
{
	return HasInput() && IsFalling();
}

bool UJuicyCharacterMovementComponent::IsHangingOnRightWall() const
{
	return IsWallHanging() && bIsOnRightWall;
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
	return Super::CanAttemptJump()
		&& !IsSliding()
		&& !IsDashing()
		&& !IsMantling()
		|| IsWallRunning()
		|| IsWallHanging();
}

bool UJuicyCharacterMovementComponent::DoJump(const bool bReplayingMoves)
{
	const bool bWasWallRunning = IsWallRunning();
	const bool bWasWallHanging = IsWallHanging();
	if (!Super::DoJump(bReplayingMoves))
	{
		return false;
	}

	if (bWasWallRunning || bWasWallHanging)
	{
		FHitResult WallHit;
		if (bWasWallRunning)
		{
			CheckWallExists(WallHit, bIsOnRightWall);
		}
		else
		{
			CheckWallExists(WallHit);
		}
		Velocity += WallHit.Normal * JumpOffWallVerticalVelocity;
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
	else if (IsMantling())
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		Super::SetMovementMode(MOVE_Flying);
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
	case Detail::CustomMode:
		return MaxCustomMovementSpeed;
	default:
		return 0.0f;
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

void UJuicyCharacterMovementComponent::PhysCustom(const float DeltaTime, const int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case Detail::NoneMode:
		break;
	case Detail::SlideMode:
		PhysSlide(DeltaTime, Iterations);
		break;
	case Detail::WallRunMode:
		PhysWallRun(DeltaTime, Iterations);
		break;
	case Detail::WallHangMode:
		PhysWallHang(DeltaTime, Iterations);
		break;
	case Detail::CustomMode:
		// user can fall back to custom implementation
		break;
	default:
		UE_LOG(LogJuicyCharacterMovement, Warning, TEXT("%s has unsupported movement mode %d"),
		       *GetJuicyCharacterOwner()->GetName(),
		       static_cast<int32>(CustomMovementMode))
		Super::SetMovementMode(MOVE_None);
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

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		// Project acceleration onto gravity direction plane
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
				if (Velocity.IsZero())
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
		const bool bCanReachWall = CheckWallExists(WallHit, bIsOnRightWall);
		const float SinMinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunMinPullAwayAngle));
		// ReSharper disable once CppTooWideScopeInitStatement
		const bool bWantsToPullAway = bCanReachWall
			&& WallHit.IsValidBlockingHit()
			&& !Acceleration.IsNearlyZero()
			&& (Acceleration.GetSafeNormal() | WallHit.Normal) > SinMinPullAwayAngle;
		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
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

	// Check invariants of wall run mode
	FHitResult FloorHit, WallHit;
	CheckFloorExists(FloorHit);
	CheckWallExists(WallHit, bIsOnRightWall);
	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit()
		|| Velocity.SizeSquared2D() < pow(WallRunMinHorizontalSpeed, 2))
	{
		SetMovementMode(MOVE_Falling);
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
		if (!CheckWallExists(WallHit) || !WallHit.IsValidBlockingHit())
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
		Acceleration = FVector::ZeroVector;

		// Apply gravity
		const FVector Gravity = -GetGravityDirection() * GetGravityZ();
		Velocity = NewFallVelocity(Velocity, Gravity, TimeTick);

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

	// Check invariants of wall hang mode
	FHitResult FloorHit, WallHit;
	CheckFloorExists(FloorHit);
	CheckWallExists(WallHit);
	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit())
	{
		SetMovementMode(MOVE_Falling);
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
	const FVector FrontDirection = Acceleration.GetSafeNormal2D();
	const FVector FeetLocation = UpdatedComponent->GetComponentLocation() + FVector::DownVector * HalfHeight;
	const float CosMinSteepnessAngle = FMath::Cos(FMath::DegreesToRadians(MantleMinSteepnessAngle));
	const float CosMaxSurfaceAngle = FMath::Cos(FMath::DegreesToRadians(MantleMaxSurfaceAngle));
	const float CosMaxAlignmentAngle = FMath::Cos(FMath::DegreesToRadians(MantleMaxAlignmentAngle));
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
	if (!SurfaceHit.IsValidBlockingHit()
		|| (SurfaceHit.Normal | FVector::UpVector) < CosMaxSurfaceAngle)
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
	const bool bFloorWasHit = CheckFloorExists(FloorHit);
	if (bFloorWasHit || FloorHit.IsValidBlockingHit())
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
	const bool bFloorWasHit = CheckFloorExists(FloorHit);
	if (bFloorWasHit || FloorHit.IsValidBlockingHit())
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
		UnSlide();
		JuicyCharacterOwner->OnEndSlide();
	}
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == Detail::WallRunMode)
	{
		JuicyCharacterOwner->OnEndWallRun();
	}
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == Detail::WallHangMode)
	{
		JuicyCharacterOwner->OnEndWallHang();
	}

	if (IsSliding())
	{
		Slide();
		JuicyCharacterOwner->OnStartSlide();
		bCrouchMaintainsBaseLocation = true;
	}
}

bool UJuicyCharacterMovementComponent::HasInput() const
{
	return !Acceleration.GetSafeNormal2D().IsNearlyZero();
}

void UJuicyCharacterMovementComponent::ResetCharacterRotation(const FVector& Forward, const bool bSweep)
{
	FHitResult OutHit;
	const FQuat RestoredRotation = FRotationMatrix::MakeFromXZ(Forward, FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, RestoredRotation, bSweep, OutHit);
}

void UJuicyCharacterMovementComponent::StartSlide()
{
	SetMovementMode(EJuicyCharacterMovementMode::Slide);
}

void UJuicyCharacterMovementComponent::EndSlide()
{
	const FVector Forward = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	ResetCharacterRotation(Forward, true);

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
	ResetCharacterRotation(CurrentDashDirection, false);
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

	FHitResult CheckDirectionHit;
	bIsOnRightWall = CheckWallExists(CheckDirectionHit, true);
	Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.0f, WallRunMaxVerticalSpeed);
	GetJuicyCharacterOwner()->OnStartWallRun(FloorHit, WallHit);
	SetMovementMode(EJuicyCharacterMovementMode::WallRun);
}

void UJuicyCharacterMovementComponent::StartWallHang()
{
	FHitResult FloorHit, WallHit;
	if (!TryWallHang(FloorHit, WallHit))
	{
		return;
	}

	FHitResult CheckDirectionHit;
	bIsOnRightWall = CheckWallExists(CheckDirectionHit, true);
	Velocity = FVector::ZeroVector;
	GetJuicyCharacterOwner()->OnStartWallHang(FloorHit, WallHit);
	SetMovementMode(EJuicyCharacterMovementMode::WallHang);
}

bool UJuicyCharacterMovementComponent::CheckFloorExists(FHitResult& FloorHit) const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * (HalfHeight + MinHeightAboveFloor);
	const auto ProfileName = TEXT("BlockAll");
	const auto Params = Detail::CollisionQueryParamsWithoutActor(CharacterOwner);

	return GetWorld()->LineTraceSingleByProfile(FloorHit, Start, End, ProfileName, Params);
}

bool UJuicyCharacterMovementComponent::CheckWallExists(FHitResult& WallHit, const bool bCheckAtRight) const
{
	const FVector Right = UpdatedComponent->GetRightVector();
	return CheckWallExists(WallHit, bCheckAtRight ? Right : -Right);
}

bool UJuicyCharacterMovementComponent::CheckWallExists(FHitResult& WallHit) const
{
	FVector Direction = UpdatedComponent->GetForwardVector();
	for (uint8 Index = 0; Index < WallHangCheckAroundCount; ++Index)
	{
		const float Angle = 360.0f / WallHangCheckAroundCount * Index;
		const FVector Up = UpdatedComponent->GetUpVector();
		Direction = Direction.RotateAngleAxis(Angle, Up);
		if (CheckWallExists(WallHit, Direction))
		{
			return true;
		}
	}
	return false;
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
