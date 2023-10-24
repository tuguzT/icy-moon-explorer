#pragma once

#include "Components/ActorComponent.h"
#include "JuicyDealDamage.h"
#include "JuicyDealDamageComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Damage), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyDealDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit UJuicyDealDamageComponent(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Components|Deal Damage")
	AActor* GetDamageDealer() const;

	UFUNCTION(BlueprintCallable, Category="Components|Deal Damage")
	AController* GetDamageInstigator() const;

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage")
	void DealDamage(FJuicyDealDamage DamageToDeal, AActor* DamagedActor) const;

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage")
	void DealPointDamage(FJuicyDealDamage DamageToDeal, AActor* DamagedActor,
	                     const FVector& HitFromDirection, const FHitResult& HitInfo) const;

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage")
	bool DealRadialDamage(FJuicyDealDamage DamageToDeal, const FVector& Origin, float DamageRadius,
	                      const TArray<AActor*>& IgnoreActors, bool bDoFullDamage = false,
	                      ECollisionChannel DamagePreventionChannel = ECC_Visibility) const;

	UFUNCTION(BlueprintPure=false, Category="Components|Deal Damage")
	bool DealRadialDamageWithFalloff(FJuicyDealDamage DamageToDeal, float MinimumDamage, const FVector& Origin,
	                                 float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff,
	                                 const TArray<AActor*>& IgnoreActors,
	                                 ECollisionChannel DamagePreventionChannel = ECC_Visibility) const;
};
