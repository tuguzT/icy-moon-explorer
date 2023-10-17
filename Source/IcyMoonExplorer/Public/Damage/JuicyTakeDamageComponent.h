#pragma once

#include "Components/ActorComponent.h"
#include "JuicyTakeDamageComponent.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamageSignature, float, Damage);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReviveSignature);

UCLASS(ClassGroup=(Damage), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyTakeDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnTakeDamageSignature OnTakeDamage;

	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnDeathSignature OnDeath;

	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnReviveSignature OnRevive;

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

	UFUNCTION(BlueprintPure, Category="Components|Take Damage|Revive")
	float GetReviveHealth() const;

	UFUNCTION(BlueprintCallable, Category="Components|Take Damage|Revive")
	void SetReviveHealth(float NewReviveHealth);

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	float GetSafeHealthPercentage() const;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	float GetUnsafeHealthPercentage() const;

	UFUNCTION(BlueprintCallable, Category="Components|Take Damage|Revive")
	void Revive();

	UFUNCTION(BlueprintPure, Category="Components|Take Damage|Revive")
	bool CanRevive() const;

protected:
	UPROPERTY(Category="Take Damage (General Settings)", EditAnywhere,
		BlueprintGetter=GetHealth, BlueprintSetter=SetHealth,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float Health;

	UPROPERTY(Category="Take Damage (General Settings)", EditAnywhere,
		BlueprintGetter=GetMaxHealth, BlueprintSetter=SetMaxHealth,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float MaxHealth;

	UPROPERTY(Category="Take Damage: Revive", EditAnywhere, BlueprintReadWrite)
	bool bCanEverRevive;

	UPROPERTY(Category="Take Damage: Revive", EditAnywhere,
		BlueprintGetter=GetReviveHealth, BlueprintSetter=SetReviveHealth,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float ReviveHealth;

private:
	UFUNCTION()
	void OnTakeAnyDamage_OwnerDelegate(AActor* DamagedActor, float Damage,
	                                   const UDamageType* DamageType,
	                                   AController* InstigatedBy, AActor* DamageCauser);

	void ForceSetHealth(float NewHealth);
};
