// DISABLED: This feature uses GTA5-style HUD natives that don't exist in RDR2
// RDR2 uses _BG_ prefixed natives which work differently
// Ref: scrCommand_dump_b1491.50 only has _BG_SET_TEXT_SCALE, _BG_SET_TEXT_COLOR, etc.

#if 0
#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include <string>

namespace YimMenu::Features
{
	static std::string g_ChatBubbleText = "";
	static bool g_ShowChatBubble = false;

	class ChatBubble : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (g_ShowChatBubble && !g_ChatBubbleText.empty())
			{
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				auto pos = ped.GetPosition();

				// Draw text above player's head
				HUD::SET_TEXT_SCALE(0.5f, 0.5f);
				HUD::SET_TEXT_COLOUR(255, 255, 255, 255);
				HUD::SET_TEXT_CENTRE(true);
				HUD::SET_TEXT_DROPSHADOW(2, 0, 0, 0, 255);
				HUD::SET_TEXT_OUTLINE();

				// Draw a background box
				GRAPHICS::DRAW_RECT(0.5f, 0.15f, g_ChatBubbleText.length() * 0.015f, 0.05f, 0, 0, 0, 150, false);

				// Display the text
				HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
				HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(g_ChatBubbleText.c_str());
				HUD::_END_TEXT_COMMAND_DISPLAY_TEXT(0.5f, 0.15f, 0);
			}
		}
	};

	static ChatBubble _ChatBubble{"chatbubble", "Chat Bubble", "Display text above your head for others to see!"};

	// Helper function to set chat text (will be called from UI)
	void SetChatBubbleText(const std::string& text)
	{
		g_ChatBubbleText = text;
		g_ShowChatBubble = !text.empty();
	}

	std::string& GetChatBubbleText()
	{
		return g_ChatBubbleText;
	}

	bool& GetChatBubbleEnabled()
	{
		return g_ShowChatBubble;
	}
}
#endif
