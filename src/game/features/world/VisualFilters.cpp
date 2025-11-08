#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	enum class VisualFilter
	{
		NONE,
		SEPIA_WESTERN,
		VIBRANT,
		NOIR,
		DREAMY,
		GOLDEN_HOUR,
		WINTER,
		DRAMATIC,
		VINTAGE,
		WARM_SUNSET,
		COOL_NIGHT
	};

	static const std::pair<int, const char*> g_VisualFilters[] = {
		{(int)VisualFilter::NONE, "None (Default)"},
		{(int)VisualFilter::SEPIA_WESTERN, "Sepia Western"},
		{(int)VisualFilter::VIBRANT, "Vibrant Colors"},
		{(int)VisualFilter::NOIR, "Film Noir"},
		{(int)VisualFilter::DREAMY, "Dreamy Glow"},
		{(int)VisualFilter::GOLDEN_HOUR, "Golden Hour"},
		{(int)VisualFilter::WINTER, "Winter Blues"},
		{(int)VisualFilter::DRAMATIC, "Dramatic"},
		{(int)VisualFilter::VINTAGE, "Vintage Photo"},
		{(int)VisualFilter::WARM_SUNSET, "Warm Sunset"},
		{(int)VisualFilter::COOL_NIGHT, "Cool Night"}
	};

	static ListCommand _VisualFilter{"visualfilter", "Visual Filter", "Change the visual atmosphere", g_VisualFilters, (int)VisualFilter::NONE};

	class VisualFilterApplier : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		const char* GetTimecycleModifier(VisualFilter filter)
		{
			switch (filter)
			{
			case VisualFilter::SEPIA_WESTERN:
				return "MP_Deathmatch_night"; // Warm sepia tones
			case VisualFilter::VIBRANT:
				return "rply_motionblur"; // Enhanced colors
			case VisualFilter::NOIR:
				return "BlackOut"; // Black and white
			case VisualFilter::DREAMY:
				return "RespawnPulse02"; // Soft ethereal glow
			case VisualFilter::GOLDEN_HOUR:
				return "GoldenRatio02"; // Warm golden lighting
			case VisualFilter::WINTER:
				return "MP_FrozenCarbon"; // Cool blue tint
			case VisualFilter::DRAMATIC:
				return "MP_Deathmatch_night02"; // High contrast
			case VisualFilter::VINTAGE:
				return "MP_OdriscolBoys_faded"; // Faded vintage look
			case VisualFilter::WARM_SUNSET:
				return "MP_BountyTarget"; // Warm orange tones
			case VisualFilter::COOL_NIGHT:
				return "MP_SleepFade"; // Cool dark atmosphere
			default:
				return nullptr;
			}
		}

		virtual void OnTick() override
		{
			auto filter = (VisualFilter)_VisualFilter.GetState();

			const char* modifier = GetTimecycleModifier(filter);
			if (modifier)
			{
				GRAPHICS::SET_TIMECYCLE_MODIFIER(modifier);
				GRAPHICS::SET_TIMECYCLE_MODIFIER_STRENGTH(1.0f);
			}
			else
			{
				GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
			}
		}

		virtual void OnDisable() override
		{
			GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
		}
	};

	static VisualFilterApplier _VisualFilterApplier{"visualfilterapplier", "Enable Visual Filter", "Apply the selected visual filter"};
}
