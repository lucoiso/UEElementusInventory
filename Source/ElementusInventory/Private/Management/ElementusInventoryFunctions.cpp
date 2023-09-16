// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "Management/ElementusInventoryFunctions.h"
#include <Components/ElementusInventoryComponent.h>
#include "Management/ElementusInventoryData.h"
#include "LogElementusInventory.h"
#include <Engine/AssetManager.h>
#include <Algo/Copy.h>

#ifdef UE_INLINE_GENERATED_CPP_BY_NAME
#include UE_INLINE_GENERATED_CPP_BY_NAME(ElementusInventoryFunctions)
#endif

void UElementusInventoryFunctions::UnloadAllElementusItems()
{
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    if (UAssetManager* const AssetManager = UAssetManager::GetIfInitialized())
#else
    if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
#endif
    {
        AssetManager->UnloadPrimaryAssetsWithType(FPrimaryAssetType(ElementusItemDataType));
    }
}

void UElementusInventoryFunctions::UnloadElementusItem(const FPrimaryElementusItemId& InItemId)
{
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    if (UAssetManager* const AssetManager = UAssetManager::GetIfInitialized())
#else
    if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
#endif
    {
        AssetManager->UnloadPrimaryAsset(FPrimaryAssetId(InItemId));
    }
}

bool UElementusInventoryFunctions::CompareItemInfo(const FElementusItemInfo& Info1, const FElementusItemInfo& Info2)
{
    return Info1 == Info2;
}

bool UElementusInventoryFunctions::CompareItemData(const UElementusItemData* Data1, const UElementusItemData* Data2)
{
    return Data1->GetPrimaryAssetId() == Data2->GetPrimaryAssetId();
}

UElementusItemData* UElementusInventoryFunctions::GetSingleItemDataById(const FPrimaryElementusItemId& InID, const TArray<FName>& InBundles, const bool bAutoUnload)
{
    UElementusItemData* Output = nullptr;

#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    if (UAssetManager* const AssetManager = UAssetManager::GetIfInitialized())
#else
    if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
#endif
    {
        if (const TSharedPtr<FStreamableHandle> StreamableHandle = AssetManager->LoadPrimaryAsset(InID, InBundles); StreamableHandle.IsValid())
        {
            StreamableHandle->WaitUntilComplete(5.f);
            Output = Cast<UElementusItemData>(StreamableHandle->GetLoadedAsset());
        }
        else // The object is already loaded
        {
            Output = AssetManager->GetPrimaryAssetObject<UElementusItemData>(InID);
        }

        if (bAutoUnload)
        {
            AssetManager->UnloadPrimaryAsset(InID);
        }
    }

    return Output;
}

TArray<UElementusItemData*> UElementusInventoryFunctions::GetItemDataArrayById(const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload)
{
    TArray<UElementusItemData*> Output;

#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    if (UAssetManager* const AssetManager = UAssetManager::GetIfInitialized())
#else
    if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
#endif
    {
        Output = LoadElementusItemDatas_Internal(AssetManager, InIDs, InBundles, bAutoUnload);
    }
    return Output;
}

TArray<UElementusItemData*> UElementusInventoryFunctions::SearchElementusItemData(const EElementusSearchType SearchType, const FString& SearchString, const TArray<FName>& InBundles, const bool bAutoUnload)
{
    TArray<UElementusItemData*> Output;

#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    if (UAssetManager* const AssetManager = UAssetManager::GetIfInitialized())
#else
    if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
#endif
    {
        TArray<UElementusItemData*> ReturnedValues = LoadElementusItemDatas_Internal(AssetManager, GetAllElementusItemIds(), InBundles, bAutoUnload);

        for (UElementusItemData* const& Iterator : ReturnedValues)
        {
            UE_LOG(LogElementusInventory_Internal, Display, TEXT("%s: Filtering items. Current iteration: id %s and name %s"), *FString(__func__), *FString::FromInt(Iterator->ItemId), *Iterator->ItemName.ToString());

            bool bAddItem = false;
            switch (SearchType)
            {
            case EElementusSearchType::Name:
                bAddItem = Iterator->ItemName.ToString().Contains(SearchString, ESearchCase::IgnoreCase);
                break;

            case EElementusSearchType::ID:
                bAddItem = FString::FromInt(Iterator->ItemId).Contains(SearchString, ESearchCase::IgnoreCase);
                break;

            case EElementusSearchType::Type:
                bAddItem = ElementusItemEnumTypeToString(Iterator->ItemType).Contains(SearchString, ESearchCase::IgnoreCase);
                break;

            default:
                break;
            }

            if (bAddItem)
            {
                UE_LOG(LogElementusInventory_Internal, Display, TEXT("%s: Item with id %s and name %s matches the search parameters"), *FString(__func__), *FString::FromInt(Iterator->ItemId), *Iterator->ItemName.ToString());

                Output.Add(Iterator);
            }
        }
    }

    return Output;
}

TMap<FGameplayTag, FName> UElementusInventoryFunctions::GetItemMetadatas(const FElementusItemInfo InItemInfo)
{
    TMap<FGameplayTag, FName> Output;
    if (UElementusItemData* const Data = GetSingleItemDataById(InItemInfo.ItemId, TArray<FName> { "Custom" }))
    {
        Output = Data->Metadatas;
        UnloadElementusItem(InItemInfo.ItemId);
    }

    return Output;
}

TMap<FGameplayTag, FPrimaryElementusItemIdContainer> UElementusInventoryFunctions::GetItemRelations(const FElementusItemInfo InItemInfo)
{
    TMap<FGameplayTag, FPrimaryElementusItemIdContainer> Output;
    if (UElementusItemData* const Data = GetSingleItemDataById(InItemInfo.ItemId, TArray<FName> { "Custom" }))
    {
        Output = Data->Relations;
        UnloadElementusItem(InItemInfo.ItemId);
    }

    return Output;
}

TArray<UElementusItemData*> UElementusInventoryFunctions::LoadElementusItemDatas_Internal(UAssetManager* InAssetManager, const TArray<FPrimaryAssetId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload)
{
    TArray<UElementusItemData*> Output;

    const auto CheckAssetValidity_Lambda = [FuncName = __func__](UObject* const& InAsset) -> bool
        {
            const bool bOutput = IsValid(InAsset);
            if (bOutput)
            {
                UE_LOG(LogElementusInventory_Internal, Display, TEXT("%s: Item data %s found and loaded"), *FString(FuncName), *InAsset->GetName());
            }
            else
            {
                UE_LOG(LogElementusInventory_Internal, Error, TEXT("%s: Failed to load item data: Invalid Asset"), *FString(FuncName));
            }

            return bOutput;
        };

    const auto PassItemArr_Lambda = [&CheckAssetValidity_Lambda, &Output, FuncName = __func__](TArray<UObject*>& InArr)
        {
            if (UElementusInventoryFunctions::HasEmptyParam(InArr))
            {
                UE_LOG(LogElementusInventory_Internal, Error, TEXT("%s: Failed to find items with the given parameters"), *FString(FuncName));
            }

            for (UObject* const& Iterator : InArr)
            {
                if (!CheckAssetValidity_Lambda(Iterator))
                {
                    continue;
                }

                if (UElementusItemData* const CastedAsset = Cast<UElementusItemData>(Iterator))
                {
                    Output.Add(CastedAsset);
                }
            }
        };

    if (const TSharedPtr<FStreamableHandle> StreamableHandle = InAssetManager->LoadPrimaryAssets(InIDs, InBundles); StreamableHandle.IsValid())
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

        if (!UElementusInventoryFunctions::HasEmptyParam(Output))
        {
            for (int32 Iterator = 0; Iterator < InIDs.Num(); ++Iterator)
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
        InAssetManager->UnloadPrimaryAssets(InIDs);
    }

    return Output;
}

TArray<UElementusItemData*> UElementusInventoryFunctions::LoadElementusItemDatas_Internal(UAssetManager* InAssetManager, const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload)
{
    const TArray<FPrimaryAssetId> PrimaryAssetIds(InIDs);
    return LoadElementusItemDatas_Internal(InAssetManager, PrimaryAssetIds, InBundles, bAutoUnload);
}

TArray<FElementusItemInfo> UElementusInventoryFunctions::FilterTradeableItems(UElementusInventoryComponent* FromInventory, UElementusInventoryComponent* ToInventory, const TArray<FElementusItemInfo>& Items)
{
    TArray<FElementusItemInfo> Output;
    float VirtualWeight = ToInventory->GetCurrentWeight();

    Algo::CopyIf(Items, Output,
        [&](const FElementusItemInfo& Iterator)
        {
            if (VirtualWeight >= ToInventory->GetMaxWeight())
            {
                return false;
            }

            bool bCanTradeIterator = FromInventory->CanGiveItem(Iterator) && ToInventory->CanReceiveItem(Iterator);

            if (bCanTradeIterator)
            {
                if (const UElementusItemData* const ItemData = UElementusInventoryFunctions::GetSingleItemDataById(Iterator.ItemId, { "Data" }))
                {
                    VirtualWeight += Iterator.Quantity * ItemData->ItemWeight;
                    bCanTradeIterator = bCanTradeIterator && VirtualWeight <= ToInventory->GetMaxWeight();
                }
                else
                {
                    return false;
                }
            }

            return bCanTradeIterator;
        }
    );

    return Output;
}

TArray<FPrimaryAssetId> UElementusInventoryFunctions::GetAllElementusItemIds()
{
    TArray<FPrimaryAssetId> Output;

#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    if (UAssetManager* const AssetManager = UAssetManager::GetIfInitialized())
#else
    if (UAssetManager* const AssetManager = UAssetManager::GetIfValid())
#endif
    {
        AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType(ElementusItemDataType), Output);
    }

    return Output;
}

void UElementusInventoryFunctions::TradeElementusItem(TArray<FElementusItemInfo> ItemsToTrade, UElementusInventoryComponent* FromInventory, UElementusInventoryComponent* ToInventory)
{
    if (UElementusInventoryFunctions::HasEmptyParam(ItemsToTrade))
    {
        return;
    }

    FromInventory->GiveItemsTo(ToInventory, ItemsToTrade);
}

bool UElementusInventoryFunctions::IsItemValid(const FElementusItemInfo InItemInfo)
{
    return InItemInfo.ItemId.IsValid() && InItemInfo != FElementusItemInfo::EmptyItemInfo && InItemInfo.Quantity > 0;
}

bool UElementusInventoryFunctions::IsItemStackable(const FElementusItemInfo InItemInfo)
{
    if (!IsItemValid(InItemInfo))
    {
        return false;
    }

    if (const UElementusItemData* const ItemData = GetSingleItemDataById(InItemInfo.ItemId, { "Data" }))
    {
        return ItemData->bIsStackable;
    }

    return true;
}

FGameplayTagContainer UElementusInventoryFunctions::GetItemTagsWithParentTag(const FElementusItemInfo InItemInfo, const FGameplayTag FromParentTag)
{
    FGameplayTagContainer Output;
    for (const FGameplayTag& Iterator : InItemInfo.Tags)
    {
        if (Iterator.MatchesTag(FromParentTag))
        {
            Output.AddTag(Iterator);
        }
    }

    return Output;
}

FString UElementusInventoryFunctions::ElementusItemEnumTypeToString(const EElementusItemType InEnumName)
{
    switch (InEnumName)
    {
    case EElementusItemType::None:
        return "None";

    case EElementusItemType::Consumable:
        return "Consumable";

    case EElementusItemType::Armor:
        return "Armor";

    case EElementusItemType::Weapon:
        return "Weapon";

    case EElementusItemType::Accessory:
        return "Accessory";

    case EElementusItemType::Crafting:
        return "Crafting";

    case EElementusItemType::Material:
        return "Material";

    case EElementusItemType::Information:
        return "Information";

    case EElementusItemType::Special:
        return "Special";

    case EElementusItemType::Event:
        return "Event";

    case EElementusItemType::Quest:
        return "Quest";

    case EElementusItemType::Junk:
        return "Junk";

    case EElementusItemType::Other:
        return "Other";

    default:
        break;
    }

    return FString();
}