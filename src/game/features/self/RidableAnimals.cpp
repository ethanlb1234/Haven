// DISABLED: SET_PED_AS_MOUNT native doesn't exist in RDR2
// Ref: scrCommand_dump_b1491.50 has TASK_MOUNT_ANIMAL but no SET_PED_AS_MOUNT
// This feature cannot be implemented with current natives

#if 0
#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Enums.hpp"
#include "game/rdr/Natives.hpp"
#include "util/Notifications.hpp"

namespace YimMenu::Features
{
	enum class RidableAnimalType
	{
		EAGLE,
		OWL,
		HAWK,
		LION,
		PANTHER,
		BUFFALO,
		ELK,
		ALLIGATOR,
		BEAR,
		WOLF
	};

	static const std::pair<RidableAnimalType, const char*> g_RidableAnimals[] = {
		{RidableAnimalType::EAGLE, "Eagle (Flying)"},
		{RidableAnimalType::OWL, "Owl (Flying)"},
		{RidableAnimalType::HAWK, "Hawk (Flying)"},
		{RidableAnimalType::LION, "Lion"},
		{RidableAnimalType::PANTHER, "Panther"},
		{RidableAnimalType::BUFFALO, "Buffalo"},
		{RidableAnimalType::ELK, "Elk"},
		{RidableAnimalType::ALLIGATOR, "Alligator"},
		{RidableAnimalType::BEAR, "Bear"},
		{RidableAnimalType::WOLF, "Wolf"}
	};

	static ListCommand _RidableAnimalType{"ridableanimaltype", "Ridable Animal", "Choose your mount", g_RidableAnimals, (int)RidableAnimalType::EAGLE};

	class SpawnRidableAnimal : public Command
	{
		using Command::Command;

		Hash GetAnimalHash(RidableAnimalType type)
		{
			switch (type)
			{
			case RidableAnimalType::EAGLE:
				return "a_c_eagle_01"_J;
			case RidableAnimalType::OWL:
				return "a_c_owl_01"_J;
			case RidableAnimalType::HAWK:
				return "a_c_hawk_01"_J;
			case RidableAnimalType::LION:
				return "a_c_lion"_J; // Mountain lion/cougar
			case RidableAnimalType::PANTHER:
				return "a_c_panther_01"_J;
			case RidableAnimalType::BUFFALO:
				return "a_c_buffalo"_J;
			case RidableAnimalType::ELK:
				return "a_c_elk_01"_J;
			case RidableAnimalType::ALLIGATOR:
				return "a_c_alligator_01"_J;
			case RidableAnimalType::BEAR:
				return "a_c_bear_01"_J;
			case RidableAnimalType::WOLF:
				return "a_c_wolf"_J;
			default:
				return "a_c_eagle_01"_J;
			}
		}

		bool IsFlying(RidableAnimalType type)
		{
			return type == RidableAnimalType::EAGLE ||
			       type == RidableAnimalType::OWL ||
			       type == RidableAnimalType::HAWK;
		}

		virtual void OnCall() override
		{
			FiberPool::Push([this] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				auto type = (RidableAnimalType)_RidableAnimalType.GetState();
				Hash animalHash = GetAnimalHash(type);
				bool flying = IsFlying(type);

				// Request model
				STREAMING::REQUEST_MODEL(animalHash, false);
				while (!STREAMING::HAS_MODEL_LOADED(animalHash))
					ScriptMgr::Yield();

				// Get player position
				auto pos = player.GetPosition();

				// Spawn animal
				Ped animal = PED::CREATE_PED(animalHash, pos.x + 1.0f, pos.y, pos.z, 0.0f, true, true, true, true);

				if (animal)
				{
					// Make it ridable like a horse
					PED::SET_PED_AS_MOUNT(animal, true); // THIS NATIVE DOESN'T EXIST
					PED::_SET_PED_PROMPT_NAME(animal, "Ridable Animal");

					// Set flags to make it mountable
					PED::SET_PED_CONFIG_FLAG(animal, (int)PedConfigFlag::BlockHorseProvokingAttack, true);
					PED::SET_PED_CONFIG_FLAG(animal, (int)PedConfigFlag::DisableMountedMeleeDamage, true);

					// Make invincible
					ENTITY::SET_ENTITY_INVINCIBLE(animal, true);

					// If flying animal, disable gravity
					if (flying)
					{
						ENTITY::SET_ENTITY_HAS_GRAVITY(animal, false);
						Notifications::Show("Ridable Animal", "Mount the animal and use Space to fly up, Ctrl to descend!", NotificationType::Success);
					}
					else
					{
						Notifications::Show("Ridable Animal", "Approach the animal and press E to mount it!", NotificationType::Success);
					}
				}

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(animalHash);
			});
		}
	};

	static SpawnRidableAnimal _SpawnRidableAnimal{"spawnridableanimal", "Spawn Ridable Animal", "Spawn an animal you can ride like a horse!"};
}
#endif
