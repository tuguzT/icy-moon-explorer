#pragma once

#include "CoreMinimal.h"
#include "Components/AGR_EquipmentManager.h"
#include "JuicyEquipmentRegistryGroup.h"
#include "NestedActors.h"

#include "JuicyEquipmentManager.generated.h"

UCLASS(ClassGroup=("AGR"), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyEquipmentManager : public UAGR_EquipmentManager
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="AGR|Game Play", EditAnywhere, BlueprintReadWrite)
	TMap<FJuicyEquipmentRegistryGroup, FNestedActors> EquipmentRegistry;

	UJuicyEquipmentManager();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	TMap<FJuicyEquipmentRegistryGroup, FNestedActors> GetEquipmentRegistry() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Has Items") bool
	GetAllItemsFromRegistry(
		UPARAM(DisplayName="Items") TArray<AActor*>& OutItems) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Has Items") bool
	GetAllItemsOfGroupFromRegistry(
		FJuicyEquipmentRegistryGroup Group,
		UPARAM(DisplayName="Items") TArray<AActor*>& OutItems) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Has Item") bool
	GetItemOfGroupFromRegistry(
		FJuicyEquipmentRegistryGroup Group,
		int32 Index,
		UPARAM(DisplayName="Item") AActor*& OutItem) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Is Empty") bool
	IsRegistryEmpty() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Found") bool
	FindItemInRegistry(
		AActor* Item,
		FGameplayTagContainer GroupLabels,
		UPARAM(DisplayName="Group") FJuicyEquipmentRegistryGroup& OutGroup,
		UPARAM(DisplayName="Index") int32& OutIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR",
		meta=(KeyWords="contains"))
	UPARAM(DisplayName="Contains") bool
	IsItemRegistered(
		AActor* Item,
		FGameplayTagContainer GroupLabels) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR",
		meta=(KeyWords="contains"))
	UPARAM(DisplayName="Contains") bool
	IsItemOfGroupRegistered(
		FJuicyEquipmentRegistryGroup Group,
		int32 Index) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR")
	void SetupDefineRegistry(
		const TMap<FJuicyEquipmentRegistryGroup, FNestedActors>& Registry);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="add"))
	UPARAM(DisplayName="Success") bool
	RegisterItem(
		AActor* Item,
		FGameplayTagContainer GroupLabels,
		UPARAM(DisplayName="Group") FJuicyEquipmentRegistryGroup& OutGroup,
		UPARAM(DisplayName="Index") int32& OutIndex);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="insert"))
	UPARAM(DisplayName="Success") bool
	RegisterItemByIndex(
		AActor* Item,
		FGameplayTagContainer GroupLabels,
		int32 Index,
		UPARAM(DisplayName="Group") FJuicyEquipmentRegistryGroup& OutGroup);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="remove"))
	UPARAM(DisplayName="Success") bool
	UnregisterItem(
		AActor* Item,
		FGameplayTagContainer GroupLabels,
		UPARAM(DisplayName="Group") FJuicyEquipmentRegistryGroup& OutGroup);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="remove at"))
	UPARAM(DisplayName="Success") bool
	UnregisterItemOfGroup(
		FJuicyEquipmentRegistryGroup Group,
		int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR")
	UPARAM(DisplayName = "Success") bool
	EquipItemFromRegistryInSlot(
		const FName Slot,
		AActor* Item,
		FGameplayTagContainer GroupLabels,
		UPARAM(DisplayName = "PreviousItem") AActor*& OutPreviousItem,
		UPARAM(DisplayName = "NewItem") AActor*& OutNewItem);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR")
	UPARAM(DisplayName = "Success") bool
	EquipItemOfGroupFromRegistryInSlot(
		const FName Slot,
		FJuicyEquipmentRegistryGroup Group,
		int32 Index,
		UPARAM(DisplayName = "PreviousItem") AActor*& OutPreviousItem,
		UPARAM(DisplayName = "NewItem") AActor*& OutNewItem);
};
