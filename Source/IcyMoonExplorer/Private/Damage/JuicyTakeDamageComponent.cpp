#include "Damage/JuicyTakeDamageComponent.h"

#include "Kismet/KismetMathLibrary.h"

UJuicyTakeDamageComponent::UJuicyTakeDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;

	MaxHealth = 100.0f;
	Health = MaxHealth;
	TakeDamageCooldown = 0.5f;
	ReviveHealth = MaxHealth;
	bCanEverRevive = false;
}

void UJuicyTakeDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	FTakeAnyDamageSignature& DamageDelegate = GetOwner()->OnTakeAnyDamage;
	DamageDelegate.AddUniqueDynamic(this, &UJuicyTakeDamageComponent::OnTakeAnyDamage_OwnerDelegate);
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
	const FTimerManager& TimerManager = GetOwner()->GetWorldTimerManager();
	const bool IsInCooldown = TimerManager.IsTimerActive(TimerHandleForTakeDamageCooldown);
	return !IsDead() && !IsInCooldown;
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

// ReSharper disable once CppParameterMayBeConstPtrOrRef
// ReSharper disable once CppMemberFunctionMayBeConst
void UJuicyTakeDamageComponent::OnTakeAnyDamage_OwnerDelegate(AActor* const DamagedActor,
                                                              const float Damage,
                                                              const UDamageType* const DamageType,
                                                              AController* const InstigatedBy,
                                                              AActor* const DamageCauser)
{
	if (GetOwner() != DamagedActor)
	{
		return;
	}

	OnTryTakingDamage.Broadcast(Damage);
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
