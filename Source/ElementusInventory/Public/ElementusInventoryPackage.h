// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryData.h"
#include "GameFramework/Actor.h"
#include "ElementusInventoryPackage.generated.h"

struct FElementusItemId;
class UElementusInventoryComponent;

UCLASS(Category = "Elementus Inventory | Classes")
class ELEMENTUSINVENTORY_API AElementusInventoryPackage : public AActor
{
	GENERATED_BODY()

public:
	explicit AElementusInventoryPackage(const FObjectInitializer& ObjectInitializer);

	/* The inventory of this package actor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	TObjectPtr<UElementusInventoryComponent> PackageInventory;

	/* Put a item in this package */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void PutItemIntoPackage(TArray<FElementusItemInfo>& ItemInfo,
	                        UElementusInventoryComponent* FromInventory) const;

	/* Get a item from this package */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void GetItemFromPackage(TArray<FElementusItemInfo>& ItemInfo,
	                        UElementusInventoryComponent* ToInventory) const;

	/* Set this package to auto destroy when its empty */
	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void SetDestroyOnEmpty(const bool bDestroy);

	/* Will this package auto destroy when empty? */
	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool GetDestroyOnEmpty() const;

protected:
	/* Will this package auto destroy when empty? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (Getter = "GetDestroyOnEmpty", Setter = "SetDestroyOnEmpty"))
	bool bDestroyWhenInventoryIsEmpty;

	virtual void BeginPlay() override;

	/* Destroy this package (Call Destroy()) */
	UFUNCTION(BlueprintNativeEvent, Category = "Elementus Inventory")
	void BeginPackageDestruction();
};
