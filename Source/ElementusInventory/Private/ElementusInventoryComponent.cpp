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

TArray<FElementusItemInfo> UElementusInventoryComponent::GetItemsArray() const
{
	return ElementusItems;
}

FElementusItemInfo& UElementusInventoryComponent::GetItemReferenceAt(const int32 Index)
{
	return ElementusItems[Index];
}

FElementusItemInfo UElementusInventoryComponent::GetItemCopyAt(const int32 Index) const
{
	return ElementusItems[Index];
}

bool UElementusInventoryComponent::CanReceiveItem(const FElementusItemInfo InItemInfo) const
{
	if (MaxWeight == 0.f)
	{
		return true;
	}

	if (const UElementusItemData* ItemData =
		UElementusInventoryFunctions::GetSingleItemDataById(InItemInfo.ItemId, {"Data"}))
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
	const bool bOutput = FindFirstItemIndexWithInfo(InItemInfo, InIndex) && ElementusItems[InIndex].Quantity >= InItemInfo.Quantity;

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

	RefreshInventory();
}

void UElementusInventoryComponent::RefreshInventory()
{
	ForceWeightUpdate();
	ForceInventoryValidation();
}

void UElementusInventoryComponent::ForceWeightUpdate()
{
	float NewWeigth = 0.f;
	for (const auto& Iterator : ElementusItems)
	{
		if (const UElementusItemData* ItemData =
			UElementusInventoryFunctions::GetSingleItemDataById(Iterator.ItemId, {"Data"}))
		{
			NewWeigth += ItemData->ItemWeight * Iterator.Quantity;
		}
	}

	CurrentWeight = NewWeigth;
}

void UElementusInventoryComponent::ForceInventoryValidation()
{
	TArray<FElementusItemInfo> NewItems;
	TArray<FElementusItemInfo> ItemsToRemove;
	
	for (int32 i = 0; i < ElementusItems.Num(); ++i)
	{
		if (ElementusItems[i].Quantity <= 0)
		{
			ItemsToRemove.Add(ElementusItems[i]);
		}

		else if (const UElementusItemData* ItemData =
				UElementusInventoryFunctions::GetSingleItemDataById(ElementusItems[i].ItemId, {"Data"});
			IsValid(ItemData)
			&& !ItemData->bIsStackable && ElementusItems[i].Quantity > 1)
		{
			for (int32 j = 0; j < ElementusItems[i].Quantity; ++j)
			{
				NewItems.Add(FElementusItemInfo(ElementusItems[i].ItemId, 1, ElementusItems[i].Tags));
			}
			
			ItemsToRemove.Add(ElementusItems[i]);
		}
	}

	if (!ItemsToRemove.IsEmpty())
	{
		Multicast_ProcessGroupItemUpdate(ItemsToRemove.Top(), EElementusInventoryUpdateOperation::Remove, ItemsToRemove, false);
	}
	if (!NewItems.IsEmpty())
	{
		Multicast_ProcessGroupItemUpdate(NewItems.Top(), EElementusInventoryUpdateOperation::Add, NewItems, false);
	}
	
	ElementusItems.Sort();
}

bool UElementusInventoryComponent::FindFirstItemIndexWithInfo(const FElementusItemInfo InItemInfo,
                                                            int32& OutIndex,
                                                            const FGameplayTagContainer IgnoreTags) const
{
	OutIndex = ElementusItems.IndexOfByPredicate([&InItemInfo, &IgnoreTags](const FElementusItemInfo& InInfo)
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

bool UElementusInventoryComponent::FindFirstItemIndexWithTags(const FGameplayTagContainer WithTags,
																	int32& OutIndex,
																	const FGameplayTagContainer IgnoreTags) const
{
	OutIndex = ElementusItems.IndexOfByPredicate([&WithTags, &IgnoreTags](const FElementusItemInfo& InInfo)
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

bool UElementusInventoryComponent::FindFirstItemIndexWithId(const FPrimaryElementusItemId InId,
																int32& OutIndex,
																const FGameplayTagContainer IgnoreTags) const
{
	OutIndex = ElementusItems.IndexOfByPredicate([&InId, &IgnoreTags](const FElementusItemInfo& InInfo)
	{
		return !InInfo.Tags.HasAll(IgnoreTags) && InInfo.ItemId == InId;
	});

	return OutIndex != INDEX_NONE;
}

bool UElementusInventoryComponent::FindAllItemIndexesWithInfo(const FElementusItemInfo InItemInfo,
																		TArray<int32>& OutIndexes,
																		const FGameplayTagContainer IgnoreTags) const
{
	for (auto Iterator = ElementusItems.CreateConstIterator(); Iterator; ++Iterator)
	{
		if (IgnoreTags.IsEmpty() && *Iterator == InItemInfo)
		{
			OutIndexes.Add(Iterator.GetIndex());
			continue;
		}
		
		FElementusItemInfo InItCopy(*Iterator);
		InItCopy.Tags.RemoveTags(IgnoreTags);

		FElementusItemInfo InParamCopy(InItemInfo);
		InParamCopy.Tags.RemoveTags(IgnoreTags);
		
		if (InItCopy == InParamCopy)
		{	
			OutIndexes.Add(Iterator.GetIndex());
		}
	}

	return !OutIndexes.IsEmpty();
}

bool UElementusInventoryComponent::FindAllItemIndexesWithTags(const FGameplayTagContainer WithTags,
																		TArray<int32>& OutIndexes,
																		const FGameplayTagContainer IgnoreTags) const
{
	for (auto Iterator = ElementusItems.CreateConstIterator(); Iterator; ++Iterator)
	{
		FElementusItemInfo InCopy(*Iterator);
		if (!IgnoreTags.IsEmpty())
		{
			InCopy.Tags.RemoveTags(IgnoreTags);
		}
		
		if (InCopy.Tags.HasAll(WithTags))
		{	
			OutIndexes.Add(Iterator.GetIndex());
		}
	}

	return !OutIndexes.IsEmpty();
}

bool UElementusInventoryComponent::FindAllItemIndexesWithId(const FPrimaryElementusItemId InId,
																	TArray<int32>& OutIndexes,
																	const FGameplayTagContainer IgnoreTags) const
{
	for (auto Iterator = ElementusItems.CreateConstIterator(); Iterator; ++Iterator)
	{
		if (!Iterator->Tags.HasAll(IgnoreTags) && Iterator->ItemId == InId)
		{	
			OutIndexes.Add(Iterator.GetIndex());
		}
	}

	return !OutIndexes.IsEmpty();
}

bool UElementusInventoryComponent::ContainsItem(const FElementusItemInfo InItemInfo) const
{
	return ElementusItems.FindByPredicate([&InItemInfo](const FElementusItemInfo& InInfo)
	{
		return InInfo == InItemInfo;
	}) != nullptr;
}

void UElementusInventoryComponent::DebugInventory()
{
	UE_LOG(LogElementusInventory, Warning, TEXT("Elementus Inventory - %s"), *FString(__func__));
	UE_LOG(LogElementusInventory, Warning, TEXT("Owning Actor: %s"), *GetOwner()->GetName());

	for (const auto& Iterator : ElementusItems)
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

void UElementusInventoryComponent::ClearInventory_Implementation()
{
	UE_LOG(LogElementusInventory, Display,
		TEXT("Elementus Inventory - %s: Cleaning %s's inventory"),
		*FString(__func__), *GetOwner()->GetName());
	
	ElementusItems.Empty();
	CurrentWeight = 0.f;
}

void UElementusInventoryComponent::AddElementusItem(const FElementusItemInfo& InModifier)
{
	UE_LOG(LogElementusInventory, Display,
		   TEXT("Elementus Inventory - %s: Adding %d item(s) with name '%s' to inventory"),
		   *FString(__func__), InModifier.Quantity, *InModifier.ItemId.ToString());

	if (const bool bIsStackable = UElementusInventoryFunctions::IsItemStackable(InModifier);
		InModifier.Quantity > 1 && !bIsStackable)
	{
		TArray<FElementusItemInfo> NewItems;
		for (int32 i = 0; i < InModifier.Quantity; ++i)
		{
			NewItems.Add(FElementusItemInfo(InModifier.ItemId, 1, InModifier.Tags));
		}

		Multicast_ProcessGroupItemUpdate(InModifier, EElementusInventoryUpdateOperation::Add, NewItems);
	}	
	else if (int32 InIndex;
			 bIsStackable && FindFirstItemIndexWithInfo(InModifier, InIndex))
	{
		Multicast_ProcessSingleItemUpdate(InModifier, EElementusInventoryUpdateOperation::Add, InIndex);
	}
	else
	{
		Multicast_ProcessSingleItemUpdate(InModifier, EElementusInventoryUpdateOperation::Add, INDEX_NONE, true);
	}
}

void UElementusInventoryComponent::RemoveElementusItem(const FElementusItemInfo& InModifier)
{
	UE_LOG(LogElementusInventory, Display,
		   TEXT("Elementus Inventory - %s: Removing %d item(s) with name '%s' from inventory"),
		   *FString(__func__), InModifier.Quantity, *InModifier.ItemId.ToString());

	if (InModifier.Quantity > 1 && !UElementusInventoryFunctions::IsItemStackable(InModifier))
	{
		TArray<FElementusItemInfo> ItemsToRemove;	
		int32 Residue = 0;
		do
		{
			if (int32 InIndex;
				FindFirstItemIndexWithInfo(InModifier, InIndex))
			{
				Residue = ElementusItems[InIndex].Quantity - InModifier.Quantity;

				if (Residue <= 0)
				{
					ItemsToRemove.Add(ElementusItems[InIndex]);
				}
				else
				{
					const FElementusItemInfo ResidueModifier
					{
						InModifier.ItemId,
						Residue,
						InModifier.Tags
					};
				
					Multicast_ProcessSingleItemUpdate(ResidueModifier, EElementusInventoryUpdateOperation::Remove, InIndex);
					break;
				}
			}
		}
		while (Residue > 0);

		if (!ItemsToRemove.IsEmpty())
		{
			Multicast_ProcessGroupItemUpdate(InModifier, EElementusInventoryUpdateOperation::Remove, ItemsToRemove);
		}
	}
	else if (int32 InIndex;		
			 FindFirstItemIndexWithInfo(InModifier, InIndex))
	{
		Multicast_ProcessSingleItemUpdate(InModifier, EElementusInventoryUpdateOperation::Remove, InIndex);
	}
}

void UElementusInventoryComponent::Multicast_ProcessSingleItemUpdate_Implementation(const FElementusItemInfo& Modifier,
																					const EElementusInventoryUpdateOperation Operation,
																					const uint8 InIndex,
																					const bool bAddNewOrRemoveAll,
																					const bool bNotify)
{
	switch (Operation)
	{
		case EElementusInventoryUpdateOperation::Add :
			bAddNewOrRemoveAll ? ElementusItems.Add(Modifier) : ElementusItems[InIndex].Quantity += Modifier.Quantity;
			break;
			
		case EElementusInventoryUpdateOperation::Remove :
			ElementusItems[InIndex].Quantity = FMath::Clamp<int32>(ElementusItems[InIndex].Quantity - Modifier.Quantity,
							   								  0,
							   								  ElementusItems[InIndex].Quantity);

			if (ElementusItems[InIndex].Quantity <= 0 || bAddNewOrRemoveAll)
			{
				ElementusItems.RemoveAt(InIndex);
			}
			break;
			
		default:
			break;
	}
	
	if (bNotify)
	{
		NotifyInventoryChange(Modifier, Operation);
	}	
}

void UElementusInventoryComponent::Multicast_ProcessGroupItemUpdate_Implementation(const FElementusItemInfo& Modifier,
																				   const EElementusInventoryUpdateOperation Operation,
																				   const TArray<FElementusItemInfo>& InGroup,
																				   const bool bNotify)
{	
	switch (Operation)
	{
		case EElementusInventoryUpdateOperation::Add :
			ElementusItems.Append(InGroup);
			break;
				
		case EElementusInventoryUpdateOperation::Remove :
			ElementusItems.RemoveAll([&InGroup, this](const FElementusItemInfo& InInfo)
			{
				return InGroup.Contains(InInfo);
			});
		
			break;
				
		default:
			break;
	}
	
	if (bNotify)
	{
		NotifyInventoryChange(Modifier, Operation);
	}
}

void UElementusInventoryComponent::NotifyInventoryChange(const FElementusItemInfo& Modifier,
                                                         const EElementusInventoryUpdateOperation Operation)
{
	OnInventoryUpdate.Broadcast(Modifier, Operation);

	if (ElementusItems.IsEmpty())
	{
		CurrentWeight = 0.f;
		OnInventoryEmpty.Broadcast();
	}
	else if (const UElementusItemData* ItemData = UElementusInventoryFunctions::GetSingleItemDataById(Modifier.ItemId,
																										 {"Data"});
		Operation == EElementusInventoryUpdateOperation::Add && ContainsItem(Modifier))
	{
		CurrentWeight += ItemData->ItemWeight * Modifier.Quantity;
	}
	else
	{
		CurrentWeight -= ItemData->ItemWeight * Modifier.Quantity;
	}
}
