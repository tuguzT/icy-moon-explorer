#include "Character/JuicyCharacter.h"

#include "Character/JuicyCharacterMovementComponent.h"

namespace Detail
{
	static const FObjectInitializer& ReplaceCharacterMovement(const FObjectInitializer& Initializer)
	{
		const auto Name = ACharacter::CharacterMovementComponentName;
		return Initializer.SetDefaultSubobjectClass<UJuicyCharacterMovementComponent>(Name);
	}
}

AJuicyCharacter::AJuicyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(Detail::ReplaceCharacterMovement(ObjectInitializer))
{
	CoyoteTime = 0.0f;

	JuicyCharacterMovement = Cast<UJuicyCharacterMovementComponent>(GetCharacterMovement());
}

UJuicyCharacterMovementComponent* AJuicyCharacter::GetJuicyCharacterMovement() const
{
	return JuicyCharacterMovement;
}

void AJuicyCharacter::Slide()
{
	if (CanSlide())
	{
		JuicyCharacterMovement->Slide();
	}
}

void AJuicyCharacter::StopSliding()
{
	if (JuicyCharacterMovement)
	{
		JuicyCharacterMovement->UnSlide();
	}
}

bool AJuicyCharacter::CanSlide() const
{
	return JuicyCharacterMovement
		&& !JuicyCharacterMovement->IsSliding()
		&& JuicyCharacterMovement->CanSlideInCurrentState();
}

void AJuicyCharacter::OnStartSlide()
{
	K2_OnStartSlide();
}

void AJuicyCharacter::OnEndSlide()
{
	K2_OnEndSlide();
}

void AJuicyCharacter::Dash()
{
	if (CanDash())
	{
		JuicyCharacterMovement->Dash();
	}
}

void AJuicyCharacter::StopDashing()
{
	if (JuicyCharacterMovement)
	{
		JuicyCharacterMovement->UnDash();
	}
}

bool AJuicyCharacter::CanDash() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->CanDashInCurrentState();
}

void AJuicyCharacter::OnStartDash()
{
	K2_OnStartDash();
}

void AJuicyCharacter::OnEndDash()
{
	K2_OnEndDash();
}

void AJuicyCharacter::OnStartDashCooldown()
{
	K2_OnStartDashCooldown();
}

void AJuicyCharacter::OnEndDashCooldown()
{
	K2_OnEndDashCooldown();
}

bool AJuicyCharacter::IsCoyoteTime() const
{
	const FTimerManager& TimerManager = GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForCoyoteTime);
}

void AJuicyCharacter::Mantle()
{
	if (CanMantle())
	{
		JuicyCharacterMovement->Mantle();
	}
}

void AJuicyCharacter::StopMantling()
{
	if (JuicyCharacterMovement)
	{
		JuicyCharacterMovement->UnMantle();
	}
}

bool AJuicyCharacter::CanMantle() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->CanMantleInCurrentState();
}

void AJuicyCharacter::OnStartMantle(const FHitResult& FrontHit, const FHitResult& SurfaceHit)
{
	K2_OnStartMantle(FrontHit, SurfaceHit);
}

void AJuicyCharacter::ExitMantling()
{
	if (JuicyCharacterMovement)
	{
		JuicyCharacterMovement->ExitMantling();
	}
}

void AJuicyCharacter::OnEndMantle()
{
	K2_OnEndMantle();
}

void AJuicyCharacter::OnMovementModeChanged(const EMovementMode PrevMovementMode, const uint8 PreviousCustomMode)
{
	if (const bool bWasNotFlying = PrevMovementMode != MOVE_Flying;
		bWasNotFlying && JuicyCharacterMovement->IsFalling())
	{
		StartCoyoteTime();
	}

	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

bool AJuicyCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() || IsCoyoteTime();
}

void AJuicyCharacter::Landed(const FHitResult& Hit)
{
	EndCoyoteTime();

	Super::Landed(Hit);
}

void AJuicyCharacter::StartCoyoteTime()
{
	if (CoyoteTime <= 0.0f)
	{
		return;
	}

	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandleForCoyoteTime, this,
	                      &AJuicyCharacter::EndCoyoteTime, CoyoteTime);
}

void AJuicyCharacter::EndCoyoteTime()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.ClearTimer(TimerHandleForCoyoteTime);
}
