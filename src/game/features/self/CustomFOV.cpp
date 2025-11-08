#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _CustomFOVValue{"customfovvalue", "Field of View", "Camera FOV (30 = zoomed in, 90 = wide angle, 120 = fish eye)", 30.0f, 120.0f, 50.0f};

	class CustomFOV : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			Cam gameplayCam = CAM::GET_RENDERING_CAM();
			if (gameplayCam)
			{
				CAM::SET_CAM_FOV(gameplayCam, _CustomFOVValue.GetState());
			}
		}

		virtual void OnDisable() override
		{
			Cam gameplayCam = CAM::GET_RENDERING_CAM();
			if (gameplayCam)
			{
				CAM::SET_CAM_FOV(gameplayCam, 50.0f); // Restore default FOV
			}
		}
	};

	static CustomFOV _CustomFOV{"customfov", "Custom FOV", "Adjust your field of view for better screenshots"};
}
