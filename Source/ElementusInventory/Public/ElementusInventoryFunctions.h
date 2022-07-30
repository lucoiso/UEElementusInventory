// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
struct FElementusItemId;
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
	static void UnloadElementusItem(const FElementusItemId& InItemId);

	/* Check if the ids are equal */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool CompareItemInfoIds(const FElementusItemId& Info1, const FElementusItemId& Info2);

	/* Check if the id of the passed datas are equal */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	static bool CompareItemDataIds(const UInventoryItemData* Data1, const UInventoryItemData* Data2);

	/* Return the data of the passed Id */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static UInventoryItemData* GetElementusItemDataById(const FElementusItemId& InID,
	                                                    const TArray<FName>& InBundles,
	                                                    const bool bAutoUnload = true);

	/* Return a array of data depending of the given ids */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<UInventoryItemData*> GetElementusItemDataArrayById(const TArray<FElementusItemId> InIDs,
	                                                                 const TArray<FName>& InBundles,
	                                                                 const bool bAutoUnload = true);

	/* Search items and return a array of item data */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<UInventoryItemData*> SearchElementusItemData(const EElementusSearchType SearchType,
	                                                           const FString& SearchString,
	                                                           const TArray<FName>& InBundles,
	                                                           const bool bAutoUnload = true);

	/* Get ids of all registered items */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static TArray<FElementusItemId> GetElementusItemIds();

	/* Trade items between two inventory components */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	static void TradeElementusItem(TMap<FElementusItemId, int32> ItemsToTrade,
	                               UElementusInventoryComponent* FromInventory,
	                               UElementusInventoryComponent* ToInventory);

private:
	static TArray<UInventoryItemData*> LoadElementusItemDatas_Internal(UAssetManager* InAssetManager,
	                                                                   const TArray<FElementusItemId> InIDs,
	                                                                   const TArray<FName>& InBundles,
	                                                                   const bool bAutoUnload);
};
