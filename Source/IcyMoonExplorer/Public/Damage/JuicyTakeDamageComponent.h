#pragma once

#include "Components/ActorComponent.h"
#include "JuicyDamageResistance.h"
#include "JuicyTakeDamageComponent.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryTakingAnyDamageSignature,
                                            FJuicyTakeDamage, DamageToTake);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnTryTakingPointDamageSignature,
                                              FJuicyTakeDamage, DamageToTake,
                                              FVector, HitLocation,
                                              UPrimitiveComponent*, FHitComponent,
                                              FName, BoneName,
                                              FVector, ShotFromDirection);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTryTakingRadialDamageSignature,
                                               FJuicyTakeDamage, DamageToTake,
                                               FVector, Origin,
                                               const FHitResult&, HitInfo);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTakenSignature,
                                            float, DamageTaken);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartTakeDamageCooldownSignature);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTakeDamageCooldownSignature);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReviveSignature);

UCLASS(Blueprintable, ClassGroup=(Damage), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyTakeDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnTryTakingAnyDamageSignature OnTryTakingAnyDamage;

	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnTryTakingPointDamageSignature OnTryTakingPointDamage;

	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnTryTakingRadialDamageSignature OnTryTakingRadialDamage;

	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnDamageTakenSignature OnDamageTaken;

	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnStartTakeDamageCooldownSignature OnStartTakeDamageCooldown;

	UPROPERTY(Category="Components|Take Damage", BlueprintAssignable)
	FOnEndTakeDamageCooldownSignature OnEndTakeDamageCooldown;

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

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	bool CanTakeDamage() const;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	bool IsTakeDamageCooldown() const;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	float GetSafeHealthPercentage() const;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage")
	float GetUnsafeHealthPercentage() const;

	UFUNCTION(BlueprintPure, Category="Components|Take Damage|Revive")
	float GetReviveHealth() const;

	UFUNCTION(BlueprintCallable, Category="Components|Take Damage|Revive")
	void SetReviveHealth(float NewReviveHealth);

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

public:
	UPROPERTY(Category="Take Damage (General Settings)", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float TakeDamageCooldown;

	UPROPERTY(Category="Take Damage (General Settings)", EditAnywhere, BlueprintReadWrite)
	uint8 bCanTakeDamageFromSelf : 1;

	UPROPERTY(Category="Take Damage: Resistance", EditAnywhere, BlueprintReadWrite)
	TArray<FJuicyDamageResistance> DamageResistances;

	UPROPERTY(Category="Take Damage: Resistance", EditAnywhere, BlueprintReadWrite)
	uint8 bProcessResistancesAutomatically : 1;

	UPROPERTY(Category="Take Damage: Revive", EditAnywhere, BlueprintReadWrite)
	uint8 bCanEverRevive : 1;

protected:
	UPROPERTY(Category="Take Damage: Revive", EditAnywhere,
		BlueprintGetter=GetReviveHealth, BlueprintSetter=SetReviveHealth,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float ReviveHealth;

private:
	FTimerHandle TimerHandleForTakeDamageCooldown;

	UFUNCTION()
	void OnTakeAnyDamageDelegatedFromOwner(AActor* DamagedActor, float Damage,
	                                       const UDamageType* DamageType,
	                                       AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION()
	void OnTakePointDamageDelegatedFromOwner(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	                                         FVector HitLocation,
	                                         UPrimitiveComponent* FHitComponent, FName BoneName,
	                                         FVector ShotFromDirection,
	                                         const UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION()
	void OnTakeRadialDamageDelegatedFromOwner(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                                          FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy,
	                                          AActor* DamageCauser);
	bool CanTakeDamageDelegatedFromOwner(AActor* DamagedActor, AActor* DamageCauser) const;

	void SetHealthRaw(float NewHealth);
	void ProcessResistancesAutomatically(FJuicyTakeDamage& DamageToTake) const;

	void StartTakeDamageCooldown();
	void EndTakeDamageCooldown();
};
