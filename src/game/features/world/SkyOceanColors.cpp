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

	static const std::pair<SkyColor, const char*> g_SkyColors[] = {
		{SkyColor::NORMAL, "Normal"},
		{SkyColor::PURPLE, "Purple Dream"},
		{SkyColor::PINK, "Cotton Candy Pink"},
		{SkyColor::GREEN, "Emerald Green"},
		{SkyColor::ORANGE, "Eternal Sunset"},
		{SkyColor::RED, "Mars Red"},
		{SkyColor::ALIEN_GREEN, "Alien World"},
		{SkyColor::DEEP_BLUE, "Deep Ocean Blue"},
		{SkyColor::RAINBOW, "Rainbow Cycle"}
	};

	static ListCommand _SkyColorType{"skycolortype", "Sky Color", "Paint the sky any color!", g_SkyColors, (int)SkyColor::NORMAL};

	class SkyColorChanger : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline int rainbowHue = 0;

		void ApplySkyColor(SkyColor color)
		{
			switch (color)
			{
			case SkyColor::PURPLE:
				GRAPHICS::_SET_CLOUD_SETTINGS(0.5f, 0.2f, 0.8f, 0.5f); // Purple tint
				break;
			case SkyColor::PINK:
				GRAPHICS::_SET_CLOUD_SETTINGS(1.0f, 0.4f, 0.7f, 0.5f); // Pink
				break;
			case SkyColor::GREEN:
				GRAPHICS::_SET_CLOUD_SETTINGS(0.3f, 0.8f, 0.3f, 0.5f); // Green
				break;
			case SkyColor::ORANGE:
				GRAPHICS::_SET_CLOUD_SETTINGS(1.0f, 0.6f, 0.2f, 0.5f); // Orange sunset
				break;
			case SkyColor::RED:
				GRAPHICS::_SET_CLOUD_SETTINGS(0.9f, 0.2f, 0.2f, 0.5f); // Red/Mars
				break;
			case SkyColor::ALIEN_GREEN:
				GRAPHICS::_SET_CLOUD_SETTINGS(0.5f, 1.0f, 0.3f, 0.5f); // Alien green
				break;
			case SkyColor::DEEP_BLUE:
				GRAPHICS::_SET_CLOUD_SETTINGS(0.2f, 0.4f, 1.0f, 0.5f); // Deep blue
				break;
			case SkyColor::RAINBOW:
				// Cycle through rainbow
				rainbowHue = (rainbowHue + 1) % 360;
				float r, g, b;
				// Simple HSV to RGB conversion
				float h = rainbowHue / 60.0f;
				float x = 1.0f - abs(fmod(h, 2.0f) - 1.0f);
				if (h < 1) { r = 1; g = x; b = 0; }
				else if (h < 2) { r = x; g = 1; b = 0; }
				else if (h < 3) { r = 0; g = 1; b = x; }
				else if (h < 4) { r = 0; g = x; b = 1; }
				else if (h < 5) { r = x; g = 0; b = 1; }
				else { r = 1; g = 0; b = x; }
				GRAPHICS::_SET_CLOUD_SETTINGS(r, g, b, 0.5f);
				break;
			default:
				GRAPHICS::_RESET_CLOUD_SETTINGS();
				break;
			}
		}

		virtual void OnTick() override
		{
			auto color = (SkyColor)_SkyColorType.GetState();
			ApplySkyColor(color);
		}

		virtual void OnDisable() override
		{
			GRAPHICS::_RESET_CLOUD_SETTINGS();
			rainbowHue = 0;
		}
	};

	static SkyColorChanger _SkyColorChanger{"skycolorchanger", "Sky Color Changer", "Paint the sky incredible colors!"};
}
