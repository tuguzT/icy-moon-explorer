#include "Damage/JuicyDealDamageComponent.h"

#include "Damage/JuicyDamageLibrary.h"
#include "Kismet/GameplayStatics.h"

UJuicyDealDamageComponent::UJuicyDealDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;

	bCanDealDamageToSelf = true;
	CanDealDamageByTeamAttitude = ETeamAttitude::Neutral;
}

AActor* UJuicyDealDamageComponent::GetDamageDealer() const
{
	return GetOwner();
}

AController* UJuicyDealDamageComponent::GetDamageInstigator() const
{
	const AActor* DamageDealer = GetDamageDealer();
	return DamageDealer ? DamageDealer->GetInstigatorController() : nullptr;
}

void UJuicyDealDamageComponent::DealDamage(const FJuicyDealDamage& DamageToDeal,
                                           AActor* const DamagedActor) const
{
	if (!CanDealDamageTo(DamagedActor))
	{
		return;
	}

	UGameplayStatics::ApplyDamage(DamagedActor, DamageToDeal.Damage, GetDamageInstigator(),
	                              GetDamageDealer(), DamageToDeal.DamageTypeClass);
}

void UJuicyDealDamageComponent::DealPointDamage(const FJuicyDealDamage& DamageToDeal,
                                                AActor* const DamagedActor,
                                                const FVector& HitFromDirection,
                                                const FHitResult& HitInfo) const
{
	if (!CanDealDamageTo(DamagedActor))
	{
		return;
	}

	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageToDeal.Damage, HitFromDirection, HitInfo,
	                                   GetDamageInstigator(), GetDamageDealer(), DamageToDeal.DamageTypeClass);
}

bool UJuicyDealDamageComponent::DealRadialDamage(const FJuicyDealDamage& DamageToDeal,
                                                 const FVector& Origin, const float DamageRadius,
                                                 const TArray<AActor*>& IgnoreActors,
                                                 const bool bDoFullDamage,
                                                 const ECollisionChannel DamagePreventionChannel) const
{
	TArray<AActor*> IgnoreActorsWithSelf{};
	AActor* const DamageDealer = GetDamageDealer();

	if (!bCanDealDamageToSelf && DamageDealer)
	{
		IgnoreActorsWithSelf = IgnoreActors;
		IgnoreActorsWithSelf.Add(DamageDealer);
	}

	const auto& ActorsToIgnore = IgnoreActorsWithSelf.IsEmpty() ? IgnoreActors : IgnoreActorsWithSelf;
	return UGameplayStatics::ApplyRadialDamage(this, DamageToDeal.Damage, Origin, DamageRadius,
	                                           DamageToDeal.DamageTypeClass, ActorsToIgnore, DamageDealer,
	                                           GetDamageInstigator(),
	                                           bDoFullDamage, DamagePreventionChannel);
}

bool UJuicyDealDamageComponent::DealRadialDamageWithFalloff(const FJuicyDealDamage& DamageToDeal,
                                                            const float MinimumDamage,
                                                            const FVector& Origin,
                                                            const float DamageInnerRadius,
                                                            const float DamageOuterRadius,
                                                            const float DamageFalloff,
                                                            const TArray<AActor*>& IgnoreActors,
                                                            const ECollisionChannel DamagePreventionChannel) const
{
	TArray<AActor*> IgnoreActorsWithSelf{};
	AActor* const DamageDealer = GetDamageDealer();

	if (!bCanDealDamageToSelf && DamageDealer)
	{
		IgnoreActorsWithSelf = IgnoreActors;
		IgnoreActorsWithSelf.Add(DamageDealer);
	}

	const auto& ActorsToIgnore = IgnoreActorsWithSelf.IsEmpty() ? IgnoreActors : IgnoreActorsWithSelf;
	return UGameplayStatics::ApplyRadialDamageWithFalloff(this, DamageToDeal.Damage, MinimumDamage, Origin,
	                                                      DamageInnerRadius, DamageOuterRadius, DamageFalloff,
	                                                      DamageToDeal.DamageTypeClass, ActorsToIgnore, DamageDealer,
	                                                      GetDamageInstigator(), DamagePreventionChannel);
}

bool UJuicyDealDamageComponent::CanDealDamageTo(const AActor* const DamagedActor) const
{
	const AActor* Dealer = GetDamageDealer();
	const AActor* Target = DamagedActor;
	const AController* DealerInstigator = GetDamageInstigator();
	const AController* TargetInstigator = IsValid(Target) ? Target->GetInstigatorController() : nullptr;

	return UJuicyDamageLibrary::CanDealDamageTo(Dealer, Target, DealerInstigator, TargetInstigator,
	                                            bCanDealDamageToSelf, CanDealDamageByTeamAttitude);
}
