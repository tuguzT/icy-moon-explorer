#pragma once

#include "CoreMinimal.h"
#include "NestedActors.h"
#include "Components/AGR_EquipmentManager.h"
#include "JuicyEquipmentManager.generated.h"

UCLASS(ClassGroup=("AGR"), meta=(BlueprintSpawnableComponent))
class ICYMOONEXPLORER_API UJuicyEquipmentManager : public UAGR_EquipmentManager
{
	GENERATED_BODY()

public:
	UJuicyEquipmentManager();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Has Items") bool
	GetAllItemsFromRegistry(
		UPARAM(DisplayName="Items") TArray<AActor*>& OutItems) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Has Items") bool
	GetAllItemsFromRegistryByTag(
		FGameplayTag Tag,
		UPARAM(DisplayName="Items") TArray<AActor*>& OutItems) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Has Item") bool
	GetItemFromRegistryByTag(
		int32 Index,
		FGameplayTag Tag,
		UPARAM(DisplayName="Item") AActor*& OutItem) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Is Empty") bool
	IsRegistryEmpty() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR")
	UPARAM(DisplayName="Found") bool
	FindItemInRegistry(
		AActor* Item,
		UPARAM(DisplayName="Index") int32& OutIndex,
		UPARAM(DisplayName="Tag") FGameplayTag& OutTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR",
		meta=(KeyWords="contains"))
	UPARAM(DisplayName="Contains") bool
	IsItemRegistered(AActor* Item) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="AGR",
		meta=(KeyWords="contains"))
	UPARAM(DisplayName="Contains") bool
	IsItemRegisteredByTag(int32 Index, FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="add"))
	UPARAM(DisplayName="Success") bool
	RegisterItem(
		AActor* Item,
		UPARAM(DisplayName="Index") int32& OutIndex,
		UPARAM(DisplayName="Tag") FGameplayTag& OutTag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="insert"))
	UPARAM(DisplayName="Success") bool
	RegisterItemByIndex(
		AActor* Item,
		int32 Index,
		UPARAM(DisplayName="Tag") FGameplayTag& OutTag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="remove"))
	UPARAM(DisplayName="Success") bool
	UnregisterItem(
		AActor* Item,
		UPARAM(DisplayName="Tag") FGameplayTag& OutTag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR",
		meta=(KeyWords="remove at"))
	UPARAM(DisplayName="Success") bool
	UnregisterItemByTag(
		int32 Index,
		FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR")
	UPARAM(DisplayName = "Success") bool EquipItemFromRegistryInSlot(
		const FName Slot,
		AActor* Item,
		UPARAM(DisplayName = "PreviousItem") AActor*& OutPreviousItem,
		UPARAM(DisplayName = "NewItem") AActor*& OutNewItem);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category="AGR")
	UPARAM(DisplayName = "Success") bool EquipItemFromRegistryInSlotByTag(
		const FName Slot,
		int32 Index,
		FGameplayTag Tag,
		UPARAM(DisplayName = "PreviousItem") AActor*& OutPreviousItem,
		UPARAM(DisplayName = "NewItem") AActor*& OutNewItem);

protected:
	UPROPERTY(Category="AGR|Game Play", EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FNestedActors> EquipmentRegistry;
};
