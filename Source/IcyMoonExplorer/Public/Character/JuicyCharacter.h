#pragma once

#include "Components/AGRAnimMasterComponent.h"
#include "GameFramework/Character.h"
#include "JuicyCharacter.generated.h"

class UJuicyCharacterMovementComponent;

UCLASS()
class ICYMOONEXPLORER_API AJuicyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(Category=Character, EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="s"))
	float CoyoteTime;

	static FName AnimMasterComponentName;

	explicit AJuicyCharacter(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category=Character)
	UJuicyCharacterMovementComponent* GetJuicyCharacterMovement() const;

	UFUNCTION(BlueprintPure, Category=Character)
	UAGRAnimMasterComponent* GetAnimMaster() const;

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Slide();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void StopSliding();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool IsSliding() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanSlide() const;

	virtual void OnStartSlide();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartSlide", ScriptName="OnStartSlide"))
	void K2_OnStartSlide();

	virtual void OnEndSlide();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndSlide", ScriptName="OnEndSlide"))
	void K2_OnEndSlide();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Dash();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void StopDashing();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool IsDashing() const;

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool IsDashingCooldown() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanDash() const;

	virtual void OnStartDash();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartDash", ScriptName="OnStartDash"))
	void K2_OnStartDash();

	virtual void OnEndDash();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndDash", ScriptName="OnEndDash"))
	void K2_OnEndDash();

	virtual void OnStartDashCooldown();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartDashCooldown", ScriptName="OnStartDashCooldown"))
	void K2_OnStartDashCooldown();

	virtual void OnEndDashCooldown();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndDashCooldown", ScriptName="OnEndDashCooldown"))
	void K2_OnEndDashCooldown();

	UFUNCTION(BlueprintPure, Category=Character)
	bool IsCoyoteTime() const;

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Mantle();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void StopMantling();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool IsMantling() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanMantle() const;

	virtual void OnStartMantle(const FHitResult& FrontHit, const FHitResult& SurfaceHit);

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartMantle", ScriptName="OnStartMantle"))
	void K2_OnStartMantle(const FHitResult& FrontHit, const FHitResult& SurfaceHit);

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void ExitMantling();

	virtual void OnEndMantle();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndMantle", ScriptName="OnEndMantle"))
	void K2_OnEndMantle();

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool IsWallRunning() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool IsWallRunningCooldown() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanWallRun() const;

	virtual void OnStartWallRun(const FHitResult& FloorHit, const FHitResult& WallHit);

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartWallRun", ScriptName="OnStartWallRun"))
	void K2_OnStartWallRun(const FHitResult& FloorHit, const FHitResult& WallHit);

	virtual void OnEndWallRun();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndWallRun", ScriptName="OnEndWallRun"))
	void K2_OnEndWallRun();

	virtual void OnStartWallRunCooldown();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartWallRunCooldown", ScriptName="OnStartWallRunCooldown"))
	void K2_OnStartWallRunCooldown();

	virtual void OnEndWallRunCooldown();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndWallRunCooldown", ScriptName="OnEndWallRunCooldown"))
	void K2_OnEndWallRunCooldown();

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool IsWallHanging() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool IsWallHangingCooldown() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanWallHang() const;

	virtual void OnStartWallHang(const FHitResult& FloorHit, const FHitResult& WallHit);

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartWallHang", ScriptName="OnStartWallHang"))
	void K2_OnStartWallHang(const FHitResult& FloorHit, const FHitResult& WallHit);

	virtual void OnEndWallHang();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndWallHang", ScriptName="OnEndWallHang"))
	void K2_OnEndWallHang();

	virtual void OnStartWallHangCooldown();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnStartWallHangCooldown", ScriptName="OnStartWallHangCooldown"))
	void K2_OnStartWallHangCooldown();

	virtual void OnEndWallHangCooldown();

	UFUNCTION(BlueprintImplementableEvent,
		meta=(DisplayName="OnEndWallHangCooldown", ScriptName="OnEndWallHangCooldown"))
	void K2_OnEndWallHangCooldown();

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool IsOnWall() const;

	UFUNCTION(BlueprintPure, Category=Character)
	virtual FVector GetWallNormal() const;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void Landed(const FHitResult& Hit) override;

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAGRAnimMasterComponent> AnimMaster;

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UJuicyCharacterMovementComponent> JuicyCharacterMovement;

	FTimerHandle TimerHandleForCoyoteTime;

	void StartCoyoteTime();
	void EndCoyoteTime();
};
