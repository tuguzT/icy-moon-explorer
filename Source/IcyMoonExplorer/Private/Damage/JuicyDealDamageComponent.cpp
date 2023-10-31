#include "Damage/JuicyDealDamageComponent.h"

#include "Kismet/GameplayStatics.h"

UJuicyDealDamageComponent::UJuicyDealDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
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
	UGameplayStatics::ApplyDamage(DamagedActor, DamageToDeal.Damage, GetDamageInstigator(),
	                              GetDamageDealer(), DamageToDeal.DamageTypeClass);
}

void UJuicyDealDamageComponent::DealPointDamage(const FJuicyDealDamage& DamageToDeal,
                                                AActor* const DamagedActor,
                                                const FVector& HitFromDirection,
                                                const FHitResult& HitInfo) const
{
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageToDeal.Damage, HitFromDirection, HitInfo,
	                                   GetDamageInstigator(), GetDamageDealer(), DamageToDeal.DamageTypeClass);
}

bool UJuicyDealDamageComponent::DealRadialDamage(const FJuicyDealDamage& DamageToDeal,
                                                 const FVector& Origin, const float DamageRadius,
                                                 const TArray<AActor*>& IgnoreActors,
                                                 const bool bDoFullDamage,
                                                 const ECollisionChannel DamagePreventionChannel) const
{
	return UGameplayStatics::ApplyRadialDamage(this, DamageToDeal.Damage, Origin, DamageRadius,
	                                           DamageToDeal.DamageTypeClass, IgnoreActors, GetDamageDealer(),
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
	return UGameplayStatics::ApplyRadialDamageWithFalloff(this, DamageToDeal.Damage, MinimumDamage, Origin,
	                                                      DamageInnerRadius, DamageOuterRadius, DamageFalloff,
	                                                      DamageToDeal.DamageTypeClass, IgnoreActors, GetDamageDealer(),
	                                                      GetDamageInstigator(), DamagePreventionChannel);
}
