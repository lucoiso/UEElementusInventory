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
	Add,
	Remove
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FElementusInventoryUpdate,
                                               const FElementusItemId, ItemId,
                                               const int32, NewQuantity,
                                               const EElementusInventoryUpdateOperation, Operation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElementusInventoryItemAdded,
                                             const FElementusItemId, ItemId,
                                             const int32, AddedQuantity);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElementusInventoryItemRemoved,
                                             const FElementusItemId, ItemId,
                                             const int32, RemovedQuantity);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElementusInventoryEmpty);

UCLASS(Blueprintable, ClassGroup=(Custom), Category = "Elementus Inventory | Classes",
	meta = (BlueprintSpawnableComponent))
class ELEMENTUSINVENTORY_API UElementusInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit UElementusInventoryComponent(const FObjectInitializer& ObjectInitializer);

	/* Get the current inventory weight */
	float GetCurrentWeight() const;

	/* Max weight allowed for this inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elementus Inventory")
	float MaxWeight;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryUpdate OnInventoryUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Elementus Inventory")
	FElementusInventoryEmpty OnInventoryEmpty;

	/* Get the items that this inventory have */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	TMap<FElementusItemId, int32> GetItemStack() const;

	/* Add a item to this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void AddElementusItem(const FElementusItemId& ItemId, const int32 Quantity);

	/* Remove a item from this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void DiscardElementusItem(const FElementusItemId& ItemId, const int32 Quantity);

	/* Print debug informations in the log about this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	virtual void DebugInventoryStack();

	/* Check if this inventory can receive the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanReceiveItem(const FElementusItemId& ItemId, const int32 Quantity) const;

	/* Check if this inventory can give the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanGiveItem(const FElementusItemId& ItemId, const int32 Quantity) const;

protected:
	/* Items that this inventory have (UInventoryItemData -> Quantity) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (Getter = "GetItemStack"))
	TMap<FElementusItemId, int32> ItemStack;
	
	/* Current weight of this inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AllowPrivateAccess = "true"))
	float CurrentWeight;

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UFUNCTION(NetMulticast, Reliable)
	void AddElementusItem_Internal(const FElementusItemId& ItemId,
	                               const int32 Quantity);

	UFUNCTION(NetMulticast, Reliable)
	void DiscardElementusItem_Internal(const FElementusItemId& ItemId,
	                                   const int32 Quantity);

	void NotifyInventoryChange(const FElementusItemId& ItemId,
	                           const int32 Quantity,
	                           const EElementusInventoryUpdateOperation Operation);

	void UpdateCurrentWeight();
	void ValidateItemStack();
};
