#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"
#include <cmath>

namespace YimMenu::Features
{
	enum class SkyColor
	{
		NORMAL,
		PURPLE,
		PINK,
		GREEN,
		ORANGE,
		RED,
		ALIEN_GREEN,
		DEEP_BLUE,
		RAINBOW
	};

	static const std::vector<std::pair<int, const char*>> g_SkyColors = {
		{(int)SkyColor::NORMAL, "Normal"},
		{(int)SkyColor::PURPLE, "Purple Dream"},
		{(int)SkyColor::PINK, "Cotton Candy Pink"},
		{(int)SkyColor::GREEN, "Emerald Green"},
		{(int)SkyColor::ORANGE, "Eternal Sunset"},
		{(int)SkyColor::RED, "Mars Red"},
		{(int)SkyColor::ALIEN_GREEN, "Alien World"},
		{(int)SkyColor::DEEP_BLUE, "Deep Ocean Blue"},
		{(int)SkyColor::RAINBOW, "Rainbow Cycle"}
	};

	static ListCommand _SkyColorType{"skycolortype", "Sky Color", "Paint the sky any color!", g_SkyColors, (int)SkyColor::NORMAL};

	class SkyColorChanger : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline int rainbowTick = 0;

		const char* GetTimecycleModifier(SkyColor color)
		{
			// RDR2 uses timecycle modifiers for sky color changes
			switch (color)
			{
			case SkyColor::PURPLE:
				return "MP_SleepFade"; // Purple/blue tint
			case SkyColor::PINK:
				return "RespawnPulse02"; // Pink ethereal glow
			case SkyColor::GREEN:
				return "InvasionOfAmerica"; // Green tint
			case SkyColor::ORANGE:
				return "MP_Deathmatch_night"; // Warm orange sunset
			case SkyColor::RED:
				return "MP_ArenaWheelLose"; // Red tint
			case SkyColor::ALIEN_GREEN:
				return "MP_FrozenCarbon_night"; // Alien atmosphere
			case SkyColor::DEEP_BLUE:
				return "MP_FrozenCarbon"; // Deep blue
			case SkyColor::RAINBOW:
				// Cycle through different modifiers for rainbow effect
				rainbowTick = (rainbowTick + 1) % 300;
				if (rainbowTick < 50) return "MP_Deathmatch_night"; // Orange
				else if (rainbowTick < 100) return "RespawnPulse02"; // Pink
				else if (rainbowTick < 150) return "MP_FrozenCarbon"; // Blue
				else if (rainbowTick < 200) return "InvasionOfAmerica"; // Green
				else if (rainbowTick < 250) return "MP_ArenaWheelLose"; // Red
				else return "GoldenRatio02"; // Gold
			default:
				return nullptr;
			}
		}

		virtual void OnTick() override
		{
			auto color = (SkyColor)_SkyColorType.GetState();
			const char* modifier = GetTimecycleModifier(color);

			if (modifier)
			{
				GRAPHICS::SET_TIMECYCLE_MODIFIER(modifier);
				GRAPHICS::SET_TIMECYCLE_MODIFIER_STRENGTH(0.8f);
			}
			else
			{
				GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
			}
		}

		virtual void OnDisable() override
		{
			GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
			rainbowTick = 0;
		}
	};

	static SkyColorChanger _SkyColorChanger{"skycolorchanger", "Sky Color Changer", "Paint the sky incredible colors!"};
}
