#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class FreezeTime : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			// Freeze the game clock
			CLOCK::PAUSE_CLOCK(TRUE, 0);
		}

		virtual void OnDisable() override
		{
			// Resume the game clock
			CLOCK::PAUSE_CLOCK(FALSE, 0);
		}
	};

	static FreezeTime _FreezeTime{"freezetime", "Freeze Time", "Stop time and hold perfect moments"};
}
