// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include <Runtime/Launch/Resources/Version.h>
#include "ElementusInventoryFunctions.generated.h"

UENUM(BlueprintType, Category = "Elementus Inventory | Enumerations")
enum class EElementusSearchType : uint8
{
    Name,
    ID,
    Type
};

class UElementusInventoryComponent;
class UAssetManager;
class UElementusItemData;
struct FPrimaryElementusItemId;

/**
 *
 */
UCLASS(Category = "Elementus Inventory | Functions")
class ELEMENTUSINVENTORY_API UElementusInventoryFunctions final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /* Unload all elementus items that were loaded by Asset Manager */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    static void UnloadAllElementusItems();

    /* Unload a elementus item that was loaded by Asset Manager */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    static void UnloadElementusItem(const FPrimaryElementusItemId& InItemId);

    /* Check if the ids are equal */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static bool CompareItemInfo(const FElementusItemInfo& Info1, const FElementusItemInfo& Info2);

    /* Check if the ids of the given item datas are equal */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static bool CompareItemData(const UElementusItemData* Data1, const UElementusItemData* Data2);

    /* Return the item data related to the given id */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    static UElementusItemData* GetSingleItemDataById(const FPrimaryElementusItemId& InID, const TArray<FName>& InBundles, const bool bAutoUnload = true);

    /* Return a array of data depending of the given ids */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    static TArray<UElementusItemData*> GetItemDataArrayById(const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload = true);

    /* Search all registered elementus items and return a array of item data that match with the given parameters */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    static TArray<UElementusItemData*> SearchElementusItemData(const EElementusSearchType SearchType, const FString& SearchString, const TArray<FName>& InBundles, const bool bAutoUnload = true);

    /* Get the primary asset ids of all registered elementus items */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static TArray<FPrimaryAssetId> GetAllElementusItemIds();

    /* Trade items between two inventory components */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    static void TradeElementusItem(TArray<FElementusItemInfo> ItemsToTrade, UElementusInventoryComponent* FromInventory, UElementusInventoryComponent* ToInventory);

    /* Check if the given item info have a valid id */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static bool IsItemValid(const FElementusItemInfo InItemInfo);

    /* Check if the given item info represents a stackable item */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static bool IsItemStackable(const FElementusItemInfo InItemInfo);

    /* Get item tags providing a parent tag */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static FGameplayTagContainer GetItemTagsWithParentTag(const FElementusItemInfo InItemInfo, const FGameplayTag FromParentTag);

    /* Convert an item type enum value to string */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static FString ElementusItemEnumTypeToString(const EElementusItemType InEnumName);

    template<typename Ty>
    constexpr static const bool HasEmptyParam(const Ty& Arg1)
    {
        if constexpr (std::is_base_of<FString, Ty>())
        {
            return Arg1.IsEmpty();
        }
        else if constexpr (std::is_base_of<FName, Ty>())
        {
            return Arg1.IsNone();
        }
        else
        {
#if ENGINE_MAJOR_VERSION >= 5
            return Arg1.IsEmpty();
#else
            return Arg1.Num() == 0;
#endif
        }
    }

    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static TMap<FGameplayTag, FName> GetItemMetadatas(const FElementusItemInfo InItemInfo);

    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static TMap<FGameplayTag, FPrimaryElementusItemIdContainer> GetItemRelations(const FElementusItemInfo InItemInfo);

private:
    static TArray<UElementusItemData*> LoadElementusItemDatas_Internal(UAssetManager* InAssetManager, const TArray<FPrimaryAssetId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload);
    static TArray<UElementusItemData*> LoadElementusItemDatas_Internal(UAssetManager* InAssetManager, const TArray<FPrimaryElementusItemId>& InIDs, const TArray<FName>& InBundles, const bool bAutoUnload);

public:
    /* Filter the container and return only items that can be traded at the current context */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    static TArray<FElementusItemInfo> FilterTradeableItems(UElementusInventoryComponent* FromInventory, UElementusInventoryComponent* ToInventory, const TArray<FElementusItemInfo>& Items);
};
