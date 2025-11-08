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
			// TODO: RDR2 doesn't have _SET_PED_COMPONENT_TINT native - need horse-specific color system
			/*
			auto mount = Self::GetMount();
			if (!mount.IsValid())
				return;

			int color = GetState();

			// Apply the selected color to horse body parts
			PED::_SET_PED_COMPONENT_TINT(mount.GetHandle(), 0, color); // Body
			PED::_SET_PED_COMPONENT_TINT(mount.GetHandle(), 5, color); // Mane
			PED::_SET_PED_COMPONENT_TINT(mount.GetHandle(), 6, color); // Tail
			*/
		}
	};

	static CustomHorseColor _CustomHorseColor{"customhorsecolor", "Horse Color", "Set your horse's color (0=Red, 6=Orange, 24=Yellow, 66=Green, 73=Cyan, 67=Blue, 49=Purple, 71=Pink)", 0, 0, 100};
}
