// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MediapipeProject : ModuleRules
{
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
	}
	public MediapipeProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore", "HeadMountedDisplay"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		PrivateDependencyModuleNames.AddRange(new string[] { "RHI", "RenderCore" });

		string JsonPath = Path.Combine(ThirdPartyPath, "nlohmann");
		PublicIncludePaths.AddRange(new string[] { Path.Combine(JsonPath, "") });

		string OpenCVPath = Path.Combine(ThirdPartyPath, "OpenCV");
		PublicIncludePaths.AddRange(new string[] { Path.Combine(OpenCVPath, "Includes") });
		PublicLibraryPaths.Add(Path.Combine(OpenCVPath, "Libraries/Win64"));

		//RuntimeDependencies.Add(Path.Combine(PluginDirectory, "Binaries/Win64/opencv_world320.dll"));
		//RuntimeDependencies.Add(Path.Combine(PluginDirectory, "Binaries/Win64/opencv_ffmpeg320_64.dll"));

		//string EigenPath = Path.Combine(ThirdPartyPath, "eigen-3.3.7");
		//PublicIncludePaths.AddRange(new string[] { Path.Combine(EigenPath, "Eigen") });

		//Add Static Libraries
		PublicAdditionalLibraries.Add("opencv_world320.lib");

		//Add Dynamic Libraries
		PublicDelayLoadDLLs.Add("opencv_world320.dll");
		PublicDelayLoadDLLs.Add("opencv_ffmpeg320_64.dll");

		bEnableUndefinedIdentifierWarnings = false;

		//DynamicTexture = UTexture2D::CreateTransient(SizeX, SizeY);
		// Allocate the texture HRI
		//DynamicTexture->UpdateResource();


		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
