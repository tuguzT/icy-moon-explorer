#include "Damage/JuicyTakeDamageComponent.h"

#include "Kismet/KismetMathLibrary.h"

UJuicyTakeDamageComponent::UJuicyTakeDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;

	MaxHealth = 100.0f;
	Health = MaxHealth;
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
	if (IsDead())
	{
		return;
	}

	ForceSetHealth(NewHealth);
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
	SetHealth(Health);
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

	ForceSetHealth(ReviveHealth);
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

	OnTakeDamage.Broadcast(Damage);
}

void UJuicyTakeDamageComponent::ForceSetHealth(const float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}
