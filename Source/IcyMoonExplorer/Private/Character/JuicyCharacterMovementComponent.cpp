#include "Character/JuicyCharacterMovementComponent.h"

#include "Character/NetworkPredictionData_Client_JuicyCharacter.h"
#include "Character/JuicyCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogJuicyCharacterMovement, Log, All);

namespace Detail
{
	constexpr auto NoneMode = static_cast<uint8>(EJuicyCharacterMovementMode::None);
	constexpr auto SlideMode = static_cast<uint8>(EJuicyCharacterMovementMode::Slide);
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
}

UJuicyCharacterMovementComponent::UJuicyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NavAgentProps.bCanCrouch = true;

	MaxSlideSpeed = MaxWalkSpeed * 2.0f;
	SlideFriction = GroundFriction;
	BrakingDecelerationSliding = BrakingDecelerationWalking;
	bCanSlideOffLedges = bCanWalkOffLedges;
	bWantsToSlide = false;

	DashImpulse = MaxWalkSpeed * 3.0f;
	DashDuration = 0.5f;
	DashCooldown = 1.0f;
	bWantsToDash = false;
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

bool UJuicyCharacterMovementComponent::IsMovementMode(const EJuicyCharacterMovementMode IsMovementMode,
                                                      const uint8 NewCustomMode) const
{
	const uint8 CustomMode = Detail::NewCustomModeWithPrefix(IsMovementMode, NewCustomMode);
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
	return HasInput() && IsMovingOnGround();
}

void UJuicyCharacterMovementComponent::Dash()
{
	bWantsToDash = true;
}

bool UJuicyCharacterMovementComponent::IsDashing() const
{
	const FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForDashDuration);
}

bool UJuicyCharacterMovementComponent::IsDashingCooldown() const
{
	const FTimerManager& TimerManager = CharacterOwner->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForDashCooldown);
}

bool UJuicyCharacterMovementComponent::CanDashInCurrentState() const
{
	const bool IsAllowedMode = MovementMode == MOVE_Walking
		|| MovementMode == MOVE_NavWalking
		|| MovementMode == MOVE_Falling
		|| MovementMode == MOVE_Flying;
	const bool IsNotDashing = !IsDashing();
	const bool IsNotDashingCooldown = !IsDashingCooldown();

	return UpdatedComponent
		&& IsAllowedMode
		&& IsNotDashing
		&& IsNotDashingCooldown;
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

bool UJuicyCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump()
		&& !IsSliding()
		&& !IsDashing();
}

// ReSharper disable once CppTooWideScopeInitStatement
void UJuicyCharacterMovementComponent::UpdateCharacterStateBeforeMovement(const float DeltaSeconds)
{
	// Sliding
	if (MovementMode == MOVE_Walking && bWantsToSlide && CanSlideInCurrentState())
	{
		StartSlide();
	}
	else if (IsSliding() && !bWantsToSlide)
	{
		EndSlide();
	}

	// Dashing
	if (bWantsToDash && CanDashInCurrentState())
	{
		StartDash();
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

bool UJuicyCharacterMovementComponent::IsMovingOnGround() const
{
	const bool bSuitableMode = MovementMode == MOVE_Walking
		|| MovementMode == MOVE_NavWalking
		|| IsSliding();
	return bSuitableMode && UpdatedComponent;
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

	if (!CanSlideInCurrentState())
	{
		Super::SetMovementMode(MOVE_Walking);
		StartNewPhysics(DeltaTime, Iterations);
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

		Velocity += GetGravityDirection() * TimeTick;
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

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == Detail::SlideMode)
	{
		UnSlide();
	}

	if (IsSliding())
	{
		Slide();
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
	GetJuicyCharacterOwner()->OnStartSlide();
}

void UJuicyCharacterMovementComponent::EndSlide()
{
	Super::SetMovementMode(MOVE_Walking);
	ResetCharacterRotation(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), true);
	GetJuicyCharacterOwner()->OnEndSlide();
}

void UJuicyCharacterMovementComponent::StartDash()
{
	if (DashDuration <= 0.0f)
	{
		return;
	}

	CurrentDashDirection = (HasInput() ? Acceleration : UpdatedComponent->GetForwardVector()).GetSafeNormal2D();
	ResetCharacterRotation(CurrentDashDirection, false);
	Super::SetMovementMode(MOVE_Flying);
	Velocity = CurrentDashDirection * DashImpulse;

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
