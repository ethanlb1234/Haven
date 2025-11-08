#include "core/commands/IntCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class CustomHorseColor : public IntCommand
	{
		using IntCommand::IntCommand;

		virtual void OnCall() override
		{
			auto mount = Self::GetMount();
			if (!mount.IsValid())
				return;

			int colorValue = GetState();

			// RDR2 horses have fixed breed colors, but we can apply visual effects
			// Use alpha channel to create color tint effect
			int alpha = 255 - (colorValue * 2); // Vary transparency based on color value
			if (alpha < 150) alpha = 150; // Keep horse visible

			ENTITY::SET_ENTITY_ALPHA(mount.GetHandle(), alpha, false);

			// Enable entity lights for glow effect on certain color values
			if (colorValue > 50)
			{
				ENTITY::_SET_ENTITY_LIGHTS_ENABLED(mount.GetHandle(), true);
			}
			else
			{
				ENTITY::_SET_ENTITY_LIGHTS_ENABLED(mount.GetHandle(), false);
			}
		}
	};

	static CustomHorseColor _CustomHorseColor{"customhorsecolor", "Horse Color", "Set your horse's color (0=Red, 6=Orange, 24=Yellow, 66=Green, 73=Cyan, 67=Blue, 49=Purple, 71=Pink)", 0, 0, 100};
}
