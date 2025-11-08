#pragma once
#include "Overlay.hpp"
#include "Menu.hpp"
#include "core/commands/BoolCommand.hpp"
#include "game/backend/ProtectionStats.hpp"
#include "game/pointers/Pointers.hpp"
#include "game/rdr/Natives.hpp"
#include "game/rdr/invoker/Invoker.hpp"

namespace YimMenu::Features
{
	BoolCommand _OverlayEnabled("overlay", "Overlay Enabled", "Show an info overlay at the top left corner of the screen");
	BoolCommand _OverlayShowFPS("overlayfps", "Overlay Show FPS", "Show frame rate in the info overlay");
	BoolCommand _OverlayShowProtections("overlayprotections", "Show Protection Stats", "Display blocked attack statistics", true);
}

namespace YimMenu
{
	void Overlay::Draw()
	{
		if (!Features::_OverlayEnabled.GetState() || !NativeInvoker::AreHandlersCached())
			return;

		ImGui::SetNextWindowSize(ImVec2(*Pointers.ScreenResX - 10.0f, *Pointers.ScreenResY - 10.0f), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushFont(Menu::Font::g_OverlayFont);

		ImGui::Begin("##overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

		if (Features::_OverlayShowFPS.GetState())
			ImGui::Text("FPS: %d", (int)(1 / MISC::GET_SYSTEM_TIME_STEP()));

		if (Features::_OverlayShowProtections.GetState())
		{
			auto stats = ProtectionStats::GetSessionStats();
			if (stats.m_TotalBlocked > 0)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
				ImGui::Text("Protection Stats:");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // Red
				if (stats.m_CrashAttempts > 0)
					ImGui::Text("  Crash Attempts Blocked: %d", stats.m_CrashAttempts);
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.0f, 1.0f)); // Orange
				if (stats.m_KickAttempts > 0)
					ImGui::Text("  Kick Attempts Blocked: %d", stats.m_KickAttempts);
				if (stats.m_EntitySpam > 0)
					ImGui::Text("  Entity Spam Blocked: %d", stats.m_EntitySpam);
				if (stats.m_EventSpam > 0)
					ImGui::Text("  Event Spam Blocked: %d", stats.m_EventSpam);
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f)); // Gray
				ImGui::Text("  Total Blocked: %d", stats.m_TotalBlocked);
				ImGui::PopStyleColor();
			}
		}

		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::End();
	}
}