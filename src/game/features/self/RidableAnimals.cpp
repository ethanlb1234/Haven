#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Enums.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/ScriptMgr.hpp"

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

	static const std::vector<std::pair<int, const char*>> g_RidableAnimals = {
		{(int)RidableAnimalType::EAGLE, "Eagle (Flying)"},
		{(int)RidableAnimalType::OWL, "Owl (Flying)"},
		{(int)RidableAnimalType::HAWK, "Hawk (Flying)"},
		{(int)RidableAnimalType::LION, "Lion"},
		{(int)RidableAnimalType::PANTHER, "Panther"},
		{(int)RidableAnimalType::BUFFALO, "Buffalo"},
		{(int)RidableAnimalType::ELK, "Elk"},
		{(int)RidableAnimalType::ALLIGATOR, "Alligator"},
		{(int)RidableAnimalType::BEAR, "Bear"},
		{(int)RidableAnimalType::WOLF, "Wolf"}
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

				if (animal.IsValid())
				{
					// Make it ridable like a horse using proper RDR2 native
					PED::_SET_PED_AS_TEMP_PLAYER_HORSE(animal.GetHandle());
					PED::_SET_PED_PROMPT_NAME(animal.GetHandle(), "Ridable Animal");

					// Set flags to make it mountable
					PED::SET_PED_CONFIG_FLAG(animal.GetHandle(), (int)PedConfigFlag::BlockHorseProvokingAttack, true);
					PED::SET_PED_CONFIG_FLAG(animal.GetHandle(), (int)PedConfigFlag::DisableMountedMeleeDamage, true);

					// Make invincible
					ENTITY::SET_ENTITY_INVINCIBLE(animal.GetHandle(), true);

					// If flying animal, disable gravity
					if (flying)
					{
						ENTITY::SET_ENTITY_HAS_GRAVITY(animal.GetHandle(), false);
						Notifications::Show("Ridable Animal", "Mount the animal and use Space to fly up, Ctrl to descend!", NotificationType::Success);
					}
					else
					{
						Notifications::Show("Ridable Animal", "Your ridable animal is ready! Mount it and explore!", NotificationType::Success);
					}

					// Scale up the animal to be ridable size
					PED::_SET_PED_SCALE(animal.GetHandle(), 1.5f);
				}

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(animalHash);
			});
		}
	};

	static SpawnRidableAnimal _SpawnRidableAnimal{"spawnridableanimal", "Spawn Ridable Animal", "Ride eagles through the sky or panthers across the land!"};
}
