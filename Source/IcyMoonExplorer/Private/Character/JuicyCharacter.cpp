#include "Character/JuicyCharacter.h"

#include "AI/JuicyAILibrary.h"
#include "Character/JuicyCharacterMovementComponent.h"

namespace Detail
{
	static const FObjectInitializer& ReplaceCharacterMovement(const FObjectInitializer& Initializer)
	{
		const auto Name = ACharacter::CharacterMovementComponentName;
		return Initializer.SetDefaultSubobjectClass<UJuicyCharacterMovementComponent>(Name);
	}
}

FName AJuicyCharacter::AnimMasterComponentName = TEXT("AnimMasterComp");

AJuicyCharacter::AJuicyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(Detail::ReplaceCharacterMovement(ObjectInitializer))
{
	AnimMaster = CreateDefaultSubobject<UAGRAnimMasterComponent>(AnimMasterComponentName);
	JuicyCharacterMovement = Cast<UJuicyCharacterMovementComponent>(GetCharacterMovement());

	CoyoteTime = 0.0f;
}

UJuicyCharacterMovementComponent* AJuicyCharacter::GetJuicyCharacterMovement() const
{
	return JuicyCharacterMovement;
}

UAGRAnimMasterComponent* AJuicyCharacter::GetAnimMaster() const
{
	return AnimMaster;
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

bool AJuicyCharacter::IsSliding() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsSliding();
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

bool AJuicyCharacter::IsDashing() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsDashing();
}

bool AJuicyCharacter::IsDashingCooldown() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsDashingCooldown();
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

bool AJuicyCharacter::IsMantling() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsMantling();
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

bool AJuicyCharacter::IsWallRunning() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsWallRunning();
}

bool AJuicyCharacter::IsWallRunningCooldown() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsWallRunningCooldown();
}

bool AJuicyCharacter::CanWallRun() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->CanWallRunInCurrentState();
}

void AJuicyCharacter::OnStartWallRun(const FHitResult& FloorHit, const FHitResult& WallHit)
{
	K2_OnStartWallRun(FloorHit, WallHit);
}

void AJuicyCharacter::OnEndWallRun()
{
	K2_OnEndWallRun();
}

void AJuicyCharacter::OnStartWallRunCooldown()
{
	K2_OnStartWallRunCooldown();
}

void AJuicyCharacter::OnEndWallRunCooldown()
{
	K2_OnEndWallRunCooldown();
}

bool AJuicyCharacter::IsWallHanging() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsWallHanging();
}

bool AJuicyCharacter::IsWallHangingCooldown() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsWallHangingCooldown();
}

bool AJuicyCharacter::CanWallHang() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->CanWallHangInCurrentState();
}

void AJuicyCharacter::OnStartWallHang(const FHitResult& FloorHit, const FHitResult& WallHit)
{
	K2_OnStartWallHang(FloorHit, WallHit);
}

void AJuicyCharacter::OnEndWallHang()
{
	K2_OnEndWallHang();
}

void AJuicyCharacter::OnStartWallHangCooldown()
{
	K2_OnStartWallHangCooldown();
}

void AJuicyCharacter::OnEndWallHangCooldown()
{
	K2_OnEndWallHangCooldown();
}

bool AJuicyCharacter::IsOnWall() const
{
	return JuicyCharacterMovement
		&& JuicyCharacterMovement->IsOnWall();
}

FVector AJuicyCharacter::GetWallNormal() const
{
	if (!JuicyCharacterMovement)
	{
		return FVector::ZeroVector;
	}
	return JuicyCharacterMovement->GetWallNormal();
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
	return JumpIsAllowedInternal() || IsCoyoteTime();
}

void AJuicyCharacter::Landed(const FHitResult& Hit)
{
	EndCoyoteTime();

	Super::Landed(Hit);
}

void AJuicyCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	K2_GetActorEyesViewPoint(OutLocation, OutRotation);
}

void AJuicyCharacter::K2_GetActorEyesViewPoint_Implementation(FVector& OutLocation, FRotator& OutRotation) const
{
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);
}

void AJuicyCharacter::SetTeamId_Implementation(const uint8 TeamID)
{
	if (Controller && Controller->Implements<UJuicyAITeam>()
		&& Execute_GetTeamId(Controller) != TeamID)
	{
		Execute_SetTeamId(Controller, TeamID);
		UJuicyAILibrary::UpdateSource(this);
	}
}

uint8 AJuicyCharacter::GetTeamId_Implementation() const
{
	if (Controller && Controller->Implements<UJuicyAITeam>())
	{
		return Execute_GetTeamId(Controller);
	}
	return FGenericTeamId::NoTeam;
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
