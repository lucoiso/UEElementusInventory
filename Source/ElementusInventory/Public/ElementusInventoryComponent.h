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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElementusInventoryUpdate,
                                             const FElementusItemInfo, Modifier,
                                             const EElementusInventoryUpdateOperation, Operation);

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
	FElementusInventoryEmpty OnInventoryEmpty;

	/* Get the items that this inventory have */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	TArray<FElementusItemInfo> GetItemStack() const;

	/* Add a item to this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void AddElementusItem(const FElementusItemInfo& AddInfo);

	/* Remove a item from this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void RemoveElementusItem(const FElementusItemInfo& RemoveInfo);

	/* Print debug informations in the log about this inventory */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	virtual void DebugInventoryStack();

	/* Check if this inventory can receive the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanReceiveItem(const FElementusItemInfo& InItemInfo) const;

	/* Check if this inventory can give the passed item */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	virtual bool CanGiveItem(const FElementusItemInfo& InItemInfo);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool FindElementusItemInStack(const FElementusItemInfo InItemInfo, int32& OutIndex) const;

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool ContainItemInStack(const FElementusItemInfo InItemInfo) const;

protected:
	/* Items that this inventory have (UInventoryItemData -> Quantity) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (Getter = "GetItemStack"))
	TArray<FElementusItemInfo> ItemStack;

	/* Current weight of this inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (AllowPrivateAccess = "true"))
	float CurrentWeight;

	virtual void BeginPlay() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void AddElementusItem_Internal(const FElementusItemInfo& AddInfo);

	UFUNCTION(NetMulticast, Reliable)
	void RemoveElementusItem_Internal(const FElementusItemInfo& RemoveInfo);

	void NotifyInventoryChange(const FElementusItemInfo& Modifier,
	                           const EElementusInventoryUpdateOperation Operation);

	void UpdateCurrentWeight();
};
