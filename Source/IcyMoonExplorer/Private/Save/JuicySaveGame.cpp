#include "Save/JuicySaveGame.h"

#include "Data/AGRLibrary.h"

void UJuicySaveGame::SaveInventoryAndEquipment_Implementation(UAGR_InventoryManager* const InventoryManager,
                                                              UJuicyEquipmentManager* const EquipmentManager)
{
	if (!IsValid(InventoryManager) || !IsValid(EquipmentManager))
	{
		return;
	}

	auto AddItemToEquipmentRegistry = [&](const AActor* ItemActor, const int32 ItemIndex)
	{
		for (const auto& RegistryItem : EquipmentManager->EquipmentRegistry)
		{
			const FJuicyEquipmentRegistryGroup& Group = RegistryItem.Key;
			if (const TArray<AActor*>& GroupActors = RegistryItem.Value.Actors;
				!GroupActors.Contains(ItemActor))
			{
				continue;
			}

			FJuicySaveGameNestedItems& NestedItems = EquipmentRegistry.FindOrAdd(Group);
			const FJuicySaveGameItemInventory InventoryItem{ItemIndex};
			NestedItems.ItemIndices.Add(InventoryItem);
		}
	};

	Items.Reset();
	Inventory.Reset();
	EquipmentRegistry.Reset();
	for (const AActor* ItemActor : InventoryManager->GetAllItems())
	{
		const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
		if (!IsValid(ItemComponent))
		{
			continue;
		}

		const FJuicySaveGameItem Item{ItemActor->GetClass(), ItemComponent->CurrentStack};
		const int32 ItemIndex = Items.Add(Item);

		const FJuicySaveGameItemInventory InventoryItem{ItemIndex};
		Inventory.Add(InventoryItem);

		AddItemToEquipmentRegistry(ItemActor, ItemIndex);
	}

	EquipmentList.Reset();
	// ReSharper disable once CppUseStructuredBinding
	for (const FEquipment& ListItem : EquipmentManager->EquipmentList)
	{
		const AActor* ItemActor = ListItem.ItemActor;
		const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
		if (!IsValid(ItemComponent))
		{
			continue;
		}

		const FJuicySaveGameItem Item{ItemActor->GetClass(), ItemComponent->CurrentStack};
		const int32 ItemIndex = Items.Add(Item);

		const FJuicySaveGameItemEquipment EquipmentItem{ItemIndex, ListItem.Id, ListItem.AcceptableSlots};
		EquipmentList.Add(EquipmentItem);

		AddItemToEquipmentRegistry(ItemActor, ItemIndex);
	}
}

void UJuicySaveGame::RestoreInventoryAndEquipment_Implementation(UAGR_InventoryManager* const InventoryManager,
                                                                 UJuicyEquipmentManager* const EquipmentManager) const
{
	if (!IsValid(InventoryManager) || !IsValid(EquipmentManager))
	{
		return;
	}

	UWorld* World = InventoryManager->GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	for (AActor* ItemActor : InventoryManager->GetAllItems())
	{
		World->DestroyActor(ItemActor);
	}

	// Used to retrieve error message from the plugin, but I don't use it actually
	FText OutNote;

	TArray<AActor*> ItemActors;
	for (const FJuicySaveGameItem& Item : Items)
	{
		verify(InventoryManager->AddItemsOfClass(Item.Class, Item.CurrentStack, OutNote));

		TArray<AActor*> ItemActorsOfClass;
		verify(InventoryManager->GetAllItemsOfClass(Item.Class, ItemActorsOfClass));
		for (AActor* ItemActor : ItemActorsOfClass)
		{
			if (ItemActors.Contains(ItemActor))
			{
				continue;
			}
			const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(ItemActor);
			verify(ItemComponent);
			if (Item.CurrentStack != ItemComponent->CurrentStack)
			{
				continue;
			}
			ItemActors.Add(ItemActor);
		}
	}

	{
		TMap<FJuicyEquipmentRegistryGroup, FNestedActors> EquipmentManagerRegistry;
		TArray<FJuicyEquipmentRegistryGroup> Keys;
		EquipmentRegistry.GetKeys(Keys);
		for (const FJuicyEquipmentRegistryGroup& Key : Keys)
		{
			EquipmentManagerRegistry.Add(Key, FNestedActors{});
		}
		EquipmentManager->SetupDefineRegistry(EquipmentManagerRegistry);
	}
	for (const auto& ItemRegistry : EquipmentRegistry)
	{
		const FJuicyEquipmentRegistryGroup& Group = ItemRegistry.Key;
		for (const TArray<FJuicySaveGameItemInventory>& ItemIndices = ItemRegistry.Value.ItemIndices;
		     const FJuicySaveGameItemInventory& ItemInventory : ItemIndices)
		{
			// const FJuicySaveGameItem& Item = Items[ItemInventory.ItemIndex];
			// verify(InventoryManager->AddItemsOfClass(Item.Class, Item.CurrentStack, OutNote));
			//
			// TArray<AActor*> ItemActors;
			// verify(InventoryManager->GetAllItemsOfClass(Item.Class, ItemActors));
			// AActor* ItemActor = ItemActors[0];
			AActor* ItemActor = ItemActors[ItemInventory.ItemIndex];

			int32 OutIndex;
			FJuicyEquipmentRegistryGroup OutGroup;
			const bool ItemRegistered = EquipmentManager->RegisterItem(ItemActor, Group.Labels, OutGroup, OutIndex);
			verify(ItemRegistered && OutGroup == Group);
		}
	}

	{
		TArray<FEquipment> EquipmentManagerList;
		for (const FJuicySaveGameItemEquipment& ItemEquipment : EquipmentList)
		{
			const FEquipment ListItem{ItemEquipment.SlotId, ItemEquipment.AcceptableSlots, nullptr};
			EquipmentManagerList.Add(ListItem);
		}
		EquipmentManager->SetupDefineSlots(EquipmentManagerList);
	}
	for (const FJuicySaveGameItemEquipment& ItemEquipment : EquipmentList)
	{
		// const FJuicySaveGameItem& Item = Items[ItemEquipment.ItemIndex];
		// verify(InventoryManager->AddItemsOfClass(Item.Class, Item.CurrentStack, OutNote));
		//
		// TArray<AActor*> ItemActors;
		// verify(InventoryManager->GetAllItemsOfClass(Item.Class, ItemActors));
		// AActor* ItemActor = ItemActors[0];
		AActor* ItemActor = ItemActors[ItemEquipment.ItemIndex];

		AActor *OutPreviousItem, *OutNewItem;
		const bool ItemEquipped = EquipmentManager->EquipItemInSlot(ItemEquipment.SlotId, ItemActor,
		                                                            OutPreviousItem, OutNewItem);
		verify(ItemEquipped);
	}

	for (const FJuicySaveGameItemInventory& ItemInventory : Inventory)
	{
		// const FJuicySaveGameItem& Item = Items[ItemInventory.ItemIndex];
		// verify(InventoryManager->AddItemsOfClass(Item.Class, Item.CurrentStack, OutNote));
		//
		// const bool HasEnoughItems = InventoryManager->HasEnoughItems(Item.Class, Item.CurrentStack, OutNote);
		// verify(HasEnoughItems);
	}
}
