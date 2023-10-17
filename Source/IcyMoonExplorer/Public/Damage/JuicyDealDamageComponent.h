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
	void DealDamage(AActor* ActorToDamage, float Damage) const;
};
