// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjetProComp_M1 : ModuleRules
{
	public ProjetProComp_M1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara" });
	}
}
