// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "SElementusFrame.h"

static const FName ElementusInventoryEditorTabName("Elementus Inventory");
#define LOCTEXT_NAMESPACE "FElementusInventoryEditorModule"

void FElementusInventoryEditorModule::StartupModule()
{
	const auto& RegisterDelegate =
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FElementusInventoryEditorModule::RegisterMenus);

	UToolMenus::RegisterStartupCallback(RegisterDelegate);
}

void FElementusInventoryEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ElementusInventoryEditorTabName);
}

TSharedRef<SDockTab> FElementusInventoryEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs) const
{
	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			SNew(SElementusFrame)
		];
}

void FElementusInventoryEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	const TSharedPtr<FWorkspaceItem> Menu = WorkspaceMenu::GetMenuStructure().GetToolsCategory()->AddGroup(
		NSLOCTEXT(LOCTEXT_NAMESPACE, "ElementusCategory", "Elementus"),
		NSLOCTEXT(LOCTEXT_NAMESPACE, "ElementusCategoryTooltip", "Elementus Plugins Tabs"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "InputBindingEditor.LevelViewport"));

	const auto& TabSpawnerDelegate =
		FOnSpawnTab::CreateRaw(this, &FElementusInventoryEditorModule::OnSpawnPluginTab);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ElementusInventoryEditorTabName, TabSpawnerDelegate)
	                        .SetDisplayName(NSLOCTEXT(LOCTEXT_NAMESPACE,
	                                                  "ElementusInventoryTitle",
	                                                  "Elementus Inventory"))
	                        .SetTooltipText(NSLOCTEXT(LOCTEXT_NAMESPACE,
	                                                  "ElementusInventoryTooltip",
	                                                  "Open Elementus Inventory Window."))
	                        .SetGroup(Menu.ToSharedRef())
	                        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "MainFrame.PackageProject"));
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FElementusInventoryEditorModule, ElementusInventoryEditor)
