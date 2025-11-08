#include "core/commands/BoolCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "game/rdr/Ped.hpp"
#include "core/frontend/Notifications.hpp"
#include <vector>
#include <random>

namespace YimMenu::Features
{
	// COD Zombies - Spawn at Braithwaite Manor, survive waves, buy weapons with points!

	static BoolCommand _CODZombiesActive{"codzombiesactive", "COD Zombies Mode", "Survive zombie hordes at Braithwaite Manor"};

	struct ZombieData
	{
		Ped ped;
		int health;
	};

	static std::vector<ZombieData> g_Zombies;
	static int g_CurrentWave = 0;
	static int g_Points = 500; // Start with 500 points
	static int g_ZombiesRemaining = 0;
	static bool g_GameStarted = false;
	static bool g_HasPistol = false;
	static Vector3 g_BraithwaiteManor = {1007.0f, -1740.0f, 48.0f}; // Braithwaite Manor coords

	// Weapon shop prices
	struct WeaponShop
	{
		const char* name;
		Hash hash;
		int price;
	};

	static WeaponShop g_Weapons[] = {
		{"Cattleman Revolver", "WEAPON_REVOLVER_CATTLEMAN"_J, 0},      // Free starter
		{"Double Action", "WEAPON_REVOLVER_DOUBLEACTION"_J, 750},
		{"Pump Shotgun", "WEAPON_SHOTGUN_PUMP"_J, 1500},
		{"Repeating Shotgun", "WEAPON_SHOTGUN_REPEATING"_J, 2000},
		{"Lancaster Repeater", "WEAPON_RIFLE_SPRINGFIELD"_J, 2500},
		{"Bolt Action Rifle", "WEAPON_RIFLE_BOLTACTION"_J, 3500},
		{"Carcano Rifle", "WEAPON_SNIPERRIFLE_CARCANO"_J, 5000},
		{"Mauser Pistol", "WEAPON_PISTOL_MAUSER"_J, 4000},
		{"Volcanic Pistol", "WEAPON_PISTOL_VOLCANIC"_J, 3000}
	};

	static void StartGame()
	{
		g_CurrentWave = 0;
		g_Points = 500;
		g_Zombies.clear();
		g_GameStarted = true;
		g_HasPistol = false;

		// Teleport to Braithwaite Manor
		auto ped = Self::GetPed();
		if (ped.IsValid())
		{
			ENTITY::SET_ENTITY_COORDS(ped.GetHandle(), g_BraithwaiteManor.x, g_BraithwaiteManor.y, g_BraithwaiteManor.z, false, false, false, false);

			// Remove all weapons first
			WEAPON::REMOVE_ALL_PED_WEAPONS(ped.GetHandle(), true, true);

			// Give starter pistol
			WEAPON::GIVE_WEAPON_TO_PED(ped.GetHandle(), "WEAPON_REVOLVER_CATTLEMAN"_J, 500, true, true, 0, false, 0.5f, 1.0f, 0x2CD419DC, false, 0.0f, false);
			g_HasPistol = true;

			Notifications::Show("COD Zombies", "Game Started! Wave 1 incoming...", NotificationType::Success);
		}
	}

	static void SpawnZombieWave(int wave)
	{
		int zombieCount = 4 + (wave * 2); // 6, 8, 10, 12...
		if (zombieCount > 50)
			zombieCount = 50;

		g_ZombiesRemaining = zombieCount;

		auto playerPed = Self::GetPed();
		if (!playerPed.IsValid())
			return;

		Vector3 playerPos = playerPed.GetPosition();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> angleDist(0.0f, 360.0f);
		std::uniform_real_distribution<float> distDist(30.0f, 60.0f);

		for (int i = 0; i < zombieCount; i++)
		{
			float angle = angleDist(gen) * (3.14159f / 180.0f);
			float dist = distDist(gen);

			Vector3 spawnPos;
			spawnPos.x = playerPos.x + cos(angle) * dist;
			spawnPos.y = playerPos.y + sin(angle) * dist;
			spawnPos.z = playerPos.z;

			// Get ground Z
			float groundZ;
			if (MISC::GET_GROUND_Z_FOR_3D_COORD(spawnPos.x, spawnPos.y, spawnPos.z + 100.0f, &groundZ, false))
				spawnPos.z = groundZ;

			// Spawn zombie (using frozen dead body model for creepy effect)
			Ped zombie = Ped::Create("RE_FROZENTODEATH_MALES_01"_J, spawnPos, 0.0f);
			if (zombie.IsValid())
			{
				// Make zombie aggressive
				PED::SET_PED_RELATIONSHIP_GROUP_HASH(zombie.GetHandle(), "WILD_ANIMAL"_J);
				TASK::TASK_COMBAT_PED(zombie.GetHandle(), playerPed.GetHandle(), 0, 16);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(zombie.GetHandle(), true);
				PED::SET_PED_COMBAT_ATTRIBUTES(zombie.GetHandle(), 46, true); // Always fight

				// Scale health with wave
				int health = 100 + (wave * 50);
				zombie.SetHealth(health);

				// Speed increases with waves
				if (wave > 5)
					PED::SET_PED_MOVE_RATE_OVERRIDE(zombie.GetHandle(), 1.2f + (wave * 0.1f));

				ZombieData zData;
				zData.ped = zombie;
				zData.health = health;
				g_Zombies.push_back(zData);
			}
		}

		Notifications::Show("COD Zombies", std::format("Wave {} - {} zombies!", wave, zombieCount), NotificationType::Warning);
	}

	class CODZombiesGame : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!_CODZombiesActive.GetState())
				return;

			auto playerPed = Self::GetPed();
			if (!playerPed.IsValid())
				return;

			// Start game if not started
			if (!g_GameStarted)
			{
				StartGame();
				SpawnZombieWave(1);
				g_CurrentWave = 1;
				return;
			}

			// Check if player died
			if (playerPed.GetHealth() <= 0)
			{
				Notifications::Show("COD Zombies", std::format("Game Over! Wave: {} | Points: {}", g_CurrentWave, g_Points), NotificationType::Error);
				_CODZombiesActive.SetState(false);
				g_GameStarted = false;

				// Cleanup zombies
				for (auto& zombie : g_Zombies)
				{
					if (zombie.ped.IsValid())
						zombie.ped.Delete();
				}
				g_Zombies.clear();
				return;
			}

			// Update zombies and award points
			for (auto it = g_Zombies.begin(); it != g_Zombies.end();)
			{
				if (!it->ped.IsValid() || !ENTITY::DOES_ENTITY_EXIST(it->ped.GetHandle()))
				{
					it = g_Zombies.erase(it);
					g_ZombiesRemaining--;
					continue;
				}

				if (ENTITY::IS_ENTITY_DEAD(it->ped.GetHandle()))
				{
					// Award points
					g_Points += 50 + (g_CurrentWave * 10); // More points in higher waves

					// Delete dead zombie
					it->ped.Delete();
					it = g_Zombies.erase(it);
					g_ZombiesRemaining--;
					continue;
				}

				++it;
			}

			// Check if wave complete
			if (g_Zombies.empty() && g_GameStarted)
			{
				g_CurrentWave++;
				Notifications::Show("COD Zombies", std::format("Wave Complete! Points: {}", g_Points), NotificationType::Success);
				ScriptMgr::Yield(3000); // 3 second break
				SpawnZombieWave(g_CurrentWave);
			}

			// Draw HUD using UIDEBUG natives
			std::string waveText = std::format("Wave: {}", g_CurrentWave);
			UIDEBUG::_BG_SET_TEXT_SCALE(0.5f, 0.5f);
			UIDEBUG::_BG_SET_TEXT_COLOR(255, 50, 50, 255);
			UIDEBUG::_BG_DISPLAY_TEXT(MISC::VAR_STRING(10, "LITERAL_STRING", waveText.c_str()), 0.02f, 0.02f);

			std::string pointsText = std::format("Points: {}", g_Points);
			UIDEBUG::_BG_SET_TEXT_SCALE(0.5f, 0.5f);
			UIDEBUG::_BG_SET_TEXT_COLOR(100, 255, 100, 255);
			UIDEBUG::_BG_DISPLAY_TEXT(MISC::VAR_STRING(10, "LITERAL_STRING", pointsText.c_str()), 0.02f, 0.06f);

			std::string zombiesText = std::format("Zombies: {}", g_ZombiesRemaining);
			UIDEBUG::_BG_SET_TEXT_SCALE(0.5f, 0.5f);
			UIDEBUG::_BG_SET_TEXT_COLOR(255, 200, 100, 255);
			UIDEBUG::_BG_DISPLAY_TEXT(MISC::VAR_STRING(10, "LITERAL_STRING", zombiesText.c_str()), 0.02f, 0.10f);
		}

		virtual void OnDisable() override
		{
			// Cleanup all zombies
			for (auto& zombie : g_Zombies)
			{
				if (zombie.ped.IsValid())
					zombie.ped.Delete();
			}
			g_Zombies.clear();
			g_GameStarted = false;
		}
	};

	static CODZombiesGame _CODZombiesGame{"codzombiesgame", "COD Zombies Game Loop", ""};

	// Weapon shop commands
	class BuyWeapon : public Command
	{
		int m_WeaponIndex;

	public:
		BuyWeapon(const char* id, const char* label, int weaponIndex) :
			Command(id, label, ""),
			m_WeaponIndex(weaponIndex)
		{
		}

		virtual void OnCall() override
		{
			if (!g_GameStarted)
			{
				Notifications::Show("COD Zombies", "Start the game first!", NotificationType::Error);
				return;
			}

			WeaponShop& weapon = g_Weapons[m_WeaponIndex];

			if (g_Points < weapon.price)
			{
				Notifications::Show("COD Zombies", std::format("Need {} points! You have {}", weapon.price, g_Points), NotificationType::Error);
				return;
			}

			auto ped = Self::GetPed();
			if (ped.IsValid())
			{
				WEAPON::GIVE_WEAPON_TO_PED(ped.GetHandle(), weapon.hash, 500, true, true, 0, false, 0.5f, 1.0f, 0x2CD419DC, false, 0.0f, false);
				g_Points -= weapon.price;
				Notifications::Show("COD Zombies", std::format("Bought {}! Points remaining: {}", weapon.name, g_Points), NotificationType::Success);
			}
		}
	};

	static BuyWeapon _BuyRevolver{"buycattleman", "Cattleman Revolver (FREE)", 0};
	static BuyWeapon _BuyDoubleAction{"buydoubleaction", "Double Action (750 pts)", 1};
	static BuyWeapon _BuyPumpShotgun{"buypumpshotgun", "Pump Shotgun (1500 pts)", 2};
	static BuyWeapon _BuyRepeatingShotgun{"buyrepeatingshotgun", "Repeating Shotgun (2000 pts)", 3};
	static BuyWeapon _BuyLancaster{"buylancaster", "Lancaster Repeater (2500 pts)", 4};
	static BuyWeapon _BuyBoltAction{"buyboltaction", "Bolt Action (3500 pts)", 5};
	static BuyWeapon _BuyCarcano{"buycarcano", "Carcano Rifle (5000 pts)", 6};
	static BuyWeapon _BuyMauser{"buymauser", "Mauser Pistol (4000 pts)", 7};
	static BuyWeapon _BuyVolcanic{"buyvolcanic", "Volcanic Pistol (3000 pts)", 8};
}
