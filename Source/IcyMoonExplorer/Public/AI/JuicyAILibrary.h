#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "JuicyAILibrary.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(ETeamAttitude::Type, FAttitudeSolverSignature,
                                          FGenericTeamId, A,
                                          FGenericTeamId, B);

UCLASS(Abstract)
class ICYMOONEXPLORER_API UJuicyAILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="AI|Team")
	static ETeamAttitude::Type GetTeamAttitudeTowards(const AActor* Target, const AActor* Other);

	UFUNCTION(BlueprintCallable, Category="AI")
	static void UpdateSource(AActor* SourceActor);

	UFUNCTION(BlueprintCallable, Category="AI|Team")
	static void SetAttitudeSolver(const FAttitudeSolverSignature& Solver);

	UFUNCTION(BlueprintCallable, Category="AI|Team")
	static void ResetAttitudeSolver();
};
