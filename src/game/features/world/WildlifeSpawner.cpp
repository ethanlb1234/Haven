#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "game/rdr/Ped.hpp"
#include "core/frontend/Notifications.hpp"
#include <vector>

namespace YimMenu::Features
{
	// Wildlife Spawner - Spawn any animal in RDR2!

	enum class AnimalType
	{
		BEAR,
		WOLF,
		COUGAR,
		PANTHER,
		ALLIGATOR,
		BISON,
		ELK,
		DEER,
		LEGENDARY_BEAR,
		LEGENDARY_WOLF,
		LEGENDARY_PANTHER,
		LEGENDARY_BISON,
		EAGLE,
		HAWK,
		DOG,
		CAT,
		HORSE_WILD,
		LEGENDARY_BOAR,
		LEGENDARY_RAM,
		LEGENDARY_MOOSE
	};

	static const std::pair<int, const char*> g_AnimalTypes[] = {
		{(int)AnimalType::BEAR, "Grizzly Bear"},
		{(int)AnimalType::WOLF, "Wolf"},
		{(int)AnimalType::COUGAR, "Cougar"},
		{(int)AnimalType::PANTHER, "Panther"},
		{(int)AnimalType::ALLIGATOR, "Alligator"},
		{(int)AnimalType::BISON, "American Bison"},
		{(int)AnimalType::ELK, "Elk"},
		{(int)AnimalType::DEER, "White-Tail Deer"},
		{(int)AnimalType::LEGENDARY_BEAR, "Legendary Bharati Grizzly"},
		{(int)AnimalType::LEGENDARY_WOLF, "Legendary Wolf"},
		{(int)AnimalType::LEGENDARY_PANTHER, "Legendary Panther"},
		{(int)AnimalType::LEGENDARY_BISON, "Legendary White Bison"},
		{(int)AnimalType::EAGLE, "Eagle"},
		{(int)AnimalType::HAWK, "Red-Tailed Hawk"},
		{(int)AnimalType::DOG, "Dog"},
		{(int)AnimalType::CAT, "Cat"},
		{(int)AnimalType::HORSE_WILD, "Wild Mustang"},
		{(int)AnimalType::LEGENDARY_BOAR, "Legendary Boar"},
		{(int)AnimalType::LEGENDARY_RAM, "Legendary Ram"},
		{(int)AnimalType::LEGENDARY_MOOSE, "Legendary Moose"}
	};

	static ListCommand _AnimalToSpawn{"animaltospawn", "Animal Type", "Choose animal to spawn", g_AnimalTypes, 0};

	static std::vector<Ped> g_SpawnedAnimals;

	const char* GetAnimalModel(AnimalType type)
	{
		switch (type)
		{
		case AnimalType::BEAR:
			return "A_C_Bear_01";
		case AnimalType::WOLF:
			return "A_C_Wolf";
		case AnimalType::COUGAR:
			return "A_C_Cougar_01";
		case AnimalType::PANTHER:
			return "A_C_Panther_01";
		case AnimalType::ALLIGATOR:
			return "A_C_Alligator_01";
		case AnimalType::BISON:
			return "A_C_Bison_01";
		case AnimalType::ELK:
			return "A_C_Elk_01";
		case AnimalType::DEER:
			return "A_C_Deer_01";
		case AnimalType::LEGENDARY_BEAR:
			return "A_C_BearBlack_01";
		case AnimalType::LEGENDARY_WOLF:
			return "A_C_Wolf_Medium";
		case AnimalType::LEGENDARY_PANTHER:
			return "A_C_Panther_01";
		case AnimalType::LEGENDARY_BISON:
			return "A_C_Bison_01";
		case AnimalType::EAGLE:
			return "A_C_Eagle_01";
		case AnimalType::HAWK:
			return "A_C_Hawk_01";
		case AnimalType::DOG:
			return "A_C_DogRufus_01";
		case AnimalType::CAT:
			return "A_C_Cat_01";
		case AnimalType::HORSE_WILD:
			return "A_C_Horse_Mustang_GrulloDun";
		case AnimalType::LEGENDARY_BOAR:
			return "A_C_Boar_01";
		case AnimalType::LEGENDARY_RAM:
			return "A_C_Sheep_01";
		case AnimalType::LEGENDARY_MOOSE:
			return "A_C_Moose_01";
		default:
			return "A_C_Deer_01";
		}
	}

	class SpawnWildlife : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				AnimalType type = (AnimalType)_AnimalToSpawn.GetState();
				const char* modelName = GetAnimalModel(type);
				Hash model = rage::joaat(modelName);

				STREAMING::REQUEST_MODEL(model, false);
				int attempts = 0;
				while (!STREAMING::HAS_MODEL_LOADED(model) && attempts < 100)
				{
					ScriptMgr::Yield();
					attempts++;
				}

				if (!STREAMING::HAS_MODEL_LOADED(model))
				{
					Notifications::Show("Wildlife Spawner", "Failed to load animal model!", NotificationType::Error);
					return;
				}

				auto pos = ped.GetPosition();
				float heading = ENTITY::GET_ENTITY_HEADING(ped.GetHandle());

				// Spawn in front of player
				pos.x += 5.0f * cos(heading * (3.14159f / 180.0f));
				pos.y += 5.0f * sin(heading * (3.14159f / 180.0f));

				Ped animal = PED::CREATE_PED(model, pos.x, pos.y, pos.z, heading, true, true, true, true);

				if (animal.IsValid())
				{
					// Make legendary animals glow
					if (type == AnimalType::LEGENDARY_BEAR || type == AnimalType::LEGENDARY_WOLF ||
						type == AnimalType::LEGENDARY_PANTHER || type == AnimalType::LEGENDARY_BISON ||
						type == AnimalType::LEGENDARY_BOAR || type == AnimalType::LEGENDARY_RAM ||
						type == AnimalType::LEGENDARY_MOOSE)
					{
						ENTITY::_SET_ENTITY_LIGHTS_ENABLED(animal.GetHandle(), true);
						animal.SetAlpha(200);
					}

					// Set as perfect pelt quality
					animal.SetQuality(2);

					g_SpawnedAnimals.push_back(animal);

					Notifications::Show("Wildlife Spawner", std::format("Spawned {}!", g_AnimalTypes[(int)type].second), NotificationType::Success);
				}

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			});
		}
	};

	static SpawnWildlife _SpawnWildlife{"spawnwildlife", "Spawn Animal", "Spawn selected animal"};

	class MakeAnimalCompanion : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (g_SpawnedAnimals.empty())
			{
				Notifications::Show("Wildlife Spawner", "Spawn an animal first!", NotificationType::Error);
				return;
			}

			FiberPool::Push([] {
				auto lastAnimal = g_SpawnedAnimals.back();
				if (!lastAnimal.IsValid())
					return;

				// Make it follow player
				int group = PED::GET_PED_GROUP_INDEX(Self::GetPed().GetHandle());
				if (!PED::DOES_GROUP_EXIST(group))
				{
					group = PED::CREATE_GROUP(0);
					PED::SET_PED_AS_GROUP_LEADER(Self::GetPed().GetHandle(), group, false);
				}

				PED::SET_PED_AS_GROUP_MEMBER(lastAnimal.GetHandle(), group);
				PED::SET_PED_RELATIONSHIP_GROUP_HASH(lastAnimal.GetHandle(), PED::GET_PED_RELATIONSHIP_GROUP_HASH(Self::GetPed().GetHandle()));

				// Disable fleeing
				FLOCK::SET_ANIMAL_TUNING_FLOAT_PARAM(lastAnimal.GetHandle(), 104, 0.0);
				FLOCK::SET_ANIMAL_TUNING_FLOAT_PARAM(lastAnimal.GetHandle(), 105, 0.0);

				Notifications::Show("Wildlife Spawner", "Animal is now your companion!", NotificationType::Success);
			});
		}
	};

	static MakeAnimalCompanion _MakeAnimalCompanion{"makeanimalcompanion", "Make Companion", "Last spawned animal follows you"};

	class DeleteAllAnimals : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto& animal : g_SpawnedAnimals)
			{
				if (animal.IsValid())
					animal.Delete();
			}
			g_SpawnedAnimals.clear();
			Notifications::Show("Wildlife Spawner", "Deleted all spawned animals!", NotificationType::Info);
		}
	};

	static DeleteAllAnimals _DeleteAllAnimals{"deleteallanimals", "Delete All Animals", "Remove all spawned animals"};
}
