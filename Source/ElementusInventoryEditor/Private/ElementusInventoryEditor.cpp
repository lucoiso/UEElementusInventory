// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryEditor.h"
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
}

void FElementusInventoryEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ElementusEditorTabId);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ItemCreatorTabId);
}

void FElementusInventoryEditorModule::RegisterMenus()
{
	const auto& TabCreator_Lambda =
		[&]([[maybe_unused]] const FSpawnTabArgs& SpawnTabArgs,
		    const TSharedRef<SWidget>& InContent) -> TSharedRef<SDockTab>
	{
		return SNew(SDockTab)
			.TabRole(NomadTab)
			[
				InContent
			];
	};

	FToolMenuOwnerScoped OwnerScoped(this);

	const TSharedPtr<FWorkspaceItem> Menu = WorkspaceMenu::GetMenuStructure().GetToolsCategory()->AddGroup(
		NSLOCTEXT(LOCTEXT_NAMESPACE, "ElementusCategory", "Elementus"),
		NSLOCTEXT(LOCTEXT_NAMESPACE, "ElementusCategoryTooltip", "Elementus Plugins Tabs"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "InputBindingEditor.LevelViewport"));

	const auto& EditorTabSpawnerDelegate =
		FOnSpawnTab::CreateLambda(TabCreator_Lambda, SNew(SElementusFrame));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ElementusEditorTabId, EditorTabSpawnerDelegate)
	                        .SetDisplayName(FText::FromString("Elementus Inventory"))
	                        .SetTooltipText(FText::FromString("Open Elementus Inventory Window"))
	                        .SetGroup(Menu.ToSharedRef())
	                        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.Package"));


	const auto& ItemCreatorTabSpawnerDelegate =
		FOnSpawnTab::CreateLambda(TabCreator_Lambda, SNew(SElementusItemCreator));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ItemCreatorTabId, ItemCreatorTabSpawnerDelegate)
	                        .SetDisplayName(FText::FromString("Elementus Item Creator"))
	                        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.PlusCircle"));
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FElementusInventoryEditorModule, ElementusInventoryEditor)
