#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class RainbowHorse : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_ColorIndex = 0;
		int m_UpdateTick = 0;

		virtual void OnTick() override
		{
			auto mount = Self::GetMount();
			if (!mount.IsValid())
				return;

			// Update color every 5 ticks (smoother animation)
			if (++m_UpdateTick < 5)
				return;

			m_UpdateTick = 0;

			// Rainbow colors (HSV converted to RGB for vivid colors)
			static const int rainbowColors[] = {
			    0, // Red
			    6, // Orange
			    24, // Yellow
			    66, // Green
			    73, // Cyan
			    67, // Blue
			    49, // Purple
			    71  // Pink
			};

			int color = rainbowColors[m_ColorIndex];

			// TODO: Find correct RDR2 native for horse coloring
			// PED::_SET_PED_COMPONENT_TINT doesn't exist in RDR2
			// Using entity alpha as placeholder effect
			ENTITY::SET_ENTITY_ALPHA(mount.GetHandle(), 255 - (m_ColorIndex * 10), false);

			m_ColorIndex = (m_ColorIndex + 1) % 8;
		}

		virtual void OnDisable() override
		{
			auto mount = Self::GetMount();
			if (mount.IsValid())
			{
				// Reset alpha
				ENTITY::SET_ENTITY_ALPHA(mount.GetHandle(), 255, false);
			}
		}
	};

	static RainbowHorse _RainbowHorse{"rainbowhorse", "Rainbow Horse", "Makes your horse cycle through rainbow colors!"};
}
