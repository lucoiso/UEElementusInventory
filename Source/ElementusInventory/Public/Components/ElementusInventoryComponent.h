// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>
#include <Components/ActorComponent.h>
#include "Management/ElementusInventoryData.h"
#include "ElementusInventoryComponent.generated.h"

UENUM(Category = "Elementus Inventory | Enumerations")
enum class EElementusInventoryUpdateOperation : uint8
{
    None,
    Add,
    Remove
};

UENUM(Category = "Elementus Inventory | Enumerations")
enum class EElementusInventorySortingMode : uint8
{
    ID,
    Name,
    Type,
    IndividualValue,
    StackValue,
    IndividualWeight,
    StackWeight,
    Quantity,
    Level,
    Tags
};

UENUM(Category = "Elementus Inventory | Enumerations")
enum class EElementusInventorySortingOrientation : uint8
{
    Ascending,
    Descending
};

USTRUCT(Category = "Elementus Inventory | Structures")
struct FItemModifierData
{
    GENERATED_BODY()

    FItemModifierData() = default;

    explicit FItemModifierData(const FElementusItemInfo& InItemInfo) : ItemInfo(InItemInfo)
    {
    }

    explicit FItemModifierData(const FElementusItemInfo& InItemInfo, const int32& InIndex) : ItemInfo(InItemInfo), Index(InIndex)
    {
    }

    FElementusItemInfo ItemInfo = FElementusItemInfo();
    int32 Index = INDEX_NONE;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElementusInventoryUpdate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElementusInventoryEmpty);

UCLASS(Blueprintable, ClassGroup = (Custom), Category = "Elementus Inventory | Classes", EditInlineNew, meta = (BlueprintSpawnableComponent))
class ELEMENTUSINVENTORY_API UElementusInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    explicit UElementusInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /* Experimental parameter to assist using empty slots in the inventory: If true, will replace empty slots with empty item info */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
    bool bAllowEmptySlots;

    /* Get the current inventory weight */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    float GetCurrentWeight() const;

    /* Get the max inventory weight */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    float GetMaxWeight() const;

    /* Get the current num of items in this inventory */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    int32 GetCurrentNumItems() const;

    /* Get the current max num of items in this inventory */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    int32 GetMaxNumItems() const;

    /* Called on every inventory update */
    UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
    FElementusInventoryUpdate OnInventoryUpdate;

    /* Called when the inventory is empty */
    UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
    FElementusInventoryEmpty OnInventoryEmpty;

    /* Get the items that this inventory have */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    TArray<FElementusItemInfo> GetItemsArray() const;

    /* Get a reference of the item at given index */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    FElementusItemInfo& GetItemReferenceAt(const int32 Index);

    /* Get a copy of the item at given index */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    FElementusItemInfo GetItemCopyAt(const int32 Index) const;

    /* Check if this inventory can receive the item */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    virtual bool CanReceiveItem(const FElementusItemInfo InItemInfo) const;

    /* Check if this inventory can give the item */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    virtual bool CanGiveItem(const FElementusItemInfo InItemInfo) const;

    /* Find the first elementus item that matches the specified info */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory", meta = (AutoCreateRefTerm = "IgnoreTags"))
    bool FindFirstItemIndexWithInfo(const FElementusItemInfo InItemInfo, int32& OutIndex, const FGameplayTagContainer& IgnoreTags, const int32 Offset = 0) const;

    /* Find the first elementus item that matches the specified tag container */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory", meta = (AutoCreateRefTerm = "IgnoreTags"))
    bool FindFirstItemIndexWithTags(const FGameplayTagContainer WithTags, int32& OutIndex, const FGameplayTagContainer& IgnoreTags, const int32 Offset = 0) const;

    /* Find the first elementus item that matches the specified id */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory", meta = (AutoCreateRefTerm = "IgnoreTags"))
    bool FindFirstItemIndexWithId(const FPrimaryElementusItemId InId, int32& OutIndex, const FGameplayTagContainer& IgnoreTags, const int32 Offset = 0) const;

    /* Find the first elementus item that matches the specified info */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory", meta = (AutoCreateRefTerm = "IgnoreTags"))
    bool FindAllItemIndexesWithInfo(const FElementusItemInfo InItemInfo, TArray<int32>& OutIndexes, const FGameplayTagContainer& IgnoreTags) const;

    /* Find the first elementus item that matches the specified tag container */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory", meta = (AutoCreateRefTerm = "IgnoreTags"))
    bool FindAllItemIndexesWithTags(const FGameplayTagContainer WithTags, TArray<int32>& OutIndexes, const FGameplayTagContainer& IgnoreTags) const;

    /* Find the first elementus item that matches the specified id */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory", meta = (AutoCreateRefTerm = "IgnoreTags"))
    bool FindAllItemIndexesWithId(const FPrimaryElementusItemId InId, TArray<int32>& OutIndexes, const FGameplayTagContainer& IgnoreTags) const;

    /* Check if the inventory stack contains a item that matches the specified info */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    bool ContainsItem(const FElementusItemInfo InItemInfo, const bool bIgnoreTags = false) const;

    /* Check if the inventory is empty */
    UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
    bool IsInventoryEmpty() const;

    /* Print debug informations in the log about this inventory */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    virtual void DebugInventory();

    /* Remove all items from this inventory */
    UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void ClearInventory();

    /* Update the current weight of this inventory */
    UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Elementus Inventory")
    void UpdateWeight();

    /* Get items from another inventory */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void GetItemIndexesFrom(UElementusInventoryComponent* OtherInventory, const TArray<int32>& ItemIndexes);

    /* Give items to another inventory */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void GiveItemIndexesTo(UElementusInventoryComponent* OtherInventory, const TArray<int32>& ItemIndexes);

    /* Get items from another inventory */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void GetItemsFrom(UElementusInventoryComponent* OtherInventory, const TArray<FElementusItemInfo>& Items);

    /* Give items to another inventory */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void GiveItemsTo(UElementusInventoryComponent* OtherInventory, const TArray<FElementusItemInfo>& Items);

    /* Discard items from this inventory */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void DiscardItemIndexes(const TArray<int32>& ItemIndexes);

    /* Discard items from this inventory */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void DiscardItems(const TArray<FElementusItemInfo>& Items);

    /* Add items to this inventory */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Elementus Inventory")
    void AddItems(const TArray<FElementusItemInfo>& Items);

    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    void SortInventory(const EElementusInventorySortingMode Mode, const EElementusInventorySortingOrientation Orientation);

protected:
    /* Items that this inventory have */
    UPROPERTY(ReplicatedUsing = OnRep_ElementusItems, EditAnywhere, BlueprintReadOnly, Category = "Elementus Inventory", meta = (Getter = "GetItemsArray", ArrayClamp = "MaxNumItems"))
    TArray<FElementusItemInfo> ElementusItems;

    /* Current weight of this inventory */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elementus Inventory", meta = (AllowPrivateAccess = "true"))
    float CurrentWeight;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void RefreshInventory();

private:
    /* Max weight allowed for this inventory */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Elementus Inventory", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
    float MaxWeight;

    /* Max num of items allowed for this inventory */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Elementus Inventory", meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
    int32 MaxNumItems;

    void ForceWeightUpdate();
    void ForceInventoryValidation();

public:
    /* Add a item to this inventory */
    void UpdateElementusItems(const TArray<FElementusItemInfo>& Modifiers, const EElementusInventoryUpdateOperation Operation);

private:
    UFUNCTION(Server, Reliable)
    void Server_ProcessInventoryAddition_Internal(const TArray<FItemModifierData>& Modifiers);

    UFUNCTION(Server, Reliable)
    void Server_ProcessInventoryRemoval_Internal(const TArray<FItemModifierData>& Modifiers);

    UFUNCTION(Category = "Elementus Inventory")
    void OnRep_ElementusItems();

protected:
    /* Mark the inventory as dirty to update the replicated data and broadcast the events */
    UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
    void NotifyInventoryChange();
};
