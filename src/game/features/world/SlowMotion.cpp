#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _SlowMotionSpeed{"slowmotionspeed", "Slow Motion Speed", "How slow time moves (0.1 = very slow, 1.0 = normal)", 0.1f, 1.0f, 0.3f};

	class SlowMotion : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			MISC::SET_TIME_SCALE(_SlowMotionSpeed.GetState());
		}

		virtual void OnDisable() override
		{
			MISC::SET_TIME_SCALE(1.0f); // Restore normal time
		}
	};

	static SlowMotion _SlowMotion{"slowmotion", "Slow Motion", "Experience the world in cinematic slow-mo"};
}
