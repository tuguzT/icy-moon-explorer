#pragma once

#include "GameFramework/Character.h"
#include "JuicyCharacter.generated.h"

class UJuicyCharacterMovementComponent;

UCLASS()
class ICYMOONEXPLORER_API AJuicyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	explicit AJuicyCharacter(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category=Character)
	UJuicyCharacterMovementComponent* GetJuicyCharacterMovement() const;

	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void Slide();

	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void StopSliding();

	UFUNCTION(BlueprintPure, Category=Character)
	virtual bool CanSlide() const;

	virtual void OnStartSlide();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartSlide", ScriptName="OnStartSlide"))
	void K2_OnStartSlide();

	virtual void OnEndSlide();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndSlide", ScriptName="OnEndSlide"))
	void K2_OnEndSlide();

	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void Dash();

	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
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

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UJuicyCharacterMovementComponent> JuicyCharacterMovement;
};
