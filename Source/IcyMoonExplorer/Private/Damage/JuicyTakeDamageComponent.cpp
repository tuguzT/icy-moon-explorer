﻿#include "Damage/JuicyTakeDamageComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Damage/JuicyDamageLibrary.h"

UJuicyTakeDamageComponent::UJuicyTakeDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;

	MaxHealth = 100.0f;
	Health = MaxHealth;
	TakeDamageCooldown = 0.5f;
	bCanTakeDamageFromSelf = true;
	CanTakeDamageByTeamAttitude = ETeamAttitude::Friendly;
	bProcessResistancesAutomatically = true;
	bCanEverRevive = false;
	ReviveHealth = MaxHealth;
}

void UJuicyTakeDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* const Owner = GetOwner();
	Owner->OnTakeAnyDamage.AddUniqueDynamic(this, &UJuicyTakeDamageComponent::OnTakeAnyDamageDelegatedFromOwner);
	Owner->OnTakePointDamage.AddUniqueDynamic(this, &UJuicyTakeDamageComponent::OnTakePointDamageDelegatedFromOwner);
	Owner->OnTakeRadialDamage.AddUniqueDynamic(this, &UJuicyTakeDamageComponent::OnTakeRadialDamageDelegatedFromOwner);
}

float UJuicyTakeDamageComponent::GetHealth() const
{
	return Health;
}

void UJuicyTakeDamageComponent::SetHealth(const float NewHealth)
{
	const bool bCanTakeDamage = NewHealth < Health && CanTakeDamage();
	const bool bCanRestoreHealth = NewHealth > Health && CanRestoreHealth();
	if (!bCanTakeDamage && !bCanRestoreHealth)
	{
		return;
	}

	const float OldHealth = Health;
	SetHealthRaw(NewHealth);
	if (const float DamageTaken = OldHealth - Health;
		DamageTaken > 0.0f)
	{
		OnDamageTaken.Broadcast(DamageTaken);
		StartTakeDamageCooldown();
	}
	else if (DamageTaken < 0.0f)
	{
		const float HealthRestored = -DamageTaken;
		OnHealthRestored.Broadcast(HealthRestored);
	}
	else
	{
		return;
	}

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

float UJuicyTakeDamageComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UJuicyTakeDamageComponent::SetMaxHealth(const float NewMaxHealth)
{
	if (NewMaxHealth < 0.0f)
	{
		return;
	}
	MaxHealth = NewMaxHealth;

	if (Health > MaxHealth)
	{
		// Forget old timer to be sure health will be changed
		// and cooldown events will be fired even if any cooldown exists
		TimerHandleForTakeDamageCooldown = FTimerHandle{};

		SetHealth(MaxHealth);
	}
}

bool UJuicyTakeDamageComponent::CanTakeDamage() const
{
	return !IsDead() && !IsTakeDamageCooldown();
}

bool UJuicyTakeDamageComponent::CanRestoreHealth() const
{
	return !IsDead();
}

bool UJuicyTakeDamageComponent::IsTakeDamageCooldown() const
{
	const FTimerManager& TimerManager = GetOwner()->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForTakeDamageCooldown);
}

float UJuicyTakeDamageComponent::GetReviveHealth() const
{
	return ReviveHealth;
}

void UJuicyTakeDamageComponent::SetReviveHealth(const float NewReviveHealth)
{
	ReviveHealth = FMath::Clamp(NewReviveHealth, 0.0f, MaxHealth);
}

bool UJuicyTakeDamageComponent::IsDead() const
{
	return Health <= 0.0f;
}

float UJuicyTakeDamageComponent::GetSafeHealthPercentage() const
{
	return UKismetMathLibrary::SafeDivide(Health, MaxHealth);
}

float UJuicyTakeDamageComponent::GetUnsafeHealthPercentage() const
{
	return Health / MaxHealth;
}

void UJuicyTakeDamageComponent::Revive()
{
	if (!CanRevive())
	{
		return;
	}

	SetHealthRaw(ReviveHealth);
	OnRevive.Broadcast();
}

bool UJuicyTakeDamageComponent::CanRevive() const
{
	return bCanEverRevive && IsDead();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UJuicyTakeDamageComponent::OnTakeAnyDamageDelegatedFromOwner(
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	AActor* const DamagedActor,
	const float Damage,
	const UDamageType* const DamageType,
	AController* const InstigatedBy,
	AActor* const DamageCauser)
{
	if (CanTakeDamageFrom(DamagedActor, InstigatedBy, DamageCauser))
	{
		auto DamageToTake = FJuicyTakeDamage{Damage, DamageType, DamageCauser, InstigatedBy};
		ProcessResistancesAutomatically(DamageToTake, DamagedActor);
		OnTryTakingAnyDamage.Broadcast(DamageToTake);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UJuicyTakeDamageComponent::OnTakePointDamageDelegatedFromOwner(
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	AActor* const DamagedActor,
	const float Damage,
	AController* const InstigatedBy,
	// ReSharper disable once CppPassValueParameterByConstReference
	const FVector HitLocation,
	UPrimitiveComponent* const FHitComponent,
	const FName BoneName,
	// ReSharper disable once CppPassValueParameterByConstReference
	const FVector ShotFromDirection,
	const UDamageType* const DamageType,
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	AActor* const DamageCauser)
{
	if (CanTakeDamageFrom(DamagedActor, InstigatedBy, DamageCauser))
	{
		auto DamageToTake = FJuicyTakeDamage{Damage, DamageType, DamageCauser, InstigatedBy};
		ProcessResistancesAutomatically(DamageToTake, DamagedActor);
		OnTryTakingPointDamage.Broadcast(DamageToTake, HitLocation, FHitComponent, BoneName, ShotFromDirection);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UJuicyTakeDamageComponent::OnTakeRadialDamageDelegatedFromOwner(
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	AActor* const DamagedActor,
	const float Damage,
	const UDamageType* const DamageType,
	// ReSharper disable once CppPassValueParameterByConstReference
	const FVector Origin,
	const FHitResult& HitInfo,
	AController* const InstigatedBy,
	AActor* const DamageCauser)
{
	if (CanTakeDamageFrom(DamagedActor, InstigatedBy, DamageCauser))
	{
		auto DamageToTake = FJuicyTakeDamage{Damage, DamageType, DamageCauser, InstigatedBy};
		ProcessResistancesAutomatically(DamageToTake, DamagedActor);
		OnTryTakingRadialDamage.Broadcast(DamageToTake, Origin, HitInfo);
	}
}

bool UJuicyTakeDamageComponent::CanTakeDamageFrom(
	const AActor* const DamagedActor,
	const AController* const InstigatedBy,
	const AActor* const DamageCauser) const
{
	if (GetOwner() != DamagedActor)
	{
		return false;
	}

	const AActor* Dealer = DamageCauser;
	const AActor* Target = DamagedActor;
	const AController* DealerInstigator = InstigatedBy;
	const AController* TargetInstigator = IsValid(Target) ? Target->GetInstigatorController() : nullptr;

	return UJuicyDamageLibrary::CanDealDamageTo(Dealer, Target, DealerInstigator, TargetInstigator,
	                                            bCanTakeDamageFromSelf, CanTakeDamageByTeamAttitude);
}

void UJuicyTakeDamageComponent::SetHealthRaw(const float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}

void UJuicyTakeDamageComponent::ProcessResistancesAutomatically(FJuicyTakeDamage& DamageToTake,
                                                                const AActor* const DamagedActor) const
{
	if (!bProcessResistancesAutomatically)
	{
		return;
	}

	DamageToTake.Damage = UJuicyDamageLibrary::ProcessResistances(DamageResistances, DamageToTake, DamagedActor);
}

void UJuicyTakeDamageComponent::StartTakeDamageCooldown()
{
	if (TakeDamageCooldown <= 0.0f)
	{
		return;
	}

	FTimerManager& TimerManager = GetOwner()->GetWorldTimerManager();
	TimerManager.SetTimer(TimerHandleForTakeDamageCooldown, this,
	                      &UJuicyTakeDamageComponent::EndTakeDamageCooldown,
	                      TakeDamageCooldown);
	OnStartTakeDamageCooldown.Broadcast();
}

void UJuicyTakeDamageComponent::EndTakeDamageCooldown()
{
	TimerHandleForTakeDamageCooldown = FTimerHandle{};
	OnEndTakeDamageCooldown.Broadcast();
}
