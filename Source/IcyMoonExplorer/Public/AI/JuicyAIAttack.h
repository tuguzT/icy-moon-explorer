#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JuicyAIAttack.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAttackEndSignature,
                                  bool, Success);

UINTERFACE(BlueprintType, Blueprintable, Category="AI", DisplayName="Juicy AI Attack")
class UJuicyAIAttack : public UInterface
{
	GENERATED_BODY()
};

class ICYMOONEXPLORER_API IJuicyAIAttack
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Attack")
	void Attack(AActor* ActorToAttack);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Attack", DisplayName="Bind Event to On Attack End")
	void BindEventToOnAttackEnd(const FOnAttackEndSignature& Event);
};
