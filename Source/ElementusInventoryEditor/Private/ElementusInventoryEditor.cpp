// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryEditor.h"

#include "ElementusDetailsPanel.h"
#include "ElementusStaticIds.h"
#include "SElementusFrame.h"
#include "SElementusItemCreator.h"
#include "Widgets/Docking/SDockTab.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "FElementusInventoryEditorModule"

void FElementusInventoryEditorModule::StartupModule()
{
	const auto& RegisterDelegate =
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FElementusInventoryEditorModule::RegisterMenus);

	UToolMenus::RegisterStartupCallback(RegisterDelegate);

	const auto& MakeInstanceDelegate =
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&ElementusDetailsPanel::MakeInstance);

	PropertyEditorModule = &FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule->RegisterCustomPropertyTypeLayout(ItemStackPropertyId, MakeInstanceDelegate);
}

void FElementusInventoryEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ElementusEditorTabId);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ItemCreatorTabId);

	PropertyEditorModule->UnregisterCustomPropertyTypeLayout(ItemStackPropertyId);
}

TSharedRef<SDockTab> FElementusInventoryEditorModule::OnSpawnTab([[maybe_unused]] const FSpawnTabArgs&,
                                                                 const FName TabId) const
{
	TSharedPtr<SWidget> OutContent;

	if (TabId == ElementusEditorTabId)
	{
		OutContent = SNew(SElementusFrame);
	}
	else if (TabId == ItemCreatorTabId)
	{
		OutContent = SNew(SElementusItemCreator);
	}

	if (OutContent.IsValid())
	{
		return SNew(SDockTab)
			.TabRole(NomadTab)
			[
				OutContent.ToSharedRef()
			];
	}

	return SNew(SDockTab);
}

void FElementusInventoryEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	const TSharedPtr<FWorkspaceItem> Menu = WorkspaceMenu::GetMenuStructure().GetToolsCategory()->AddGroup(
		NSLOCTEXT(LOCTEXT_NAMESPACE, "ElementusCategory", "Elementus"),
		NSLOCTEXT(LOCTEXT_NAMESPACE, "ElementusCategoryTooltip", "Elementus Plugins Tabs"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "InputBindingEditor.LevelViewport"));

	const auto& EditorTabSpawnerDelegate =
		FOnSpawnTab::CreateRaw(this, &FElementusInventoryEditorModule::OnSpawnTab, ElementusEditorTabId);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ElementusEditorTabId, EditorTabSpawnerDelegate)
	                        .SetDisplayName(FText::FromString("Elementus Inventory"))
	                        .SetTooltipText(FText::FromString("Open Elementus Inventory Window"))
	                        .SetGroup(Menu.ToSharedRef())
	                        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.Package"));


	const auto& ItemCreatorTabSpawnerDelegate =
		FOnSpawnTab::CreateRaw(this, &FElementusInventoryEditorModule::OnSpawnTab, ItemCreatorTabId);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ItemCreatorTabId, ItemCreatorTabSpawnerDelegate)
	                        .SetDisplayName(FText::FromString("Elementus Item Creator"))
	                        .SetGroup(Menu.ToSharedRef())
	                        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.PlusCircle"));
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FElementusInventoryEditorModule, ElementusInventoryEditor)
