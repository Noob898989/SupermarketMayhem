// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SupermarketMayhem : ModuleRules
{
	public SupermarketMayhem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"SupermarketMayhem",
			"SupermarketMayhem/Variant_Horror",
			"SupermarketMayhem/Variant_Horror/UI",
			"SupermarketMayhem/Variant_Shooter",
			"SupermarketMayhem/Variant_Shooter/AI",
			"SupermarketMayhem/Variant_Shooter/UI",
			"SupermarketMayhem/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
