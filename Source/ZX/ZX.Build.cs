// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ZX : ModuleRules
{
	public ZX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CoreUObject", "Engine",										// Core
				"InputCore", "EnhancedInput", "CommonUI", "UMG", "Slate", "SlateCore",  // ui stuff
				"AIModule", "StateTreeModule", "GameplayStateTreeModule",				// AI + state trees
				"GameplayAbilities", "GameplayTasks", "GameplayTags"					// Gameplay Ability System
			});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
