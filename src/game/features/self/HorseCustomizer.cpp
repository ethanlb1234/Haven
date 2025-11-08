#include "core/commands/Command.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "game/rdr/Ped.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Horse Customizer - Advanced horse customization and spawning

	enum class HorseBreed
	{
		ARABIAN_WHITE,
		ARABIAN_BLACK,
		ARABIAN_ROSE_GREY,
		MUSTANG,
		THOROUGHBRED,
		TURKOMAN_GOLD,
		TURKOMAN_DARK_BAY,
		MISSOURI_FOX_TROTTER,
		ANDALUSIAN,
		ARDENNES,
		NOKOTA,
		HUNGARIAN_HALFBRED,
		APPALOOSA,
		SHIRE,
		BRETON,
		CRIOLLO,
		NORFOLK_ROADSTER
	};

	static const std::pair<int, const char*> g_HorseBreeds[] = {
		{(int)HorseBreed::ARABIAN_WHITE, "Arabian (White)"},
		{(int)HorseBreed::ARABIAN_BLACK, "Arabian (Black)"},
		{(int)HorseBreed::ARABIAN_ROSE_GREY, "Arabian (Rose Grey Bay)"},
		{(int)HorseBreed::MUSTANG, "Mustang (Wild)"},
		{(int)HorseBreed::THOROUGHBRED, "Thoroughbred (Brindle)"},
		{(int)HorseBreed::TURKOMAN_GOLD, "Turkoman (Gold)"},
		{(int)HorseBreed::TURKOMAN_DARK_BAY, "Turkoman (Dark Bay)"},
		{(int)HorseBreed::MISSOURI_FOX_TROTTER, "Missouri Fox Trotter"},
		{(int)HorseBreed::ANDALUSIAN, "Andalusian (Rose Grey)"},
		{(int)HorseBreed::ARDENNES, "Ardennes (Strawberry Roan)"},
		{(int)HorseBreed::NOKOTA, "Nokota (White Roan)"},
		{(int)HorseBreed::HUNGARIAN_HALFBRED, "Hungarian Halfbred"},
		{(int)HorseBreed::APPALOOSA, "Appaloosa (Leopard)"},
		{(int)HorseBreed::SHIRE, "Shire (Dark Bay)"},
		{(int)HorseBreed::BRETON, "Breton (Steel Grey)"},
		{(int)HorseBreed::CRIOLLO, "Criollo (Dun Overo)"},
		{(int)HorseBreed::NORFOLK_ROADSTER, "Norfolk Roadster"}
	};

	static ListCommand _HorseBreed{"horsebreed", "Horse Breed", "Choose horse breed", g_HorseBreeds, 0};
	static BoolCommand _MaxHorseStats{"maxhorsestats", "Max Stats", "Give horse max stats"};
	static BoolCommand _MaxHorseBonding{"maxhorsebonding", "Max Bonding", "Instant level 4 bonding"};

	const char* GetHorseModel(HorseBreed breed)
	{
		switch (breed)
		{
		case HorseBreed::ARABIAN_WHITE:
			return "A_C_Horse_Arabian_White";
		case HorseBreed::ARABIAN_BLACK:
			return "A_C_Horse_Arabian_Black";
		case HorseBreed::ARABIAN_ROSE_GREY:
			return "A_C_Horse_Arabian_RoseGreyBay";
		case HorseBreed::MUSTANG:
			return "A_C_Horse_Mustang_GrulloDun";
		case HorseBreed::THOROUGHBRED:
			return "A_C_Horse_Thoroughbred_Brindle";
		case HorseBreed::TURKOMAN_GOLD:
			return "A_C_Horse_Turkoman_Gold";
		case HorseBreed::TURKOMAN_DARK_BAY:
			return "A_C_Horse_Turkoman_DarkBay";
		case HorseBreed::MISSOURI_FOX_TROTTER:
			return "A_C_Horse_MissouriFoxTrotter_SilverDapplePinto";
		case HorseBreed::ANDALUSIAN:
			return "A_C_Horse_Andalusian_RoseGray";
		case HorseBreed::ARDENNES:
			return "A_C_Horse_Ardennes_StrawberryRoan";
		case HorseBreed::NOKOTA:
			return "A_C_Horse_Nokota_WhiteRoan";
		case HorseBreed::HUNGARIAN_HALFBRED:
			return "A_C_Horse_HungarianHalfbred_PiebaldTobiano";
		case HorseBreed::APPALOOSA:
			return "A_C_Horse_Appaloosa_Leopard";
		case HorseBreed::SHIRE:
			return "A_C_Horse_Shire_DarkBay";
		case HorseBreed::BRETON:
			return "A_C_Horse_Breton_SteelGrey";
		case HorseBreed::CRIOLLO:
			return "A_C_Horse_Criollo_DunOvero";
		case HorseBreed::NORFOLK_ROADSTER:
			return "A_C_Horse_NorfolkRoadster_PiebaldRoan";
		default:
			return "A_C_Horse_Arabian_White";
		}
	}

	class SpawnHorse : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				HorseBreed breed = (HorseBreed)_HorseBreed.GetState();
				const char* modelName = GetHorseModel(breed);
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
					Notifications::Show("Horse Customizer", "Failed to load horse model!", NotificationType::Error);
					return;
				}

				auto pos = ped.GetPosition();
				float heading = ENTITY::GET_ENTITY_HEADING(ped.GetHandle());

				// Spawn horse
				Ped horse = PED::CREATE_PED(model, pos.x + 3.0f, pos.y, pos.z, heading, true, true, true, true);

				if (horse.IsValid())
				{
					// Set as player's horse
					PED::SET_PED_AS_MOUNT(ped.GetHandle(), horse.GetHandle());

					// Max stats if enabled
					if (_MaxHorseStats.GetState())
					{
						// Max health and stamina
						PED::SET_PED_MAX_HEALTH(horse.GetHandle(), 999);
						horse.SetHealth(999);
						PED::_CHANGE_PED_STAMINA(horse.GetHandle(), 999.0f);
					}

					// Max bonding if enabled
					if (_MaxHorseBonding.GetState())
					{
						// Set bonding level to 4 (max)
						PED::_SET_PED_BONDING_LEVEL(horse.GetHandle(), 4);
					}

					Notifications::Show("Horse Customizer", std::format("Spawned {}!", g_HorseBreeds[(int)breed].second), NotificationType::Success);
				}

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			});
		}
	};

	static SpawnHorse _SpawnHorse{"spawnhorse", "Spawn Horse", "Spawn selected horse breed"};

	class MaxCurrentHorse : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				Ped horse;

				// Check if player is on a horse
				if (PED::IS_PED_ON_MOUNT(ped.GetHandle()))
				{
					horse = PED::GET_MOUNT(ped.GetHandle());
				}
				else
				{
					// Try to get last used horse
					horse = PED::_GET_LAST_MOUNT(ped.GetHandle());
				}

				if (!horse.IsValid())
				{
					Notifications::Show("Horse Customizer", "No horse found! Spawn one first.", NotificationType::Error);
					return;
				}

				// Max stats
				PED::SET_PED_MAX_HEALTH(horse.GetHandle(), 999);
				horse.SetHealth(999);
				PED::_CHANGE_PED_STAMINA(horse.GetHandle(), 999.0f);

				// Max bonding
				PED::_SET_PED_BONDING_LEVEL(horse.GetHandle(), 4);

				// Make invincible
				horse.SetInvincible(true);

				Notifications::Show("Horse Customizer", "Maxed out your horse's stats!", NotificationType::Success);
			});
		}
	};

	static MaxCurrentHorse _MaxCurrentHorse{"maxcurrenthorse", "Max Current Horse", "Max stats and bonding for your current horse"};

	class HealHorse : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				Ped horse;

				if (PED::IS_PED_ON_MOUNT(ped.GetHandle()))
					horse = PED::GET_MOUNT(ped.GetHandle());
				else
					horse = PED::_GET_LAST_MOUNT(ped.GetHandle());

				if (!horse.IsValid())
				{
					Notifications::Show("Horse Customizer", "No horse found!", NotificationType::Error);
					return;
				}

				// Heal horse
				horse.SetHealth(horse.GetMaxHealth());
				PED::_CHANGE_PED_STAMINA(horse.GetHandle(), 100.0f);

				// Remove dirt
				PED::CLEAR_PED_WETNESS(horse.GetHandle());
				PED::CLEAR_PED_BLOOD_DAMAGE(horse.GetHandle());
				PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(horse.GetHandle(), 10, "ALL");

				Notifications::Show("Horse Customizer", "Horse healed and cleaned!", NotificationType::Success);
			});
		}
	};

	static HealHorse _HealHorse{"healhorse", "Heal Horse", "Fully heal and clean your horse"};
}
