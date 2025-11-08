#include "core/commands/BoolCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Photo Mode - Advanced screenshot mode with filters, camera controls, and poses

	enum class PhotoFilter
	{
		NONE,
		SEPIA,
		NOIR,
		VINTAGE,
		DRAMATIC,
		COLD,
		WARM,
		NIGHT_VISION,
		DRUNK
	};

	static const std::vector<std::pair<int, const char*>> g_PhotoFilters = {
		{(int)PhotoFilter::NONE, "No Filter"},
		{(int)PhotoFilter::SEPIA, "Sepia (Old West)"},
		{(int)PhotoFilter::NOIR, "Black & White"},
		{(int)PhotoFilter::VINTAGE, "Vintage"},
		{(int)PhotoFilter::DRAMATIC, "Dramatic"},
		{(int)PhotoFilter::COLD, "Cold Blue"},
		{(int)PhotoFilter::WARM, "Warm Orange"},
		{(int)PhotoFilter::NIGHT_VISION, "Night Vision"},
		{(int)PhotoFilter::DRUNK, "Drunk Effect"}
	};

	static BoolCommand _PhotoModeActive{"photomodeactive", "Photo Mode", "Freeze time and add filters for screenshots"};
	static ListCommand _PhotoFilter{"photofilter", "Photo Filter", "Choose visual filter", g_PhotoFilters, 0};
	static BoolCommand _HideHUD{"photohidehud", "Hide HUD", "Hide all HUD elements"};
	static BoolCommand _SlowMotion{"photoslowmo", "Slow Motion", "Enable slow motion"};

	const char* GetTimecycleForFilter(PhotoFilter filter)
	{
		switch (filter)
		{
		case PhotoFilter::SEPIA:
			return "MP_SleepFade";
		case PhotoFilter::NOIR:
			return "BlackOut";
		case PhotoFilter::VINTAGE:
			return "MP_Deathmatch_night";
		case PhotoFilter::DRAMATIC:
			return "RespawnPulse02";
		case PhotoFilter::COLD:
			return "MP_Corona_PhotoEffect";
		case PhotoFilter::WARM:
			return "MP_Deathmatch_intro";
		case PhotoFilter::NIGHT_VISION:
			return "BikerFilter01";
		case PhotoFilter::DRUNK:
			return "DRUNK";
		case PhotoFilter::NONE:
		default:
			return "";
		}
	}

	class PhotoModeController : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline bool wasActive = false;
		static inline float originalTimescale = 1.0f;

		virtual void OnTick() override
		{
			if (!_PhotoModeActive.GetState())
			{
				if (wasActive)
				{
					// Restore normal settings
					GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
					MISC::SET_TIME_SCALE(originalTimescale);
					HUD::DISPLAY_HUD(true);
					HUD::DISPLAY_RADAR(true);
					wasActive = false;
				}
				return;
			}

			wasActive = true;

			// Apply filter
			PhotoFilter filter = (PhotoFilter)_PhotoFilter.GetState();
			const char* modifier = GetTimecycleForFilter(filter);

			if (filter != PhotoFilter::NONE)
			{
				GRAPHICS::SET_TIMECYCLE_MODIFIER(modifier);
				GRAPHICS::SET_TIMECYCLE_MODIFIER_STRENGTH(1.0f);
			}
			else
			{
				GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
			}

			// Hide HUD if enabled
			if (_HideHUD.GetState())
			{
				HUD::DISPLAY_HUD(false);
				HUD::DISPLAY_RADAR(false);
			}
			else
			{
				HUD::DISPLAY_HUD(true);
				HUD::DISPLAY_RADAR(true);
			}

			// Slow motion effect
			if (_SlowMotion.GetState())
			{
				MISC::SET_TIME_SCALE(0.3f);
			}
			else
			{
				MISC::SET_TIME_SCALE(1.0f);
			}
		}

		virtual void OnDisable() override
		{
			GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
			MISC::SET_TIME_SCALE(1.0f);
			HUD::DISPLAY_HUD(true);
			HUD::DISPLAY_RADAR(true);
			wasActive = false;
		}
	};

	static PhotoModeController _PhotoModeController{"photomodecontroller", "Photo Mode Controller", ""};

	// Free camera for photo mode
	class PhotoFreeCamera : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline Cam photoCam = 0;
		static inline float camDistance = 5.0f;
		static inline float camAngle = 0.0f;
		static inline float camHeight = 2.0f;

		virtual void OnTick() override
		{
			if (!_PhotoModeActive.GetState())
				return;

			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			auto pos = ped.GetPosition();

			// Create camera
			if (!photoCam || !CAM::DOES_CAM_EXIST(photoCam))
			{
				photoCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", true);
				CAM::SET_CAM_ACTIVE(photoCam, true);
				CAM::RENDER_SCRIPT_CAMS(true, false, 3000, true, false, 0);
			}

			// Camera controls (use controller/keyboard)
			// Rotate with A/D
			if (PAD::IS_CONTROL_PRESSED(0, 0x7065027D)) // INPUT_MOVE_LEFT
				camAngle += 2.0f;
			if (PAD::IS_CONTROL_PRESSED(0, 0xB4E465B4)) // INPUT_MOVE_RIGHT
				camAngle -= 2.0f;

			// Zoom with W/S
			if (PAD::IS_CONTROL_PRESSED(0, 0x8FD015D8)) // INPUT_MOVE_UP
				camDistance = std::max(1.0f, camDistance - 0.1f);
			if (PAD::IS_CONTROL_PRESSED(0, 0xD27782E3)) // INPUT_MOVE_DOWN
				camDistance = std::min(20.0f, camDistance + 0.1f);

			// Height with Q/E
			if (PAD::IS_CONTROL_PRESSED(0, 0xE30CD707)) // INPUT_CROUCH
				camHeight = std::max(-5.0f, camHeight - 0.1f);
			if (PAD::IS_CONTROL_PRESSED(0, 0x8FFC75D6)) // INPUT_JUMP
				camHeight = std::min(10.0f, camHeight + 0.1f);

			// Calculate camera position
			float radAngle = camAngle * (3.14159f / 180.0f);
			float camX = pos.x + (cos(radAngle) * camDistance);
			float camY = pos.y + (sin(radAngle) * camDistance);
			float camZ = pos.z + camHeight;

			CAM::SET_CAM_COORD(photoCam, camX, camY, camZ);
			CAM::POINT_CAM_AT_COORD(photoCam, pos.x, pos.y, pos.z + 1.0f);
			CAM::SET_CAM_FOV(photoCam, 50.0f);
		}

		virtual void OnDisable() override
		{
			if (photoCam && CAM::DOES_CAM_EXIST(photoCam))
			{
				CAM::RENDER_SCRIPT_CAMS(false, false, 3000, true, false, 0);
				CAM::SET_CAM_ACTIVE(photoCam, false);
				CAM::DESTROY_CAM(photoCam, false);
				photoCam = 0;
			}
		}
	};

	static PhotoFreeCamera _PhotoFreeCamera{"photofreecamera", "Free Camera", "Enable free camera for photo mode"};
}
