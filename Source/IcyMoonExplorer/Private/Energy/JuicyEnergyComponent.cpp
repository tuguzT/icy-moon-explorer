#include "Energy/JuicyEnergyComponent.h"

#include "Kismet/KismetMathLibrary.h"

UJuicyEnergyComponent::UJuicyEnergyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;

	Energy = 3.0f;
	MaxEnergy = 3.0f;
	bCanEverRecoverEnergy = true;
	EnergyRecoverySpeed = 1.0f;
}

void UJuicyEnergyComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanEverRecoverEnergy)
	{
		const float OldEnergy = Energy;
		const float NewEnergy = Energy + DeltaTime * EnergyRecoverySpeed;
		SetEnergyRaw(NewEnergy);

		if (Energy != OldEnergy && IsFullEnergy())
		{
			OnEnergyRenewed.Broadcast();
		}
	}
}

float UJuicyEnergyComponent::GetEnergy() const
{
	return Energy;
}

void UJuicyEnergyComponent::SetEnergy(const float NewEnergy)
{
	const float OldEnergy = Energy;
	SetEnergyRaw(NewEnergy);
	if (const float EnergyUsed = OldEnergy - Energy;
		EnergyUsed > 0.0f)
	{
		OnEnergyUsed.Broadcast(EnergyUsed);
	}
	else if (EnergyUsed < 0.0f)
	{
		const float EnergyRestored = -EnergyUsed;
		OnEnergyRestored.Broadcast(EnergyRestored);
	}
	else
	{
		return;
	}

	if (HasEnergy() && IsFullEnergy())
	{
		OnEnergyRenewed.Broadcast();
	}
	if (!HasEnergy())
	{
		OnEnergyDepleted.Broadcast();
	}
}

float UJuicyEnergyComponent::GetMaxEnergy() const
{
	return MaxEnergy;
}

void UJuicyEnergyComponent::SetMaxEnergy(const float NewMaxEnergy)
{
	if (NewMaxEnergy < 0.0f)
	{
		return;
	}
	MaxEnergy = NewMaxEnergy;

	if (Energy > MaxEnergy)
	{
		SetEnergy(MaxEnergy);
	}
}

bool UJuicyEnergyComponent::HasEnergy() const
{
	return Energy != 0.0f;
}

bool UJuicyEnergyComponent::IsFullEnergy() const
{
	return Energy == MaxEnergy;
}

float UJuicyEnergyComponent::GetSafeEnergyPercentage() const
{
	return UKismetMathLibrary::SafeDivide(Energy, MaxEnergy);
}

float UJuicyEnergyComponent::GetUnsafeEnergyPercentage() const
{
	return Energy / MaxEnergy;
}

void UJuicyEnergyComponent::SetEnergyRaw(const float NewEnergy)
{
	Energy = FMath::Clamp(NewEnergy, 0.0f, MaxEnergy);
}
