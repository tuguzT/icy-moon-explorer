#include "Damage/JuicyDealDamageComponent.h"

#include "Kismet/GameplayStatics.h"

UJuicyDealDamageComponent::UJuicyDealDamageComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
}

void UJuicyDealDamageComponent::DealDamage(AActor* DamagedActor, const float Damage,
                                           const TSubclassOf<UDamageType> DamageTypeClass) const
{
	AActor* const DamageCauser = GetOwner();
	AController* const Instigator = DamageCauser->GetInstigatorController();
	UGameplayStatics::ApplyDamage(DamagedActor, Damage, Instigator, DamageCauser, DamageTypeClass);
}

void UJuicyDealDamageComponent::DealPointDamage(AActor* DamagedActor, const float Damage,
                                                const TSubclassOf<UDamageType> DamageTypeClass,
                                                const FVector& HitFromDirection, const FHitResult& HitInfo) const
{
	AActor* const DamageCauser = GetOwner();
	AController* const Instigator = DamageCauser->GetInstigatorController();
	UGameplayStatics::ApplyPointDamage(DamagedActor, Damage, HitFromDirection, HitInfo,
	                                   Instigator, DamageCauser, DamageTypeClass);
}

bool UJuicyDealDamageComponent::DealRadialDamage(const float Damage, const TSubclassOf<UDamageType> DamageTypeClass,
                                                 const UObject* const WorldContextObject,
                                                 const FVector& Origin, const float DamageRadius,
                                                 const TArray<AActor*>& IgnoreActors, const bool bDoFullDamage,
                                                 const ECollisionChannel DamagePreventionChannel) const
{
	AActor* const DamageCauser = GetOwner();
	AController* const Instigator = DamageCauser->GetInstigatorController();
	return UGameplayStatics::ApplyRadialDamage(WorldContextObject, Damage, Origin, DamageRadius,
	                                           DamageTypeClass, IgnoreActors, DamageCauser, Instigator, bDoFullDamage,
	                                           DamagePreventionChannel);
}

bool UJuicyDealDamageComponent::DealRadialDamageWithFalloff(const float Damage,
                                                            const TSubclassOf<UDamageType> DamageTypeClass,
                                                            const UObject* WorldContextObject,
                                                            const float MinimumDamage,
                                                            const FVector& Origin, const float DamageInnerRadius,
                                                            const float DamageOuterRadius, const float DamageFalloff,
                                                            const TArray<AActor*>& IgnoreActors,
                                                            const ECollisionChannel DamagePreventionChannel) const
{
	AActor* const DamageCauser = GetOwner();
	AController* const Instigator = DamageCauser->GetInstigatorController();
	return UGameplayStatics::ApplyRadialDamageWithFalloff(WorldContextObject, Damage, MinimumDamage, Origin,
	                                                      DamageInnerRadius, DamageOuterRadius, DamageFalloff,
	                                                      DamageTypeClass, IgnoreActors, DamageCauser, Instigator,
	                                                      DamagePreventionChannel);
}
