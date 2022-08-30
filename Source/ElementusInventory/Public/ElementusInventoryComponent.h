// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "Components/ActorComponent.h"
#include "ElementusInventoryComponent.generated.h"

UENUM(BlueprintType, Category = "Elementus Inventory | Enumerations")
enum class EElementusInventoryUpdateOperation : uint8
{
	None,
	Add,
	Remove
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElementusInventoryUpdate,
                                             const FElementusItemInfo, Modifier,
                                             const EElementusInventoryUpdateOperation, Operation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElementusInventoryEmpty);

UCLASS(Blueprintable, ClassGroup=(Custom), Category = "Elementus Inventory | Classes", meta = (BlueprintSpawnableComponent))
class ELEMENTUSINVENTORY_API UElementusInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit UElementusInventoryComponent(const FObjectInitializer& ObjectInitializer);

	/* Get the current inventory weight */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	float GetCurrentWeight() const;

	/* Max weight allowed for this inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
	float MaxWeight;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryUpdate OnInventoryUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryEmpty OnInventoryEmpty;

	/* Get the items that this inventory have */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	TArray<FElementusItemInfo> GetItemsArray() const;

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	FElementusItemInfo& GetItemReferenceAt(const int32 Index);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	FElementusItemInfo GetItemCopyAt(const int32 Index) const;

	/* Check if this inventory can receive the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanReceiveItem(const FElementusItemInfo InItemInfo) const;

	/* Check if this inventory can give the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanGiveItem(const FElementusItemInfo InItemInfo) const;

	/* Find the first elementus item that matches the specified info */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindFirstItemIndexWithInfo(const FElementusItemInfo InItemInfo,
									int32& OutIndex,
									const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;

	/* Find the first elementus item that matches the specified tag container */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindFirstItemIndexWithTags(const FGameplayTagContainer WithTags,
									int32& OutIndex,
									const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;

	/* Find the first elementus item that matches the specified id */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindFirstItemIndexWithId(const FPrimaryElementusItemId InId,
								  int32& OutIndex,
								  const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;
	

	/* Find the first elementus item that matches the specified info */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindAllItemIndexesWithInfo(const FElementusItemInfo InItemInfo,
								  	TArray<int32>& OutIndexes,
								  	const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;

	/* Find the first elementus item that matches the specified tag container */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindAllItemIndexesWithTags(const FGameplayTagContainer WithTags,
									TArray<int32>& OutIndexes,
									const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;

	/* Find the first elementus item that matches the specified id */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindAllItemIndexesWithId(const FPrimaryElementusItemId InId,
								  TArray<int32>& OutIndexes,
								  const FGameplayTagContainer IgnoreTags = FGameplayTagContainer()) const;
	
	/* Check if the inventory stack contains a item that matches the specified info */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool ContainsItem(const FElementusItemInfo InItemInfo) const;

	/* Print debug informations in the log about this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	virtual void DebugInventory();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Elementus Inventory")
	void ClearInventory();

protected:
	/* Items that this inventory have */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (Getter = "GetItemsArray"))
	TArray<FElementusItemInfo> ElementusItems;

	/* Current weight of this inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AllowPrivateAccess = "true"))
	float CurrentWeight;

	virtual void BeginPlay() override;
	
	virtual void RefreshInventory();

private:
	void ForceWeightUpdate();
	void ForceInventoryValidation();

public:	
	/* Add a item to this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void AddElementusItem(const FElementusItemInfo& InModifier);

	/* Remove a item from this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void RemoveElementusItem(const FElementusItemInfo& InModifier);
	
private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ProcessSingleItemUpdate(const FElementusItemInfo& Modifier,
										   const EElementusInventoryUpdateOperation Operation,
										   const uint8 InIndex = INDEX_NONE,
										   const bool bAddNewOrRemoveAll = false,
										   const bool bNotify = true);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ProcessGroupItemUpdate(const FElementusItemInfo& Modifier,
										  const EElementusInventoryUpdateOperation Operation,
										  const TArray<FElementusItemInfo>& InGroup,
										  const bool bNotify = true);

	void NotifyInventoryChange(const FElementusItemInfo& Modifier,
	                           const EElementusInventoryUpdateOperation Operation);
};
