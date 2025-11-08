#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class NightVision : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			// Apply night vision effect
			GRAPHICS::SET_TIMECYCLE_MODIFIER("ReplayHighlights");
			GRAPHICS::SET_TIMECYCLE_MODIFIER_STRENGTH(0.8f);

			// TODO: RDR2 doesn't have _SET_VISION_TYPE native
			// GRAPHICS::_SET_VISION_TYPE(1); // Enhanced vision type
		}

		virtual void OnDisable() override
		{
			// Remove night vision
			GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
			// TODO: RDR2 doesn't have _SET_VISION_TYPE native
			// GRAPHICS::_SET_VISION_TYPE(0); // Normal vision
		}
	};

	static NightVision _NightVision{"nightvision", "Night Vision", "See clearly in the dark with enhanced vision!"};
}
