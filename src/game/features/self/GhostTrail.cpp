#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include <vector>

namespace YimMenu::Features
{
	struct GhostClone
	{
		Ped ped;
		int lifetime;
	};

	static std::vector<GhostClone> g_GhostClones;

	class GhostTrail : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline int tickCounter = 0;

		virtual void OnTick() override
		{
			auto player = Self::GetPed();
			if (!player.IsValid())
				return;

			tickCounter++;

			// Spawn a ghost clone every 30 ticks (about 0.5 seconds)
			if (tickCounter >= 30)
			{
				tickCounter = 0;

				// Get player model
				Hash playerModel = ENTITY::GET_ENTITY_MODEL(player.GetHandle());

				// Get player position and heading
				auto pos = player.GetPosition();
				float heading = ENTITY::GET_ENTITY_HEADING(player.GetHandle());

				// Request model
				STREAMING::REQUEST_MODEL(playerModel, false);

				// Create ghost clone
				Ped ghost = PED::CREATE_PED(playerModel, pos.x, pos.y, pos.z, heading, false, false, false, false);

				if (ghost)
				{
					// Make it look ghostly
					ENTITY::SET_ENTITY_ALPHA(ghost, 100, false); // Semi-transparent
					ENTITY::SET_ENTITY_COLLISION(ghost, false, false);
					ENTITY::FREEZE_ENTITY_POSITION(ghost, true);

					// Copy player's animation
					TASK::TASK_STAND_STILL(ghost, -1);

					// Add to list
					g_GhostClones.push_back({ghost, 150}); // 150 ticks = ~2.5 seconds
				}
			}

			// Update and remove old ghosts
			for (auto it = g_GhostClones.begin(); it != g_GhostClones.end();)
			{
				it->lifetime--;

				// Fade out over time
				int alpha = (it->lifetime * 100) / 150;
				if (ENTITY::DOES_ENTITY_EXIST(it->ped))
				{
					ENTITY::SET_ENTITY_ALPHA(it->ped, alpha, false);
				}

				if (it->lifetime <= 0)
				{
					if (ENTITY::DOES_ENTITY_EXIST(it->ped))
					{
						PED::DELETE_PED(&it->ped);
					}
					it = g_GhostClones.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		virtual void OnDisable() override
		{
			// Clean up all ghost clones
			for (auto& ghost : g_GhostClones)
			{
				if (ENTITY::DOES_ENTITY_EXIST(ghost.ped))
				{
					PED::DELETE_PED(&ghost.ped);
				}
			}
			g_GhostClones.clear();
			tickCounter = 0;
		}
	};

	static GhostTrail _GhostTrail{"ghosttrail", "Ghost Trail", "Leave holographic clones behind as you move! Epic visual effect!"};
}
