using UnrealBuildTool;

public class IcyMoonExplorerEditorTarget : TargetRules
{
	public IcyMoonExplorerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange(new string[] { "IcyMoonExplorer" });
	}
}
