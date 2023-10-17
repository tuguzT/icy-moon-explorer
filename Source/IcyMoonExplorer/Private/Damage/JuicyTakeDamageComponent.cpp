#include "Damage/JuicyTakeDamageComponent.h"

UJuicyTakeDamageComponent::UJuicyTakeDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;

	MaxHealth = 100.0f;
	Health = MaxHealth;
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
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
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
