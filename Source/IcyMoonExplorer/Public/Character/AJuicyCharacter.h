#pragma once

#include "GameFramework/Character.h"
#include "AJuicyCharacter.generated.h"

class UJuicyCharacterMovementComponent;

UCLASS()
class ICYMOONEXPLORER_API AJuicyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	explicit AJuicyCharacter(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UJuicyCharacterMovementComponent* GetJuicyCharacterMovement() const;

	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void Slide();

	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void StopSliding();

	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool CanSlide() const;

	virtual void OnStartSlide();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartSlide", ScriptName="OnStartSlide"))
	void K2_OnStartSlide();

	virtual void OnEndSlide();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndSlide", ScriptName="OnEndSlide"))
	void K2_OnEndSlide();

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UJuicyCharacterMovementComponent> JuicyCharacterMovement;
};
