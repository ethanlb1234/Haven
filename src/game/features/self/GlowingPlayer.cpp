#include "core/commands/BoolCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class GlowingPlayer : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		virtual void OnEnable() override
		{
			auto ped = Self::GetPed();
			if (ped.IsValid())
			{
				// Add glowing light effect to player
				GRAPHICS::_SET_ENTITY_LIGHT(ped.GetHandle(), true, 255, 150, 50, 10.0f, 100.0f);
			}
		}

		virtual void OnDisable() override
		{
			auto ped = Self::GetPed();
			if (ped.IsValid())
			{
				GRAPHICS::_SET_ENTITY_LIGHT(ped.GetHandle(), false, 0, 0, 0, 0.0f, 0.0f);
			}
		}
	};

	static GlowingPlayer _GlowingPlayer{"glowingplayer", "Glowing Player", "Makes you emit a warm golden glow"};
}
