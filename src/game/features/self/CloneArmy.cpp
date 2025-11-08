#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "util/Notifications.hpp"
#include <vector>

namespace YimMenu::Features
{
	static IntCommand _CloneCount{"clonecount", "Clone Count", "How many clones to spawn", 1, 20, 5};
	static std::vector<Ped> g_Clones;

	class SpawnCloneArmy : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([this] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				// Clear old clones
				for (auto clone : g_Clones)
				{
					if (ENTITY::DOES_ENTITY_EXIST(clone))
					{
						PED::DELETE_PED(&clone);
					}
				}
				g_Clones.clear();

				// Get player model
				Hash playerModel = ENTITY::GET_ENTITY_MODEL(player.GetHandle());
				auto pos = player.GetPosition();

				// Request model
				STREAMING::REQUEST_MODEL(playerModel, false);
				while (!STREAMING::HAS_MODEL_LOADED(playerModel))
					ScriptMgr::Yield();

				int count = _CloneCount.GetState();

				// Spawn clones in a circle around player
				for (int i = 0; i < count; i++)
				{
					float angle = (i * 360.0f / count) * (3.14159f / 180.0f);
					float radius = 3.0f;

					rage::fvector3 clonePos;
					clonePos.x = pos.x + cos(angle) * radius;
					clonePos.y = pos.y + sin(angle) * radius;
					clonePos.z = pos.z;

					Ped clone = PED::CREATE_PED(playerModel, clonePos.x, clonePos.y, clonePos.z, 0.0f, true, true, true, true);

					if (clone)
					{
						// Make clone follow player
						TASK::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(clone, player.GetHandle(),
							cos(angle) * radius, sin(angle) * radius, 0.0f,
							1.0f, -1, 2.0f, true);

						// Make invincible
						ENTITY::SET_ENTITY_INVINCIBLE(clone, true);

						// Copy player's outfit/appearance
						PED::CLONE_PED_TO_TARGET(player.GetHandle(), clone);

						g_Clones.push_back(clone);
					}
				}

				Notifications::Show("Clone Army", std::format("Spawned {} clones!", count), NotificationType::Success);

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(playerModel);
			});
		}
	};

	static SpawnCloneArmy _SpawnCloneArmy{"spawnclonearmy", "Spawn Clone Army", "Create an army of yourself!"};

	class DeleteClones : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				for (auto clone : g_Clones)
				{
					if (ENTITY::DOES_ENTITY_EXIST(clone))
					{
						PED::DELETE_PED(&clone);
					}
				}
				g_Clones.clear();
				Notifications::Show("Clone Army", "All clones removed!", NotificationType::Info);
			});
		}
	};

	static DeleteClones _DeleteClones{"deleteclones", "Delete Clones", "Remove all your clones"};
}
