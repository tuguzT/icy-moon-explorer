#include "Item/JuicyEquipmentManager.h"

#include "Data/AGRLibrary.h"

UJuicyEquipmentManager::UJuicyEquipmentManager()
{
}

bool UJuicyEquipmentManager::GetAllItemsFromRegistry_Implementation(TArray<AActor*>& OutItems) const
{
	OutItems.Reset();

	for (const auto& Record : EquipmentRegistry)
	{
		for (const TArray<AActor*>& Items = Record.Value.Actors;
		     AActor* const Item : Items)
		{
			if (IsValid(Item))
			{
				OutItems.Add(Item);
			}
		}
	}

	return !OutItems.IsEmpty();
}

bool UJuicyEquipmentManager::GetAllItemsFromRegistryByTag_Implementation(const FGameplayTag Tag,
                                                                         TArray<AActor*>& OutItems) const
{
	OutItems.Reset();

	if (const FNestedActors* NestedItems = EquipmentRegistry.Find(Tag);
		NestedItems != nullptr)
	{
		const TArray<AActor*>& Items = NestedItems->Actors;
		OutItems.Reset(Items.Num());

		for (AActor* const Item : Items)
		{
			if (IsValid(Item))
			{
				OutItems.Add(Item);
			}
		}
	}

	return !OutItems.IsEmpty();
}

bool UJuicyEquipmentManager::GetItemFromRegistryByTag_Implementation(const int32 Index, const FGameplayTag Tag,
                                                                     AActor*& OutItem) const
{
	const FNestedActors* NestedItems = EquipmentRegistry.Find(Tag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	AActor* const Item = Items[Index];
	if (!IsValid(Item))
	{
		return false;
	}

	OutItem = Item;
	return true;
}

bool UJuicyEquipmentManager::IsRegistryEmpty_Implementation() const
{
	for (const auto& Record : EquipmentRegistry)
	{
		if (const TArray<AActor*>& Items = Record.Value.Actors;
			!Items.IsEmpty())
		{
			return false;
		}
	}

	return true;
}

bool UJuicyEquipmentManager::FindItemInRegistry_Implementation(AActor* const Item,
                                                               int32& OutIndex, FGameplayTag& OutTag) const
{
	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FGameplayTag ItemTag = ItemComponent->ItemTagSlotType;
	const FNestedActors* NestedItems = EquipmentRegistry.Find(ItemTag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	if (const bool ItemFound = Items.Find(Item, OutIndex);
		!ItemFound)
	{
		return false;
	}

	OutTag = ItemTag;
	return true;
}

bool UJuicyEquipmentManager::IsItemRegistered_Implementation(AActor* Item) const
{
	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FGameplayTag ItemTag = ItemComponent->ItemTagSlotType;
	const FNestedActors* NestedItems = EquipmentRegistry.Find(ItemTag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	return Items.Contains(Item);
}

bool UJuicyEquipmentManager::IsItemRegisteredByTag_Implementation(const int32 Index, const FGameplayTag Tag) const
{
	const FNestedActors* NestedItems = EquipmentRegistry.Find(Tag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	const AActor* Item = Items[Index];
	return IsValid(Item);
}

bool UJuicyEquipmentManager::RegisterItem_Implementation(AActor* Item,
                                                         int32& OutIndex, FGameplayTag& OutTag)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FGameplayTag ItemTag = ItemComponent->ItemTagSlotType;
	FNestedActors* NestedItems = EquipmentRegistry.Find(ItemTag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	OutIndex = Items.AddUnique(Item);
	OutTag = ItemTag;
	return true;
}

bool UJuicyEquipmentManager::RegisterItemByIndex_Implementation(AActor* Item, const int32 Index,
                                                                FGameplayTag& OutTag)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FGameplayTag ItemTag = ItemComponent->ItemTagSlotType;
	FNestedActors* NestedItems = EquipmentRegistry.Find(ItemTag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	if (Items.Contains(Item))
	{
		return false;
	}

	Items.Insert(Item, Index);
	OutTag = ItemTag;
	return true;
}

bool UJuicyEquipmentManager::UnregisterItem_Implementation(AActor* Item,
                                                           FGameplayTag& OutTag)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FGameplayTag ItemTag = ItemComponent->ItemTagSlotType;
	FNestedActors* NestedItems = EquipmentRegistry.Find(ItemTag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	Items.RemoveSingle(Item);
	OutTag = ItemTag;
	return true;
}

bool UJuicyEquipmentManager::UnregisterItemByTag_Implementation(const int32 Index, const FGameplayTag Tag)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	FNestedActors* NestedItems = EquipmentRegistry.Find(Tag);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	Items.RemoveAt(Index);
	return true;
}

bool UJuicyEquipmentManager::EquipItemFromRegistryInSlot_Implementation(const FName Slot, AActor* Item,
                                                                        AActor*& OutPreviousItem, AActor*& OutNewItem)
{
	if (!IsItemRegistered(Item))
	{
		return false;
	}

	return Super::EquipItemInSlot(Slot, Item, OutPreviousItem, OutNewItem);
}

bool UJuicyEquipmentManager::EquipItemFromRegistryInSlotByTag_Implementation(const FName Slot,
                                                                             const int32 Index, const FGameplayTag Tag,
                                                                             AActor*& OutPreviousItem,
                                                                             AActor*& OutNewItem)
{
	AActor* Item;
	if (!GetItemFromRegistryByTag(Index, Tag, Item))
	{
		return false;
	}

	return Super::EquipItemInSlot(Slot, Item, OutPreviousItem, OutNewItem);
}
