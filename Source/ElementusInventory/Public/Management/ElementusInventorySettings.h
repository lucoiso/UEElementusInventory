// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEAzSpeech

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include "ElementusInventorySettings.generated.h"

/**
 *
 */
UCLASS(Config = Plugins, DefaultConfig, meta = (DisplayName = "Elementus Inventory"))
class ELEMENTUSINVENTORY_API UElementusInventorySettings final : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    explicit UElementusInventorySettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    static const UElementusInventorySettings* Get();

    /* Will print extra internal informations in log - Useful for debugging */
    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Settings", Meta = (DisplayName = "Enable Internal Logs"))
    bool bEnableInternalLogs;

    /* Experimental parameter to assist using empty slots in the inventory: If true, will replace empty slots with empty item info */
    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Default Values | Inventory Component", Meta = (DisplayName = "Allow Empty Slots"))
    bool bAllowEmptySlots;

    /* Max weight allowed for this inventory */
    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Default Values | Inventory Component", meta = (DisplayName = "Max Weight", ClampMin = "0", UIMin = "0"))
    float MaxWeight;

    /* Max num of items allowed for this inventory */
    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Default Values | Inventory Component", meta = (DisplayName = "Max Num Items", ClampMin = "1", UIMin = "1"))
    int32 MaxNumItems;

    /* Should the inventory package auto destroy when empty? */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Values | Inventory Package", meta = (DisplayName = "Destroy When Inventory Is Empty"))
    bool bDestroyWhenInventoryIsEmpty;

protected:
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    virtual void PostInitProperties() override;

private:
    void ToggleInternalLogs();
};
