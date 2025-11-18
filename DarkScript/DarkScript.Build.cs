// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DarkScript : ModuleRules
{
	public DarkScript(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EnhancedInput",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"OnlineSubsystemNull",
				"OnlineSubsystemSteam",
				"NetCore",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"GameplayTags",
				"UnrealEd",
				"InputCore",
				"Kismet",
				"EditorSubsystem",
				"EditorStyle",
				"PropertyEditor",
				"Blutility",
				"UMG"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
