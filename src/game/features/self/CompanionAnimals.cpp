#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Enums.hpp"
#include "game/rdr/Natives.hpp"
#include "util/Notifications.hpp"

namespace YimMenu::Features
{
	enum class CompanionType
	{
		DOG,
		WOLF,
		COYOTE,
		COUGAR,
		DEER,
		CAT,
		EAGLE,
		CROW,
		RABBIT,
		BEAR
	};

	static const std::pair<CompanionType, const char*> g_CompanionTypes[] = {
		{CompanionType::DOG, "Dog"},
		{CompanionType::WOLF, "Wolf"},
		{CompanionType::COYOTE, "Coyote"},
		{CompanionType::COUGAR, "Cougar"},
		{CompanionType::DEER, "Deer"},
		{CompanionType::CAT, "Cat"},
		{CompanionType::EAGLE, "Eagle"},
		{CompanionType::CROW, "Crow"},
		{CompanionType::RABBIT, "Rabbit"},
		{CompanionType::BEAR, "Bear"}
	};

	static ListCommand _CompanionType{"companiontype", "Companion Animal", "Choose your animal companion", g_CompanionTypes, (int)CompanionType::DOG};

	class SpawnCompanion : public Command
	{
		using Command::Command;

		Hash GetAnimalHash(CompanionType type)
		{
			switch (type)
			{
			case CompanionType::DOG:
				return "a_c_dog_shepherd"_J;
			case CompanionType::WOLF:
				return "a_c_wolf"_J;
			case CompanionType::COYOTE:
				return "a_c_coyote"_J;
			case CompanionType::COUGAR:
				return "a_c_cougar"_J;
			case CompanionType::DEER:
				return "a_c_deer"_J;
			case CompanionType::CAT:
				return "a_c_cat_01"_J;
			case CompanionType::EAGLE:
				return "a_c_eagle_01"_J;
			case CompanionType::CROW:
				return "a_c_crow_01"_J;
			case CompanionType::RABBIT:
				return "a_c_rabbit_01"_J;
			case CompanionType::BEAR:
				return "a_c_bear_01"_J;
			default:
				return "a_c_dog_shepherd"_J;
			}
		}

		virtual void OnCall() override
		{
			FiberPool::Push([this] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				auto type = (CompanionType)_CompanionType.GetState();
				Hash animalHash = GetAnimalHash(type);

				// Request model
				STREAMING::REQUEST_MODEL(animalHash, false);
				while (!STREAMING::HAS_MODEL_LOADED(animalHash))
					ScriptMgr::Yield();

				// Get player position
				auto pos = player.GetPosition();

				// Spawn companion slightly in front of player
				Ped companion = PED::CREATE_PED(animalHash, pos.x + 2.0f, pos.y + 2.0f, pos.z, 0.0f, true, true, true, true);

				if (companion)
				{
					// Make friendly
					PED::SET_PED_AS_NO_LONGER_NEEDED(&companion);
					PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(companion, true);
					PED::_SET_PED_PERSONALITY(companion, "DISPOSIT ION_FRIENDLY"_J);

					// Set as companion
					PED::SET_PED_AS_GROUP_MEMBER(companion, PED::GET_PED_GROUP_INDEX(player.GetHandle()));

					// Make invincible
					ENTITY::SET_ENTITY_INVINCIBLE(companion, true);

					// Set relationship
					PED::SET_PED_RELATIONSHIP_GROUP_HASH(companion, "PLAYER"_J);

					Notifications::Show("Companion Spawned", "Your companion is ready to follow you!", NotificationType::Success);
				}

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(animalHash);
			});
		}
	};

	static SpawnCompanion _SpawnCompanion{"spawncompanion", "Spawn Companion", "Spawn a friendly animal companion that follows you"};
}
