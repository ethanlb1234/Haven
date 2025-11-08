#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	enum class GravityMode
	{
		NORMAL,
		LOW,
		VERY_LOW,
		MOON,
		ZERO,
		REVERSE,
		HEAVY
	};

	static const std::pair<GravityMode, const char*> g_GravityModes[] = {
		{GravityMode::NORMAL, "Normal (1.0x)"},
		{GravityMode::LOW, "Low (0.5x)"},
		{GravityMode::VERY_LOW, "Very Low (0.25x)"},
		{GravityMode::MOON, "Moon (0.16x)"},
		{GravityMode::ZERO, "Zero Gravity (0.0x)"},
		{GravityMode::REVERSE, "Reverse (-1.0x)"},
		{GravityMode::HEAVY, "Heavy (2.0x)"}
	};

	static ListCommand _GravityMode{"gravitymode", "Gravity Mode", "Change world gravity", g_GravityModes, (int)GravityMode::NORMAL};

	class GravityModifier : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		float GetGravityLevel(GravityMode mode)
		{
			switch (mode)
			{
			case GravityMode::NORMAL:
				return 1.0f;
			case GravityMode::LOW:
				return 0.5f;
			case GravityMode::VERY_LOW:
				return 0.25f;
			case GravityMode::MOON:
				return 0.166f; // ~1/6 Earth gravity
			case GravityMode::ZERO:
				return 0.0f;
			case GravityMode::REVERSE:
				return -1.0f; // Fall upwards!
			case GravityMode::HEAVY:
				return 2.0f;
			default:
				return 1.0f;
			}
		}

		virtual void OnTick() override
		{
			auto mode = (GravityMode)_GravityMode.GetState();
			float gravityLevel = GetGravityLevel(mode);

			// Apply gravity modification
			MISC::SET_GRAVITY_LEVEL(gravityLevel);
		}

		virtual void OnDisable() override
		{
			// Restore normal gravity
			MISC::SET_GRAVITY_LEVEL(1.0f);
		}
	};

	static GravityModifier _GravityModifier{"gravitymodifier", "Gravity Modifier", "Experience different gravity levels"};
}
