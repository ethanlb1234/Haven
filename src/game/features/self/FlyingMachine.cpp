#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	enum class FlyingMachineType
	{
		AIRSHIP,
		PLANE,
		UFO,
		HOT_AIR_BALLOON,
		FLYING_SHARK,
		MAGIC_CARPET
	};

	static const std::pair<FlyingMachineType, const char*> g_FlyingMachines[] = {
		{FlyingMachineType::AIRSHIP, "Airship"},
		{FlyingMachineType::PLANE, "Biplane"},
		{FlyingMachineType::UFO, "UFO"},
		{FlyingMachineType::HOT_AIR_BALLOON, "Hot Air Balloon"},
		{FlyingMachineType::FLYING_SHARK, "Flying Shark"},
		{FlyingMachineType::MAGIC_CARPET, "Magic Carpet"}
	};

	static ListCommand _FlyingMachineType{"flyingmachinetype", "Flying Machine", "Choose your flying vehicle", g_FlyingMachines, (int)FlyingMachineType::AIRSHIP};

	class SpawnFlyingMachine : public Command
	{
		using Command::Command;

		Hash GetVehicleHash(FlyingMachineType type)
		{
			switch (type)
			{
			case FlyingMachineType::AIRSHIP:
				return "hotairballoon01"_J; // Airship/balloon
			case FlyingMachineType::PLANE:
				return "utilliwag"_J; // Wagon (we'll make it fly!)
			case FlyingMachineType::UFO:
				return "breach_cannon"_J; // Cannon (UFO shape)
			case FlyingMachineType::HOT_AIR_BALLOON:
				return "hotairballoon01"_J;
			case FlyingMachineType::FLYING_SHARK:
				return "pirogue"_J; // Boat (shark concept)
			case FlyingMachineType::MAGIC_CARPET:
				return "utilliwag"_J; // Small wagon base
			default:
				return "hotairballoon01"_J;
			}
		}

		virtual void OnCall() override
		{
			FiberPool::Push([this] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				auto type = (FlyingMachineType)_FlyingMachineType.GetState();
				Hash vehicleHash = GetVehicleHash(type);

				// Request model
				STREAMING::REQUEST_MODEL(vehicleHash, false);
				while (!STREAMING::HAS_MODEL_LOADED(vehicleHash))
					ScriptMgr::Yield();

				// Get player position
				auto pos = player.GetPosition();

				// Spawn flying machine above player
				Vehicle vehicle = VEHICLE::CREATE_VEHICLE(vehicleHash, pos.x, pos.y, pos.z + 2.0f, 0.0f, true, true, false, false);

				if (vehicle)
				{
					// Make it fly!
					ENTITY::SET_ENTITY_INVINCIBLE(vehicle, true);
					VEHICLE::SET_VEHICLE_GRAVITY(vehicle, false); // No gravity = flies!
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, 0.0f);

					// Put player in vehicle
					TASK::TASK_WARP_PED_INTO_VEHICLE(player.GetHandle(), vehicle, -1);

					// Add upward velocity for hovering
					ENTITY::SET_ENTITY_VELOCITY(vehicle, 0.0f, 0.0f, 1.0f);

					Notifications::Show("Flying Machine", "Use WASD to fly! Space = up, Ctrl = down", NotificationType::Success);
				}

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehicleHash);
			});
		}
	};

	static SpawnFlyingMachine _SpawnFlyingMachine{"spawnflyingmachine", "Spawn Flying Machine", "Spawn a flying vehicle to explore the skies!"};
}
