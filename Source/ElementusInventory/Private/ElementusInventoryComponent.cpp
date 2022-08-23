// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryComponent.h"
#include "ElementusInventoryFunctions.h"
#include "Engine/AssetManager.h"
#include "GameFramework/Actor.h"

UElementusInventoryComponent::UElementusInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  MaxWeight(0.f),
	  CurrentWeight(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

float UElementusInventoryComponent::GetCurrentWeight() const
{
	return CurrentWeight;
}

TArray<FElementusItemInfo> UElementusInventoryComponent::GetItemStack() const
{
	return ItemStack;
}

FElementusItemInfo& UElementusInventoryComponent::GetItemReferenceAt(const int32 Index)
{
	return ItemStack[Index];
}

void UElementusInventoryComponent::AddElementusItem(const FElementusItemInfo& InModifier)
{
	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Adding %d item(s) with name '%s' to inventory"),
	       *FString(__func__), InModifier.Quantity, *InModifier.ItemId.ToString());

	AddElementusItem_Internal(InModifier);
}

void UElementusInventoryComponent::RemoveElementusItem(const FElementusItemInfo& InModifier)
{
	UE_LOG(LogElementusInventory, Display,
	       TEXT("Elementus Inventory - %s: Discarding %d item(s) with name '%s' from inventory"),
	       *FString(__func__), InModifier.Quantity, *InModifier.ItemId.ToString());

	RemoveElementusItem_Internal(InModifier);
}

constexpr void DoMulticastLoggingIdentification(const ENetMode& CurrentNetMode)
{
	if (CurrentNetMode == NM_Client)
	{
		UE_LOG(LogElementusInventory_Internal, Warning,
			TEXT("Elementus Inventory - %s: Client logging: "),
		       *FString(__func__));
	}
	else if (CurrentNetMode != NM_Standalone)
	{
		UE_LOG(LogElementusInventory_Internal, Warning,
			TEXT("Elementus Inventory - %s: Server logging: "),
		       *FString(__func__));
	}
}

void UElementusInventoryComponent::AddElementusItem_Internal_Implementation(const FElementusItemInfo& AddInfo)
{
	DoMulticastLoggingIdentification(GetOwner()->GetNetMode());

	UE_LOG(LogElementusInventory_Internal, Display,
	       TEXT("Elementus Inventory - %s: Adding %d item(s) with name '%s' to inventory"),
	       *FString(__func__), AddInfo.Quantity, *AddInfo.ItemId.ToString());

	if (int32 InIndex;
		UElementusInventoryFunctions::CanStackItem(AddInfo)
		&& FindElementusItemInStack(AddInfo, InIndex))
	{
		ItemStack[InIndex].Quantity += AddInfo.Quantity;
	}
	else if (!UElementusInventoryFunctions::CanStackItem(AddInfo))
	{
		for (int32 i = 0; i < AddInfo.Quantity; ++i)
		{
			ItemStack.Add(FElementusItemInfo(AddInfo.ItemId, 1, AddInfo.Tags));
		}
	}
	else
	{
		ItemStack.Add(AddInfo);
	}

	NotifyInventoryChange(AddInfo, EElementusInventoryUpdateOperation::Add);
}

void UElementusInventoryComponent::RemoveElementusItem_Internal_Implementation(const FElementusItemInfo& RemoveInfo)
{
	DoMulticastLoggingIdentification(GetOwner()->GetNetMode());

	UE_LOG(LogElementusInventory_Internal, Display,
	       TEXT("Elementus Inventory - %s: Removing %d item(s) with name '%s' from inventory"),
	       *FString(__func__), RemoveInfo.Quantity, *RemoveInfo.ItemId.ToString());

	bool bNotify = false;
	int32 Residue = 0;
	do
	{
		if (int32 InIndex;
			FindElementusItemInStack(RemoveInfo, InIndex))
		{
			Residue = RemoveInfo.Quantity - ItemStack[InIndex].Quantity;

			ItemStack[InIndex].Quantity =
				FMath::Clamp<int32>(ItemStack[InIndex].Quantity - RemoveInfo.Quantity,
				                    0,
				                    ItemStack[InIndex].Quantity);

			if (ItemStack[InIndex].Quantity <= 0)
			{
				ItemStack.RemoveAt(InIndex);
			}

			bNotify = true;
		}
	}
	while (Residue > 0);

	if (bNotify)
	{
		NotifyInventoryChange(RemoveInfo, EElementusInventoryUpdateOperation::Remove);
	}
}

bool UElementusInventoryComponent::CanReceiveItem(const FElementusItemInfo InItemInfo) const
{
	if (MaxWeight == 0.f)
	{
		return true;
	}

	if (const UElementusItemData* ItemData =
		UElementusInventoryFunctions::GetElementusItemDataById(InItemInfo.ItemId, {"Data"}))
	{
		if (MaxWeight >= CurrentWeight + ItemData->ItemWeight * InItemInfo.Quantity)
		{
			return true;
		}
	}

	UE_LOG(LogElementusInventory, Warning,
	       TEXT("Elementus Inventory - %s: Actor %s cannot receive %d item(s) with name '%s'"),
	       *FString(__func__), *GetOwner()->GetName(), InItemInfo.Quantity, *InItemInfo.ItemId.ToString());

	return false;
}

bool UElementusInventoryComponent::CanGiveItem(const FElementusItemInfo InItemInfo) const
{
	int32 InIndex;
	const bool bOutput = FindElementusItemInStack(InItemInfo, InIndex) && ItemStack[InIndex].Quantity >= InItemInfo.Quantity;

	if (!bOutput)
	{
		UE_LOG(LogElementusInventory, Warning,
		       TEXT("Elementus Inventory - %s: Actor %s cannot give %d item(s) with name '%s'"),
		       *FString(__func__), *GetOwner()->GetName(), InItemInfo.Quantity, *InItemInfo.ItemId.ToString());
	}

	return bOutput;
}

void UElementusInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateCurrentWeight();
	UpdateInventoryStack();
}

void UElementusInventoryComponent::NotifyInventoryChange(const FElementusItemInfo& Modifier,
                                                         const EElementusInventoryUpdateOperation Operation)
{
	OnInventoryUpdate.Broadcast(Modifier, Operation);

	if (ItemStack.IsEmpty())
	{
		CurrentWeight = 0.f;
		OnInventoryEmpty.Broadcast();
	}
	else if (const UElementusItemData* ItemData =
		UElementusInventoryFunctions::GetElementusItemDataById(Modifier.ItemId, {"Data"});
		ContainItemInStack(Modifier)
		&& Operation == EElementusInventoryUpdateOperation::Add)
	{
		CurrentWeight += ItemData->ItemWeight * Modifier.Quantity;
	}
	else
	{
		UpdateCurrentWeight();
	}
}

void UElementusInventoryComponent::UpdateCurrentWeight()
{
	float NewWeigth = 0.f;
	for (const auto& Iterator : ItemStack)
	{
		if (const UElementusItemData* ItemData =
			UElementusInventoryFunctions::GetElementusItemDataById(Iterator.ItemId, {"Data"}))
		{
			NewWeigth += ItemData->ItemWeight * Iterator.Quantity;
		}
	}

	CurrentWeight = NewWeigth;
}

void UElementusInventoryComponent::UpdateInventoryStack()
{
	for (int32 i = 0; i < ItemStack.Num(); ++i)
	{
		if (ItemStack[i].Quantity <= 0)
		{
			ItemStack.RemoveAt(i);
			--i;
		}

		else if (const UElementusItemData* ItemData =
				UElementusInventoryFunctions::GetElementusItemDataById(ItemStack[i].ItemId, {"Data"});
			IsValid(ItemData)
			&& !ItemData->bIsStackable && ItemStack[i].Quantity > 1)
		{
			const int32 Quant = ItemStack[i].Quantity;
			ItemStack[i].Quantity = 1;
			for (int32 j = 0; j < Quant - 1; ++j)
			{
				ItemStack.Add(FElementusItemInfo(ItemStack[i].ItemId, 1, ItemStack[i].Tags));
			}
		}
	}

	ItemStack.Sort();
}

bool UElementusInventoryComponent::FindElementusItemInStack(const FElementusItemInfo InItemInfo,
                                                            int32& OutIndex,
                                                            const FGameplayTagContainer IgnoreTags) const
{
	OutIndex = ItemStack.IndexOfByPredicate([&InItemInfo, &IgnoreTags](const FElementusItemInfo& InInfo)
	{
		if (IgnoreTags.IsEmpty())
		{
			return InInfo == InItemInfo;
		}
		
		FElementusItemInfo InCopy(InItemInfo);
		InCopy.Tags.RemoveTags(IgnoreTags);

		FElementusItemInfo InLambdaCopy(InItemInfo);
		InLambdaCopy.Tags.RemoveTags(IgnoreTags);

		return InCopy == InLambdaCopy;
	});

	return OutIndex != INDEX_NONE;
}

bool UElementusInventoryComponent::FindElementusItemInStackWithTags(const FGameplayTagContainer WithTags,
																	int32& OutIndex,
																	const FGameplayTagContainer IgnoreTags) const
{
	OutIndex = ItemStack.IndexOfByPredicate([&WithTags, &IgnoreTags](const FElementusItemInfo& InInfo)
	{
		FElementusItemInfo InCopy(InInfo);
		if (!IgnoreTags.IsEmpty())
		{
			InCopy.Tags.RemoveTags(IgnoreTags);
		}
		
		return InCopy.Tags.HasAll(WithTags);
	});

	return OutIndex != INDEX_NONE;
}

bool UElementusInventoryComponent::ContainItemInStack(const FElementusItemInfo InItemInfo) const
{
	return ItemStack.FindByPredicate([&InItemInfo](const FElementusItemInfo& InInfo)
	{
		return InInfo == InItemInfo;
	}) != nullptr;
}

void UElementusInventoryComponent::DebugInventoryStack()
{
	UE_LOG(LogElementusInventory, Warning, TEXT("Elementus Inventory - %s"), *FString(__func__));
	UE_LOG(LogElementusInventory, Warning, TEXT("Owning Actor: %s"), *GetOwner()->GetName());

	for (const auto& Iterator : ItemStack)
	{
		UE_LOG(LogElementusInventory, Warning, TEXT("Item: %s"), *Iterator.ItemId.ToString());
		UE_LOG(LogElementusInventory, Warning, TEXT("Quantity: %d"), Iterator.Quantity);

		for (const auto& Tag : Iterator.Tags)
		{
			UE_LOG(LogElementusInventory, Warning, TEXT("Tag: %s"), *Tag.ToString());
		}
	}

	UE_LOG(LogElementusInventory, Warning, TEXT("Weight: %d"), CurrentWeight);
}