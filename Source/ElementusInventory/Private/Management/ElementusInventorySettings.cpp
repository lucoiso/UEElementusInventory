// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEAzSpeech

#include "Management/ElementusInventorySettings.h"
#include "LogElementusInventory.h"

#ifdef UE_INLINE_GENERATED_CPP_BY_NAME
#include UE_INLINE_GENERATED_CPP_BY_NAME(ElementusInventorySettings)
#endif

UElementusInventorySettings::UElementusInventorySettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bEnableInternalLogs(false)
{
    CategoryName = TEXT("Plugins");
}

const UElementusInventorySettings* UElementusInventorySettings::Get()
{
    static const UElementusInventorySettings* const Instance = GetDefault<UElementusInventorySettings>();
    return Instance;
}

#if WITH_EDITOR
void UElementusInventorySettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UElementusInventorySettings, bEnableInternalLogs))
    {
        ToggleInternalLogs();
    }
}
#endif

void UElementusInventorySettings::PostInitProperties()
{
    Super::PostInitProperties();

    ToggleInternalLogs();
}

void UElementusInventorySettings::ToggleInternalLogs()
{
#if !UE_BUILD_SHIPPING
    LogElementusInventory_Internal.SetVerbosity(bEnableInternalLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
#endif
}