#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JuicyEnergyComponent.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyUsedSignature,
                                            float, EnergyUsed);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyRestoredSignature,
                                            float, EnergyRestored);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnergyDepletedSignature);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnergyRenewedSignature);

UCLASS(Blueprintable, ClassGroup=(Energy), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyEnergyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Components|Energy", BlueprintAssignable)
	FOnEnergyUsedSignature OnEnergyUsed;

	UPROPERTY(Category="Components|Energy", BlueprintAssignable)
	FOnEnergyRestoredSignature OnEnergyRestored;

	UPROPERTY(Category="Components|Energy", BlueprintAssignable)
	FOnEnergyDepletedSignature OnEnergyDepleted;

	UPROPERTY(Category="Components|Energy", BlueprintAssignable)
	FOnEnergyRenewedSignature OnEnergyRenewed;

	explicit UJuicyEnergyComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category="Components|Energy")
	float GetEnergy() const;

	UFUNCTION(BlueprintCallable, Category="Components|Energy")
	void SetEnergy(float NewEnergy);

	UFUNCTION(BlueprintPure, Category="Components|Energy")
	float GetMaxEnergy() const;

	UFUNCTION(BlueprintCallable, Category="Components|Energy")
	void SetMaxEnergy(float NewMaxEnergy);

	UFUNCTION(BlueprintPure, Category="Components|Energy")
	bool HasEnergy() const;

	UFUNCTION(BlueprintPure, Category="Components|Energy")
	bool IsFullEnergy() const;

	UFUNCTION(BlueprintPure, Category="Components|Energy")
	float GetSafeEnergyPercentage() const;

	UFUNCTION(BlueprintPure, Category="Components|Energy")
	float GetUnsafeEnergyPercentage() const;

protected:
	UPROPERTY(Category="Energy (General Settings)", EditAnywhere,
		BlueprintGetter=GetEnergy, BlueprintSetter=SetEnergy,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float Energy;

	UPROPERTY(Category="Energy (General Settings)", EditAnywhere,
		BlueprintGetter=GetMaxEnergy, BlueprintSetter=SetMaxEnergy,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points"))
	float MaxEnergy;

public:
	UPROPERTY(Category="Energy: Recovery", EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin="0", UIMin="0", ForceUnits="points/s"))
	float EnergyRecoverySpeed;

	UPROPERTY(Category="Energy: Recovery", EditAnywhere, BlueprintReadWrite)
	uint8 bCanEverRecoverEnergy : 1;

private:
	void SetEnergyRaw(float NewEnergy);
};
