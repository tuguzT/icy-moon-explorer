#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Item/JuicyEquipmentRegistryGroup.h"
#include "JuicySaveGameItem.h"
#include "JuicySaveGameItemEquipment.h"
#include "JuicySaveGameItemInventory.h"
#include "JuicySaveGameNestedItems.h"
#include "Components/AGR_InventoryManager.h"
#include "Item/JuicyEquipmentManager.h"

#include "JuicySaveGame.generated.h"

UCLASS()
class ICYMOONEXPLORER_API UJuicySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SaveInventoryAndEquipment(UAGR_InventoryManager* InventoryManager,
	                               UJuicyEquipmentManager* EquipmentManager);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RestoreInventoryAndEquipment(UAGR_InventoryManager* InventoryManager,
	                                  UJuicyEquipmentManager* EquipmentManager) const;

protected:
	/**
	 * Array of all the items contained in inventory or equipment.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FJuicySaveGameItem> Items;

	/**
	 * Array of the items contained in inventory.
	 * Each item contains an index to the array of all the items.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FJuicySaveGameItemInventory> Inventory;

	/**
	 * Array of the items contained in equipment list.
	 * Each item contains an index to the array of all the items.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FJuicySaveGameItemEquipment> EquipmentList;

	/**
	 * Map of the items contained in equipment registry.
	 * Each value contains an array of indices to the array of all the items.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FJuicyEquipmentRegistryGroup, FJuicySaveGameNestedItems> EquipmentRegistry;
};
