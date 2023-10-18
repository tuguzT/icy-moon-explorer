#include "Character/JuicyCharacterMovementComponent.h"

#include "Character/NetworkPredictionData_Client_JuicyCharacter.h"
#include "Character/JuicyCharacter.h"
#include "Components/CapsuleComponent.h"

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
		const auto Prefix = PrefixForMovementMode(MovementMode);
		const auto MovementModeRaw = static_cast<uint8>(MovementMode);
		return MovementModeRaw + Prefix + NewCustomMode;
	}

	FCollisionQueryParams CollisionQueryParamsForSliding(const AActor* Actor)
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
	MaxSlideSpeed = 900.0f;
	SlideFriction = 2.0f;
	SlideHalfHeight = 30.0f;
	bWantsToSlide = false;

	DashImpulse = 1200.0f;
	DashDuration = 0.5f;
	DashCooldown = 1.0f;
	bWantsToDash = false;
}

FORCEINLINE AJuicyCharacter* UJuicyCharacterMovementComponent::GetJuicyCharacterOwner() const
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
	const auto CustomMode = Detail::NewCustomModeWithPrefix(NewMovementMode, NewCustomMode);
	Super::SetMovementMode(MOVE_Custom, CustomMode);
}

bool UJuicyCharacterMovementComponent::IsMovementMode(const EJuicyCharacterMovementMode IsMovementMode,
                                                      const uint8 NewCustomMode) const
{
	const auto CustomMode = Detail::NewCustomModeWithPrefix(IsMovementMode, NewCustomMode);
	return MovementMode == MOVE_Custom && CustomMovementMode == CustomMode;
}

void UJuicyCharacterMovementComponent::Slide()
{
	if (!CanSlideInCurrentState())
	{
		return;
	}

	bWantsToSlide = true;
	SetMovementMode(EJuicyCharacterMovementMode::Slide);
	ChangeHalfHeightBeforeSliding();
	GetJuicyCharacterOwner()->OnStartSlide();
}

void UJuicyCharacterMovementComponent::UnSlide()
{
	const FVector Forward = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	ResetCharacterRotation(Forward, true);

	bWantsToSlide = false;
	Super::SetMovementMode(MOVE_Walking);
	RestoreHalfHeightAfterSliding();
	GetJuicyCharacterOwner()->OnEndSlide();
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

	if (CanDashInCurrentState())
	{
		StartDash();
	}
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

bool UJuicyCharacterMovementComponent::IsMovingOnGround() const
{
	const bool IsAllowedMode = MovementMode == MOVE_Walking
		|| MovementMode == MOVE_NavWalking
		|| IsSliding();
	return IsAllowedMode && UpdatedComponent;
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
		PhysJuicyCustom(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogJuicyCharacterMovement, Warning, TEXT("%s has unsupported movement mode %d"),
		       *GetJuicyCharacterOwner()->GetName(),
		       static_cast<int32>(CustomMovementMode))
		Super::SetMovementMode(MOVE_None);
		break;
	}
}

void UJuicyCharacterMovementComponent::PhysJuicyCustom(float DeltaTime, int32 Iterations)
{
	// to be implemented by derived class
}

void UJuicyCharacterMovementComponent::PhysSlide(const float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	// Check if any surface is underneath us and if we can actually slide
	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || !CanSlideInCurrentState())
	{
		UnSlide();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	// Apply gravity
	Velocity += FVector::DownVector * DeltaTime * GravityScale;

	// Apply acceleration
	Velocity += Acceleration.GetSafeNormal2D() * MaxSlideSpeed;
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime, SlideFriction, true, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(DeltaTime);

	// Actual sliding implementation
	Iterations++;
	bJustTeleported = false;

	constexpr float HitTime = 1.0f;
	FHitResult Hit(HitTime);
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * DeltaTime;
	const FVector SurfaceNormal = SurfaceHit.Normal;
	const FVector VelocityPlaneDirection = FVector::VectorPlaneProject(Velocity, SurfaceNormal).GetSafeNormal();
	const FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelocityPlaneDirection, SurfaceNormal).ToQuat();
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit);

	// Slide along surface if we hit the wall
	if (Hit.Time < HitTime)
	{
		HandleImpact(Hit, DeltaTime, Adjusted);
		SlideAlongSurface(Adjusted, HitTime - Hit.Time, Hit.Normal, Hit, true);
	}

	// Update velocity accordingly to possible wall hit
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
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

void UJuicyCharacterMovementComponent::UpdateCharacterStateBeforeMovement(const float DeltaSeconds)
{
	// Check for a change in sliding state.
	// Players toggle sliding by changing bWantsToSlide.
	if (const bool bIsSliding = IsSliding();
		bIsSliding && (!bWantsToSlide || !CanSlideInCurrentState()))
	{
		UnSlide();
	}
	else if (!bIsSliding && bWantsToSlide && CanSlideInCurrentState())
	{
		Slide();
	}

	// Check for a change in dashing state.
	// Players toggle sliding by changing bWantsToDash.
	if (bWantsToDash && CanDashInCurrentState())
	{
		Dash();
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

bool UJuicyCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump()
		&& !IsSliding()
		&& !IsDashing();
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

bool UJuicyCharacterMovementComponent::GetSlideSurface(FHitResult& OutHit) const
{
	const float ScaledCapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + ScaledCapsuleHalfHeight * 2.5f * FVector::DownVector;
	const FName ProfileName = TEXT("BlockAll");
	const FCollisionQueryParams Params = Detail::CollisionQueryParamsForSliding(CharacterOwner);
	return GetWorld()->LineTraceSingleByProfile(OutHit, Start, End, ProfileName, Params);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UJuicyCharacterMovementComponent::ChangeHalfHeightBeforeSliding()
{
	CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(SlideHalfHeight);
	const FVector OldLocation = CharacterOwner->GetActorLocation();
	CharacterOwner->SetActorLocation(OldLocation + FVector::DownVector * SlideHalfHeight);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UJuicyCharacterMovementComponent::RestoreHalfHeightAfterSliding()
{
	const auto* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<AJuicyCharacter>();
	const auto* DefaultCapsule = DefaultCharacter->GetCapsuleComponent();
	const float DefaultHalfHeight = DefaultCapsule->GetScaledCapsuleHalfHeight();

	CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultHalfHeight);
	const FVector OldLocation = CharacterOwner->GetActorLocation();
	CharacterOwner->SetActorLocation(OldLocation + FVector::UpVector * SlideHalfHeight);
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
