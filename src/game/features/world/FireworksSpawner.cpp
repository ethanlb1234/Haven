#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"
#include <cmath>

namespace YimMenu::Features
{
	enum class FireworkType
	{
		SINGLE_BURST,
		MULTI_BURST,
		FOUNTAIN,
		RAINBOW_BURST,
		HEART_SHAPE
	};

	static const std::pair<FireworkType, const char*> g_FireworkTypes[] = {
		{FireworkType::SINGLE_BURST, "Single Burst"},
		{FireworkType::MULTI_BURST, "Multi Burst"},
		{FireworkType::FOUNTAIN, "Fountain"},
		{FireworkType::RAINBOW_BURST, "Rainbow Burst"},
		{FireworkType::HEART_SHAPE, "Heart Shape"}
	};

	static ListCommand _FireworkType{"fireworktype", "Firework Type", "Choose your firework style", g_FireworkTypes, (int)FireworkType::SINGLE_BURST};

	class SpawnFirework : public Command
	{
		using Command::Command;

		void SpawnFireworkBurst(rage::fvector3 pos, int r, int g, int b)
		{
			// Request particle effect
			STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_rcpaparazzo1"_J);

			if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_rcpaparazzo1"_J))
			{
				GRAPHICS::USE_PARTICLE_FX_ASSET("scr_rcpaparazzo1");

				// Create firework explosion effect
				GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(
					"scr_mich4_firework_burst_spawn",
					pos.x, pos.y, pos.z,
					0.0f, 0.0f, 0.0f,
					2.0f, // Scale
					false, false, false, false
				);
			}
		}

		virtual void OnCall() override
		{
			FiberPool::Push([this] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				auto pos = player.GetPosition();
				auto type = (FireworkType)_FireworkType.GetState();

				// Launch fireworks above player
				pos.z += 20.0f;

				switch (type)
				{
				case FireworkType::SINGLE_BURST:
					SpawnFireworkBurst(pos, 255, 0, 0); // Red
					break;

				case FireworkType::MULTI_BURST:
					// Launch 5 fireworks in sequence
					for (int i = 0; i < 5; i++)
					{
						rage::fvector3 burstPos = pos;
						burstPos.x += (i - 2) * 10.0f;
						SpawnFireworkBurst(burstPos, 255, (i * 50), (255 - i * 50));
						ScriptMgr::Yield(10);
					}
					break;

				case FireworkType::FOUNTAIN:
					// Multiple bursts going up
					for (int i = 0; i < 10; i++)
					{
						rage::fvector3 fountainPos = pos;
						fountainPos.z += i * 3.0f;
						SpawnFireworkBurst(fountainPos, 255, 215, 0); // Gold
						ScriptMgr::Yield(5);
					}
					break;

				case FireworkType::RAINBOW_BURST:
					// Rainbow colored bursts
					int colors[][3] = {{255,0,0}, {255,127,0}, {255,255,0}, {0,255,0}, {0,0,255}, {75,0,130}, {148,0,211}};
					for (int i = 0; i < 7; i++)
					{
						rage::fvector3 rainbowPos = pos;
						float angle = (i * 51.4f) * (3.14159f / 180.0f); // Convert degrees to radians
						rainbowPos.x += cos(angle) * 10.0f;
						rainbowPos.y += sin(angle) * 10.0f;
						SpawnFireworkBurst(rainbowPos, colors[i][0], colors[i][1], colors[i][2]);
						ScriptMgr::Yield(5);
					}
					break;

				case FireworkType::HEART_SHAPE:
					// Heart shape pattern
					for (float t = 0; t < 360; t += 20)
					{
						float rad = t * (3.14159f / 180.0f);
						float x = 16 * pow(sin(rad), 3);
						float y = 13 * cos(rad) - 5 * cos(2*rad) - 2 * cos(3*rad) - cos(4*rad);

						rage::fvector3 heartPos = pos;
						heartPos.x += x;
						heartPos.y += y;
						SpawnFireworkBurst(heartPos, 255, 20, 147); // Pink
						ScriptMgr::Yield(3);
					}
					break;
				}

				Notifications::Show("Fireworks", "Enjoy the show!", NotificationType::Success);
			});
		}
	};

	static SpawnFirework _SpawnFirework{"spawnfirework", "Launch Fireworks", "Launch spectacular fireworks anywhere!"};
}
