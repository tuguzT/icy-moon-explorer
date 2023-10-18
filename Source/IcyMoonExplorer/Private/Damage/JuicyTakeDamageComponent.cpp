#include "Damage/JuicyTakeDamageComponent.h"

#include "Kismet/KismetMathLibrary.h"

UJuicyTakeDamageComponent::UJuicyTakeDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;

	MaxHealth = 100.0f;
	Health = MaxHealth;
	TakeDamageCooldown = 0.5f;
	bCanTakeDamageFromSelf = true;
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

FORCEINLINE float UJuicyTakeDamageComponent::GetHealth() const
{
	return Health;
}

void UJuicyTakeDamageComponent::SetHealth(const float NewHealth)
{
	if (!CanTakeDamage())
	{
		return;
	}

	const float OldHealth = Health;
	SetHealthRaw(NewHealth);
	if (OldHealth == Health)
	{
		return;
	}

	const float DamageTaken = OldHealth - Health;
	OnDamageTaken.Broadcast(DamageTaken);
	StartTakeDamageCooldown();

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

FORCEINLINE float UJuicyTakeDamageComponent::GetMaxHealth() const
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
}

bool UJuicyTakeDamageComponent::CanTakeDamage() const
{
	return !IsDead() && !IsTakeDamageCooldown();
}

bool UJuicyTakeDamageComponent::IsTakeDamageCooldown() const
{
	const FTimerManager& TimerManager = GetOwner()->GetWorldTimerManager();
	return TimerManager.IsTimerActive(TimerHandleForTakeDamageCooldown);
}

FORCEINLINE float UJuicyTakeDamageComponent::GetReviveHealth() const
{
	return ReviveHealth;
}

void UJuicyTakeDamageComponent::SetReviveHealth(const float NewReviveHealth)
{
	ReviveHealth = FMath::Clamp(NewReviveHealth, 0.0f, MaxHealth);
}

FORCEINLINE bool UJuicyTakeDamageComponent::IsDead() const
{
	return Health <= 0.0f;
}

FORCEINLINE float UJuicyTakeDamageComponent::GetSafeHealthPercentage() const
{
	return UKismetMathLibrary::SafeDivide(Health, MaxHealth);
}

FORCEINLINE float UJuicyTakeDamageComponent::GetUnsafeHealthPercentage() const
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
	if (CanTakeDamageDelegatedFromOwner(DamagedActor, DamageCauser))
	{
		OnTryTakingAnyDamage.Broadcast(Damage, DamageType, InstigatedBy, DamageCauser);
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
	if (CanTakeDamageDelegatedFromOwner(DamagedActor, DamageCauser))
	{
		OnTryTakingPointDamage.Broadcast(Damage, DamageType, InstigatedBy, DamageCauser,
		                                 HitLocation, FHitComponent, BoneName, ShotFromDirection);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UJuicyTakeDamageComponent::OnTakeRadialDamageDelegatedFromOwner(
	AActor* const DamagedActor,
	const float Damage,
	const UDamageType* const DamageType,
	// ReSharper disable once CppPassValueParameterByConstReference
	const FVector Origin,
	const FHitResult& HitInfo,
	AController* const InstigatedBy,
	AActor* const DamageCauser)
{
	if (CanTakeDamageDelegatedFromOwner(DamagedActor, DamageCauser))
	{
		OnTryTakingRadialDamage.Broadcast(Damage, DamageType, InstigatedBy, DamageCauser,
		                                  Origin, HitInfo);
	}
}

bool UJuicyTakeDamageComponent::CanTakeDamageDelegatedFromOwner(
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	AActor* const DamagedActor,
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	AActor* const DamageCauser) const
{
	const AActor* Owner = GetOwner();
	if (Owner != DamagedActor)
	{
		return false;
	}
	if (!bCanTakeDamageFromSelf && Owner == DamageCauser)
	{
		return false;
	}
	return true;
}

void UJuicyTakeDamageComponent::SetHealthRaw(const float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
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
	FTimerManager& TimerManager = GetOwner()->GetWorldTimerManager();
	TimerManager.ClearTimer(TimerHandleForTakeDamageCooldown);
	OnEndTakeDamageCooldown.Broadcast();
}
