// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElementusInventoryData.h"
#include "ElementusInventoryPackage.generated.h"

class UElementusInventoryComponent;

UCLASS(Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORY_API AElementusInventoryPackage : public AActor
{
	GENERATED_BODY()

public:
	explicit AElementusInventoryPackage(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	TObjectPtr<UElementusInventoryComponent> PackageInventory;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void PutItemIntoPackage(TMap<FPrimaryAssetId, int32>& ItemInfo,
	                        UElementusInventoryComponent* FromInventory) const;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void GetItemFromPackage(TMap<FPrimaryAssetId, int32>& ItemInfo,
	                        UElementusInventoryComponent* ToInventory) const;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void SetDestroyOnEmpty(const bool bDestroy);

	UFUNCTION(BlueprintPure, Category = "Elementus Inventory")
	bool GetDestroyOnEmpty() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory",
		meta = (Getter = "GetDestroyOnEmpty", Setter = "SetDestroyOnEmpty"))
	bool bDestroyWhenInventoryIsEmpty;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Elementus Inventory")
	void BeginPackageDestruction();
};
