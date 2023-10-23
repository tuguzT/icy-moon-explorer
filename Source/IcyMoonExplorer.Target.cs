using UnrealBuildTool;

public class IcyMoonExplorerTarget : TargetRules
{
	public IcyMoonExplorerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange(new string[] { "IcyMoonExplorer" });
	}
}
