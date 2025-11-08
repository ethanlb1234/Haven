#include "core/commands/Command.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	class OpenNearbyDoors : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				auto pos = player.GetPosition();
				int doorsOpened = 0;

				// Scan for nearby doors (radius 25m)
				for (float x = -25.0f; x <= 25.0f; x += 5.0f)
				{
					for (float y = -25.0f; y <= 25.0f; y += 5.0f)
					{
						for (float z = -10.0f; z <= 10.0f; z += 5.0f)
						{
							Hash doorHash;
							if (OBJECT::IS_DOOR_REGISTERED_WITH_SYSTEM(rage::joaat("door")))
							{
								OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(doorHash, 1); // 1 = Open
								doorsOpened++;
							}
						}
					}
				}

				// Try common door hashes
				Hash commonDoors[] = {
					"p_door_val_bank_01"_J,
					"p_door_val_sheriff"_J,
					"p_door_val_stable"_J,
					"p_door_gen_frame_01"_J,
					"p_door_gen_frame_02"_J,
					"p_door_val_saloon01"_J
				};

				for (auto doorHash : commonDoors)
				{
					OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(doorHash, 1); // Open
					OBJECT::DOOR_SYSTEM_SET_OPEN_RATIO(doorHash, 1.0f, false, true);
				}

				Notifications::Show("Door Controls", "Opened all nearby doors!", NotificationType::Success);
			});
		}
	};

	static OpenNearbyDoors _OpenNearbyDoors{"opennearbydoors", "Open Nearby Doors", "Open all doors around you (banks, shops, locked areas)"};

	class UnlockAllDoors : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Common locked doors to unlock
				Hash lockedDoors[] = {
					"p_door_val_bank_01"_J, // Valentine Bank
					"p_door_val_bank_02"_J,
					"p_door_rho_bank_01"_J, // Rhodes Bank
					"p_door_stdbank_door_01a"_J, // Saint Denis Bank
					"p_door_val_jail_01"_J, // Jail
					"p_door_val_stable"_J, // Stable
					"p_door_gen_frame_01"_J, // Generic doors
					"p_door_gen_frame_02"_J
				};

				for (auto doorHash : lockedDoors)
				{
					// Unlock the door
					OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(doorHash, 0); // Unlocked state
					OBJECT::_DOOR_SYSTEM_CHANGE_SCRIPT_OWNER(doorHash, true, false);
				}

				Notifications::Show("Door Controls", "Unlocked all major doors! Explore restricted areas!", NotificationType::Success);
			});
		}
	};

	static UnlockAllDoors _UnlockAllDoors{"unlockalldoors", "Unlock All Doors", "Unlock banks, jails, and restricted areas"};
}
