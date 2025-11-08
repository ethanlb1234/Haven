#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _LetterboxSize{"letterboxsize", "Letterbox Size", "How thick the black bars are", 0.05f, 0.3f, 0.15f};
	static BoolCommand _HideHUD{"hidehud", "Hide HUD", "Hide all UI elements for clean screenshots"};

	class CinematicLetterbox : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			// Draw black bars at top and bottom
			float barHeight = _LetterboxSize.GetState();

			// Top bar (RDR2 DRAW_RECT has different parameters than GTA V)
			GRAPHICS::DRAW_RECT(0.5f, barHeight / 2.0f, 1.0f, barHeight, 0, 0, 0, 255);

			// Bottom bar
			GRAPHICS::DRAW_RECT(0.5f, 1.0f - (barHeight / 2.0f), 1.0f, barHeight, 0, 0, 0, 255);
		}
	};

	static CinematicLetterbox _CinematicLetterbox{"cinematicletterbox", "Cinematic Letterbox", "Add movie-style black bars for epic screenshots!"};

	class HideHUD : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (_HideHUD.GetState())
			{
				// Hide all HUD elements
				HUD::HIDE_HUD_AND_RADAR_THIS_FRAME();
				// TODO: RDR2 doesn't have HIDE_HUD_COMPONENT_THIS_FRAME or HIDE_HELP_TEXT_THIS_FRAME
				// Need to find RDR2-specific HUD hiding methods
				/*
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(1); // Wanted
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(2); // Weapon icon
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(3); // Cash
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(4); // MP cash
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(6); // Vehicle name
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(7); // Area name
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(8); // Vehicle class
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(9); // Street name
				HUD::HIDE_HELP_TEXT_THIS_FRAME();
				*/
			}
		}
	};

	static HideHUD _HideHUDImpl{"hidehudimpl", "Hide HUD Impl", ""};
}
