#pragma once

#include "Components/ActorComponent.h"
#include "JuicyTakeDamageComponent.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamageSignature, float, Damage);

UCLASS(ClassGroup=(Damage), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyTakeDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Components|Take Damage")
	FOnTakeDamageSignature OnTakeDamage;

	explicit UJuicyTakeDamageComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category="Components|Take Damage")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category="Components|Take Damage")
	void SetMaxHealth(float NewMaxHealth);

protected:
	UPROPERTY(Category="Take Damage (General Settings)", EditAnywhere,
		BlueprintGetter=GetHealth, BlueprintSetter=SetHealth,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float Health;

	UPROPERTY(Category="Take Damage (General Settings)", EditAnywhere,
		BlueprintGetter=GetMaxHealth, BlueprintSetter=SetMaxHealth,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float MaxHealth;

private:
	UFUNCTION()
	void OnTakeAnyDamage_OwnerDelegate(AActor* DamagedActor, float Damage,
	                                   const UDamageType* DamageType,
	                                   AController* InstigatedBy, AActor* DamageCauser);
};
