#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _PlayerScaleValue{"playerscalevalue", "Player Scale", "How big you are (0.2 = tiny, 1.0 = normal, 5.0 = giant)", 0.2f, 5.0f, 1.0f};

	class PlayerScale : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (ped.IsValid())
			{
				PED::_SET_PED_SCALE(ped.GetHandle(), _PlayerScaleValue.GetState());
			}
		}

		virtual void OnDisable() override
		{
			auto ped = Self::GetPed();
			if (ped.IsValid())
			{
				PED::_SET_PED_SCALE(ped.GetHandle(), 1.0f); // Restore normal size
			}
		}
	};

	static PlayerScale _PlayerScale{"playerscale", "Player Scale", "Resize yourself! See the world from new perspectives"};
}
