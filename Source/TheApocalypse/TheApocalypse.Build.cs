// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheApocalypse : ModuleRules
{
	public TheApocalypse(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","UMG","Slate", "SlateCore" });
	}
}
