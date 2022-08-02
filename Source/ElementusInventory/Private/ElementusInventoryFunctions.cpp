// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryFunctions.h"
#include "ElementusInventoryComponent.h"
#include "Engine/AssetManager.h"

void UElementusInventoryFunctions::UnloadAllElementusItems()
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		AssetManager->UnloadPrimaryAssetsWithType(FPrimaryAssetType(ElementusItemDataType));
	}
}

void UElementusInventoryFunctions::UnloadElementusItem(const FPrimaryElementusItemId& InItemId)
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		AssetManager->UnloadPrimaryAsset(FPrimaryAssetId(InItemId));
	}
}

bool UElementusInventoryFunctions::CompareElementusItems(const FElementusItemInfo& Info1,
                                                         const FElementusItemInfo& Info2)
{
	return Info1 == Info2;
}

bool UElementusInventoryFunctions::CompareElementusItemDatas(const UInventoryItemData* Data1,
                                                             const UInventoryItemData* Data2)
{
	return Data1->GetPrimaryAssetId() == Data2->GetPrimaryAssetId();
}

UInventoryItemData* UElementusInventoryFunctions::GetElementusItemDataById(const FPrimaryElementusItemId& InID,
                                                                           const TArray<FName>& InBundles,
                                                                           const bool bAutoUnload)
{
	UInventoryItemData* Output = nullptr;
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		if (const TSharedPtr<FStreamableHandle> StreamableHandle =
				AssetManager->LoadPrimaryAsset(InID, InBundles);
			StreamableHandle.IsValid())
		{
			StreamableHandle->WaitUntilComplete(5.f);
			Output = Cast<UInventoryItemData>(StreamableHandle->GetLoadedAsset());
		}
		else // The object is already loaded
		{
			Output = AssetManager->GetPrimaryAssetObject<UInventoryItemData>(InID);
		}

		if (bAutoUnload)
		{
			AssetManager->UnloadPrimaryAsset(InID);
		}
	}

	return Output;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::GetElementusItemDataArrayById(
	const TArray<FPrimaryElementusItemId> InIDs,
	const TArray<FName>& InBundles,
	const bool bAutoUnload)
{
	TArray<UInventoryItemData*> Output;
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		Output = LoadElementusItemDatas_Internal(AssetManager, InIDs, InBundles, bAutoUnload);
	}
	return Output;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::SearchElementusItemData(const EElementusSearchType SearchType,
	const FString& SearchString,
	const TArray<FName>& InBundles,
	const bool bAutoUnload)
{
	TArray<UInventoryItemData*> Output;
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		TArray<UInventoryItemData*> ReturnedValues =
			LoadElementusItemDatas_Internal(AssetManager, TArray<FPrimaryElementusItemId>(), InBundles, bAutoUnload);

		for (const auto& Iterator : ReturnedValues)
		{
			bool bAddItem = false;
			switch (SearchType)
			{
			case EElementusSearchType::Name:
				bAddItem = Iterator->ItemName.ToString().Contains(SearchString);
				break;

			case EElementusSearchType::ID:
				bAddItem = FString::FromInt(Iterator->ItemId).Contains(SearchString);
				break;

			case EElementusSearchType::Type:
				bAddItem = static_cast<int8>(Iterator->ItemType) == FCString::Atoi(*SearchString);
				break;

			default:
				break;
			}

			if (bAddItem)
			{
				UE_LOG(LogElementusInventory_Internal, Display,
				       TEXT("Elementus Inventory - %s: Added Item Name: %s"),
				       *FString(__func__), *Iterator->ItemName.ToString());

				Output.Add(Iterator);
			}
		}
	}

	return Output;
}

TArray<UInventoryItemData*> UElementusInventoryFunctions::LoadElementusItemDatas_Internal(UAssetManager* InAssetManager,
	const TArray<FPrimaryElementusItemId> InIDs,
	const TArray<FName>& InBundles,
	const bool bAutoUnload)
{
	TArray<UInventoryItemData*> Output;
	const TArray<FPrimaryAssetId> PrimaryAssetIds(InIDs);

	constexpr auto& FuncNam_LambVer = __func__;
	const auto& CheckAssetValidity_Lambda =
		[&FuncNam_LambVer](UObject* InAsset) -> bool
	{
		const bool bOutput = IsValid(InAsset);
		if (IsValid(InAsset))
		{
			UE_LOG(LogElementusInventory_Internal, Display,
			       TEXT("Elementus Inventory - %s: Item data %s found and loaded"),
			       *FString(FuncNam_LambVer), *InAsset->GetName());
		}
		else
		{
			UE_LOG(LogElementusInventory_Internal, Error,
			       TEXT("Elementus Inventory - %s: Failed to load item data: Invalid Asset"),
			       *FString(FuncNam_LambVer));
		}

		return bOutput;
	};

	const auto& PassItemArr_Lambda =
		[&CheckAssetValidity_Lambda, &Output](TArray<UObject*>& InArr)
	{
		if (InArr.IsEmpty())
		{
			UE_LOG(LogElementusInventory_Internal, Error,
			       TEXT("Elementus Inventory - %s: Failed to find items with the given parameters"),
			       *FString(__func__));
		}

		for (UObject* Iterator : InArr)
		{
			if (!CheckAssetValidity_Lambda(Iterator))
			{
				continue;
			}

			if (UInventoryItemData* CastedAsset = Cast<UInventoryItemData>(Iterator))
			{
				Output.Add(CastedAsset);
			}
		}
	};

	if (const TSharedPtr<FStreamableHandle> StreamableHandle =
			InAssetManager->LoadPrimaryAssets(PrimaryAssetIds, InBundles);
		StreamableHandle.IsValid())
	{
		StreamableHandle->WaitUntilComplete(5.f);

		TArray<UObject*> LoadedAssets;
		StreamableHandle->GetLoadedAssets(LoadedAssets);
		PassItemArr_Lambda(LoadedAssets);
	}
	else // Objects already loaded
	{
		if (TArray<UObject*> LoadedAssets;
			InAssetManager->GetPrimaryAssetObjectList(FPrimaryAssetType(ElementusItemDataType), LoadedAssets))
		{
			PassItemArr_Lambda(LoadedAssets);
		}

		if (!Output.IsEmpty())
		{
			for (int Iterator = 0; Iterator < InIDs.Num(); ++Iterator)
			{
				if (!InIDs.Contains(Output[Iterator]->GetPrimaryAssetId()))
				{
					Output.RemoveAt(Iterator);
					--Iterator;
				}
			}
		}
	}

	if (bAutoUnload)
	{
		// Unload all elementus item assets
		InAssetManager->UnloadPrimaryAssets(PrimaryAssetIds);
	}
	return Output;
}

TArray<FPrimaryElementusItemId> UElementusInventoryFunctions::GetAllElementusItemIds()
{
	if (const UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		if (TArray<FPrimaryAssetId> IdList;
			AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType(ElementusItemDataType), IdList))
		{
			return TArray<FPrimaryElementusItemId>(IdList);
		}
	}
	return TArray<FPrimaryElementusItemId>();
}

void UElementusInventoryFunctions::TradeElementusItem(TArray<FElementusItemInfo> ItemsToTrade,
                                                      UElementusInventoryComponent* FromInventory,
                                                      UElementusInventoryComponent* ToInventory)
{
	for (auto& Iterator : ItemsToTrade)
	{
		Iterator.Quantity =
			FMath::Clamp(Iterator.Quantity, 0, Iterator.Quantity);

		if (FromInventory->CanGiveItem(Iterator) && ToInventory->CanReceiveItem(Iterator))
		{
			FromInventory->RemoveElementusItem(Iterator);
			ToInventory->AddElementusItem(Iterator);
		}
		else
		{
			UE_LOG(LogElementusInventory_Internal, Error,
			       TEXT("Elementus Inventory - %s: Failed to trade item %s"),
			       *FString(__func__), *Iterator.ItemId.ToString());
		}
	}
}

bool UElementusInventoryFunctions::IsItemValid(const FElementusItemInfo InItemInfo)
{
	return InItemInfo.ItemId.IsValid();
}
