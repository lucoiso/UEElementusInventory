// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryEditor.h"
#include "ElementusInventoryEditorStyle.h"
#include "ElementusInventoryEditorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

static const FName ElementusInventoryEditorTabName("ElementusInventory");

#define LOCTEXT_NAMESPACE "FElementusInventoryEditorModule"

void FElementusInventoryEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FElementusInventoryEditorStyle::Initialize();
	FElementusInventoryEditorStyle::ReloadTextures();

	FElementusInventoryEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(FElementusInventoryEditorCommands::Get().OpenPluginWindow,
	                          FExecuteAction::CreateRaw(this, &FElementusInventoryEditorModule::PluginButtonClicked),
	                          FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,
		&FElementusInventoryEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ElementusInventoryEditorTabName,
	                                                  FOnSpawnTab::CreateRaw(this,
	                                                                         &FElementusInventoryEditorModule::OnSpawnPluginTab)
	                        )
	                        .SetDisplayName(LOCTEXT("FElementusInventoryEditorTabTitle", "ElementusInventory"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FElementusInventoryEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FElementusInventoryEditorStyle::Shutdown();
	FElementusInventoryEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ElementusInventoryEditorTabName);
}

TSharedRef<SDockTab> FElementusInventoryEditorModule::OnSpawnPluginTab(
	[[maybe_unused]] const FSpawnTabArgs& SpawnTabArgs) const
{
	const FText WidgetText = LOCTEXT("WindowWidgetText", "Work in Progress");

	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FElementusInventoryEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ElementusInventoryEditorTabName);
}

void FElementusInventoryEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	const TSharedPtr<FWorkspaceItem> Menu = WorkspaceMenu::GetMenuStructure().GetToolsCategory()->AddGroup(
		LOCTEXT("ElementusCategory", "Elementus"),
		LOCTEXT("ElementusCategoryTooltipText", "Elementus Plugins Tabs."),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "InputBindingEditor.LevelViewport"));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName(TEXT("ElementusInventoryEditor")),
	                                                  FOnSpawnTab::CreateRaw(this,
	                                                                         &FElementusInventoryEditorModule::OnSpawnPluginTab))
	                        .SetDisplayName(NSLOCTEXT("ElementusInventoryEditorModule",
	                                                  "ElementusInventoryTitle",
	                                                  "Elementus Inventory"))
	                        .SetTooltipText(NSLOCTEXT("ElementusInventoryEditorModule",
	                                                  "ElementusInventoryTooltipText",
	                                                  "Open ElementusInventory Editor Tab."))
	                        .SetGroup(Menu.ToSharedRef())
	                        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "MainFrame.PackageProject"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FElementusInventoryEditorModule, ElementusInventoryEditor)
