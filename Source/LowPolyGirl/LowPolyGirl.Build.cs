// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LowPolyGirl : ModuleRules
{
	public LowPolyGirl(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
