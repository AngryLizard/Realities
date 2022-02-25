// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class RealitiesTarget : TargetRules
{
	public RealitiesTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Game;

        ExtraModuleNames.AddRange(new string[] { "Realities"}); // , "RealitiesExtension"
        DefaultBuildSettings = BuildSettingsVersion.V2;
    }
}
