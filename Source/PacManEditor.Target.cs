// Original Author: Luccas de Avila Machado.
// Hope you can appreciate and learn from this code.

using UnrealBuildTool;
using System.Collections.Generic;

public class PacManEditorTarget : TargetRules
{
	public PacManEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "PacMan" } );
	}
}
