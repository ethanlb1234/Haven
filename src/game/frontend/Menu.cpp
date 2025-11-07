#include "Menu.hpp"

#include "core/commands/Commands.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "core/renderer/Renderer.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/frontend/fonts/Fonts.hpp"
#include "game/pointers/Pointers.hpp"
#include "submenus/Debug.hpp"
#include "submenus/Network.hpp"
#include "submenus/Players.hpp"
#include "submenus/Self.hpp"
#include "submenus/Settings.hpp"
#include "submenus/Teleport.hpp"
#include "submenus/World.hpp"
#include "submenus/Recovery.hpp"

namespace YimMenu
{
    void Menu::Init()
    {
        // Arguably the only place this file should be edited at for more menus
        UIManager::AddSubmenu(std::make_shared<Submenus::Self>());
        UIManager::AddSubmenu(std::make_shared<Submenus::Teleport>());
        UIManager::AddSubmenu(std::make_shared<Submenus::Network>());
        UIManager::AddSubmenu(std::make_shared<Submenus::Players>());
        UIManager::AddSubmenu(std::make_shared<Submenus::World>());
        UIManager::AddSubmenu(std::make_shared<Submenus::Recovery>());
        UIManager::AddSubmenu(std::make_shared<Submenus::Settings>());
        UIManager::AddSubmenu(std::make_shared<Submenus::Debug>());

        Renderer::AddRendererCallBack(
            [&] {
                if (!GUI::IsOpen())
                    return;

                ImGui::PushFont(Menu::Font::g_DefaultFont);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImU32(ImColor(0, 0, 0, 100))); // Darker alpha for less transparency

                ImGui::SetNextWindowSize(ImVec2((*Pointers.ScreenResX / 3.1), (*Pointers.ScreenResY / 1.5)), ImGuiCond_Once);

                if (ImGui::Begin("Haven - Peaceful Menu", nullptr, ImGuiWindowFlags_NoDecoration))
                {
                    // Western crimson red for the title, like a wanted poster
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.25f, 0.18f, 1.0f)); // Western Crimson
                    if (ImGui::Button("HAVEN", ImVec2(120, 0)))
                    {
                        if (ScriptMgr::CanTick())
                        {
                            FiberPool::Push([] {
                                Commands::Shutdown();
                                g_Running = false;
                            });
                        }
                        else
                        {
                            g_Running = false;
                        }
                    }
                    ImGui::PopStyleColor(); // Pop the text color after drawing the button

                    UIManager::Draw();

                    ImGui::End();
                }

                ImGui::PopStyleColor(); // Pop the window background color
                ImGui::PopFont();
            },
            -1);
    }

    void Menu::SetupStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        // Red Dead Redemption 2 Western Theme
        // Warm sepia text like old Western wanted posters
        style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.90f, 0.80f, 1.00f); // Warm sepia white
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.55f, 0.45f, 1.00f); // Faded brown

        // Dark leather/wood backgrounds
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.06f, 0.04f, 0.85f); // Dark brown leather
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.08f, 0.06f, 0.60f); // Lighter brown wood
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.05f, 0.03f, 0.90f); // Very dark leather

        // Crimson/rust borders like RDR2 UI
        style.Colors[ImGuiCol_Border] = ImVec4(0.60f, 0.15f, 0.10f, 0.50f); // Rust red border

        // Frame backgrounds (dark wood)
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.10f, 0.08f, 0.70f); // Dark wood
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.70f, 0.20f, 0.15f, 0.70f); // Crimson highlight
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.25f, 0.18f, 0.80f); // Bright crimson

        // Button colors (leather with crimson accents)
        style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.12f, 0.10f, 0.70f); // Leather brown
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.20f, 0.15f, 0.85f); // Crimson hover
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.25f, 0.18f, 0.95f); // Bright crimson press

        // Title bar (dark with gold/brass accent)
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.08f, 0.06f, 0.85f); // Dark brown
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.15f, 0.12f, 0.90f); // Medium brown
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.06f, 0.04f, 0.80f); // Very dark

        // Tab colors (old paper/leather)
        style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.12f, 0.10f, 0.70f); // Leather tab
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.70f, 0.20f, 0.15f, 0.80f); // Crimson hover
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.60f, 0.18f, 0.13f, 0.90f); // Active crimson

        // Header colors (table headers like ledger books)
        style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.12f, 0.10f, 0.70f); // Ledger brown
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.20f, 0.15f, 0.85f); // Crimson highlight
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.25f, 0.18f, 0.95f); // Bright crimson

        // Plot colors (warm gold/brass like belt buckles)
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.85f, 0.65f, 0.20f, 1.00f); // Gold
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.95f, 0.75f, 0.30f, 1.00f); // Bright gold
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.80f, 0.55f, 0.15f, 1.00f); // Brass
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.90f, 0.65f, 0.25f, 1.00f); // Bright brass

        // Slightly sharper corners for that rustic look
        style.GrabRounding = style.FrameRounding = style.ChildRounding = style.WindowRounding = 6.0f;
    }

    void Menu::SetupFonts()
    {
        auto& IO = ImGui::GetIO();
        ImFontConfig FontCfg{};
        FontCfg.FontDataOwnedByAtlas = false;

        Menu::Font::g_DefaultFont = IO.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), Menu::Font::g_DefaultFontSize, &FontCfg);
        Menu::Font::g_OptionsFont = IO.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), Menu::Font::g_OptionsFontSize, &FontCfg);
        Menu::Font::g_ChildTitleFont = IO.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), Menu::Font::g_ChildTitleFontSize, &FontCfg);
        Menu::Font::g_ChatFont = IO.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), Menu::Font::g_ChatFontSize, &FontCfg);
        Menu::Font::g_OverlayFont = IO.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), Menu::Font::g_OverlayFontSize, &FontCfg);

        UIManager::SetOptionsFont(Menu::Font::g_OptionsFont);
    }
}