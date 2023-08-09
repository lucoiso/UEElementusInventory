// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

using UnrealBuildTool;

public class ElementusInventoryEditor : ModuleRules
{
    public ElementusInventoryEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "ElementusInventory",
            "InputCore",
            "CoreUObject",
            "UnrealEd",
            "AssetTools",
            "ToolMenus",
            "Engine",
            "Slate",
            "SlateCore",
            "EditorStyle",
            "WorkspaceMenuStructure",
            "PropertyEditor",
            "GameplayTags"
        });
    }
}