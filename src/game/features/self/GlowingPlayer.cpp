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
				// Enable entity lights for glowing effect
				ENTITY::_SET_ENTITY_LIGHTS_ENABLED(ped.GetHandle(), true);
			}
		}

		virtual void OnDisable() override
		{
			auto ped = Self::GetPed();
			if (ped.IsValid())
			{
				// Disable entity lights
				ENTITY::_SET_ENTITY_LIGHTS_ENABLED(ped.GetHandle(), false);
			}
		}
	};

	static GlowingPlayer _GlowingPlayer{"glowingplayer", "Glowing Player", "Makes you emit a warm golden glow"};
}
