#pragma once

#include "Components/ActorComponent.h"
#include "JuicyDealDamageComponent.generated.h"

UCLASS(ClassGroup=(Damage), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyDealDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit UJuicyDealDamageComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage")
	void DealDamage(AActor* DamagedActor, float Damage,
	                TSubclassOf<UDamageType> DamageTypeClass) const;

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage")
	void DealPointDamage(AActor* DamagedActor, float Damage,
	                     TSubclassOf<UDamageType> DamageTypeClass,
	                     const FVector& HitFromDirection, const FHitResult& HitInfo) const;

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage",
		meta=(WorldContext="WorldContextObject"))
	bool DealRadialDamage(float Damage, TSubclassOf<UDamageType> DamageTypeClass,
	                      const UObject* WorldContextObject,
	                      const FVector& Origin, float DamageRadius,
	                      const TArray<AActor*>& IgnoreActors, bool bDoFullDamage = false,
	                      ECollisionChannel DamagePreventionChannel = ECC_Visibility) const;

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage",
		meta=(WorldContext="WorldContextObject"))
	bool DealRadialDamageWithFalloff(float Damage, TSubclassOf<UDamageType> DamageTypeClass,
	                                 const UObject* WorldContextObject,
	                                 float MinimumDamage, const FVector& Origin,
	                                 float DamageInnerRadius, float DamageOuterRadius,
	                                 float DamageFalloff, const TArray<AActor*>& IgnoreActors,
	                                 ECollisionChannel DamagePreventionChannel = ECC_Visibility) const;
};
