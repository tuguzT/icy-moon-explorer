﻿#pragma once

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

	explicit AJuicyCharacter(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category=Character)
	UJuicyCharacterMovementComponent* GetJuicyCharacterMovement() const;

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Slide();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void StopSliding();

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanSlide() const;

	virtual void OnStartSlide();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartSlide", ScriptName="OnStartSlide"))
	void K2_OnStartSlide();

	virtual void OnEndSlide();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndSlide", ScriptName="OnEndSlide"))
	void K2_OnEndSlide();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Dash();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void StopDashing();

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanDash() const;

	virtual void OnStartDash();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartDash", ScriptName="OnStartDash"))
	void K2_OnStartDash();

	virtual void OnEndDash();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndDash", ScriptName="OnEndDash"))
	void K2_OnEndDash();

	virtual void OnStartDashCooldown();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartDashCooldown", ScriptName="OnStartDashCooldown"))
	void K2_OnStartDashCooldown();

	virtual void OnEndDashCooldown();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndDashCooldown", ScriptName="OnEndDashCooldown"))
	void K2_OnEndDashCooldown();

	UFUNCTION(BlueprintPure, Category=Character)
	bool IsCoyoteTime() const;

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Mantle();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void StopMantling();

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanMantle() const;

	virtual void OnStartMantle(const FHitResult& FrontHit, const FHitResult& SurfaceHit);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartMantle", ScriptName="OnStartMantle"))
	void K2_OnStartMantle(const FHitResult& FrontHit, const FHitResult& SurfaceHit);

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void ExitMantling();

	virtual void OnEndMantle();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndMantle", ScriptName="OnEndMantle"))
	void K2_OnEndMantle();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void Landed(const FHitResult& Hit) override;

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UJuicyCharacterMovementComponent> JuicyCharacterMovement;

	FTimerHandle TimerHandleForCoyoteTime;

	void StartCoyoteTime();
	void EndCoyoteTime();
};
