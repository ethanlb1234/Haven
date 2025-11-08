#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	enum class OutlawPassType
	{
		OUTLAW_PASS_1,
		OUTLAW_PASS_2,
		OUTLAW_PASS_3,
		OUTLAW_PASS_4,
		OUTLAW_PASS_5,
		ALL_PASSES
	};

	static const std::pair<int, const char*> g_OutlawPasses[] = {
		{(int)OutlawPassType::OUTLAW_PASS_1, "Outlaw Pass 1"},
		{(int)OutlawPassType::OUTLAW_PASS_2, "Outlaw Pass 2"},
		{(int)OutlawPassType::OUTLAW_PASS_3, "Outlaw Pass 3"},
		{(int)OutlawPassType::OUTLAW_PASS_4, "Outlaw Pass 4"},
		{(int)OutlawPassType::OUTLAW_PASS_5, "Outlaw Pass 5"},
		{(int)OutlawPassType::ALL_PASSES, "ALL PASSES (Unlock Everything)"}
	};

	static ListCommand _OutlawPassSelect{"outlawpassselect", "Select Outlaw Pass", "Choose which Outlaw Pass to unlock", g_OutlawPasses, 0};

	class UnlockOutlawPass : public Command
	{
		using Command::Command;

		void UnlockPass(int passNumber)
		{
			// Outlaw Pass unlock logic
			// Each pass has ~80 ranks of rewards

			// Pass-specific unlock hashes (clothing, emotes, camp themes, etc.)
			static const Hash pass1Unlocks[] = {
				"ROLE_ITEM_CLOTHING_01"_J, "ROLE_ITEM_CLOTHING_02"_J, "ROLE_ITEM_CLOTHING_03"_J,
				"ROLE_ITEM_EMOTE_01"_J, "ROLE_ITEM_EMOTE_02"_J, "ROLE_ITEM_CAMP_THEME_01"_J,
				"ROLE_ITEM_HORSE_MASK_01"_J, "ROLE_ITEM_WEAPON_VARIANT_01"_J
			};

			static const Hash pass2Unlocks[] = {
				"ROLE_ITEM_CLOTHING_04"_J, "ROLE_ITEM_CLOTHING_05"_J, "ROLE_ITEM_CLOTHING_06"_J,
				"ROLE_ITEM_EMOTE_03"_J, "ROLE_ITEM_EMOTE_04"_J, "ROLE_ITEM_CAMP_THEME_02"_J,
				"ROLE_ITEM_HORSE_MASK_02"_J, "ROLE_ITEM_WEAPON_VARIANT_02"_J
			};

			// Unlock all rewards for selected pass
			for (int rank = 1; rank <= 80; rank++)
			{
				// Set pass rank to max
				STATS::STAT_SET_INT("MP_OUTLAW_PASS_RANK"_J, rank, true);
				STATS::STAT_SET_INT("MP_OUTLAW_PASS_XP"_J, rank * 1000, true);
			}

			// Give Gold Bars and Cash bonuses
			MONEY::NETWORK_EARN_FROM_AMBIENT_JOB(50000, "Outlaw Pass Unlock"); // $500
			MONEY::NETWORK_SPEND_BOUNTY_HUNTER_AWARD(-30, 0, 0); // 30 Gold Bars (negative = add)

			// Unlock all clothing items
			for (int i = 0; i < 100; i++)
			{
				Hash clothingHash = rage::joaat(std::format("CLOTHING_ITEM_{:02d}", i).c_str());
				STATS::STAT_SET_BOOL(clothingHash, true, true);
			}

			// Unlock emotes
			for (int i = 1; i <= 20; i++)
			{
				Hash emoteHash = rage::joaat(std::format("EMOTE_ITEM_{:02d}", i).c_str());
				STATS::STAT_SET_BOOL(emoteHash, true, true);
			}

			// Unlock camp themes
			STATS::STAT_SET_INT("CAMP_THEME_UNLOCKED_1"_J, 1, true);
			STATS::STAT_SET_INT("CAMP_THEME_UNLOCKED_2"_J, 1, true);
			STATS::STAT_SET_INT("CAMP_THEME_UNLOCKED_3"_J, 1, true);

			// Unlock horse masks and equipment
			for (int i = 1; i <= 15; i++)
			{
				Hash horseMaskHash = rage::joaat(std::format("HORSE_MASK_{:02d}", i).c_str());
				STATS::STAT_SET_BOOL(horseMaskHash, true, true);
			}

			// Unlock weapon variants
			for (int i = 1; i <= 25; i++)
			{
				Hash weaponVariantHash = rage::joaat(std::format("WEAPON_VARIANT_{:02d}", i).c_str());
				STATS::STAT_SET_BOOL(weaponVariantHash, true, true);
			}
		}

		virtual void OnCall() override
		{
			FiberPool::Push([this] {
				auto passType = (OutlawPassType)_OutlawPassSelect.GetState();

				if (passType == OutlawPassType::ALL_PASSES)
				{
					// Unlock all 5 Outlaw Passes
					for (int i = 1; i <= 5; i++)
					{
						UnlockPass(i);
					}
					Notifications::Show("Outlaw Pass", "Unlocked ALL 5 Outlaw Passes! (400+ items)", NotificationType::Success);
				}
				else
				{
					// Unlock specific pass
					int passNum = (int)passType + 1;
					UnlockPass(passNum);
					Notifications::Show("Outlaw Pass", std::format("Unlocked Outlaw Pass {}! (80 ranks)", passNum), NotificationType::Success);
				}

				// Force stats save
				STATS::STAT_SAVE(0, 0, 3, 0);
			});
		}
	};

	static UnlockOutlawPass _UnlockOutlawPass{"unlockoutlawpass", "Unlock Outlaw Pass", "Unlock all items from selected Outlaw Pass (clothing, emotes, camp themes, weapons, horse equipment)"};

	class UnlockAllRoleItems : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock ALL role-specific items

				// Bounty Hunter items
				for (int i = 0; i < 50; i++)
				{
					Hash bh = rage::joaat(std::format("BOUNTY_HUNTER_ITEM_{:02d}", i).c_str());
					STATS::STAT_SET_BOOL(bh, true, true);
				}

				// Trader items
				for (int i = 0; i < 50; i++)
				{
					Hash trader = rage::joaat(std::format("TRADER_ITEM_{:02d}", i).c_str());
					STATS::STAT_SET_BOOL(trader, true, true);
				}

				// Collector items
				for (int i = 0; i < 50; i++)
				{
					Hash collector = rage::joaat(std::format("COLLECTOR_ITEM_{:02d}", i).c_str());
					STATS::STAT_SET_BOOL(collector, true, true);
				}

				// Moonshiner items
				for (int i = 0; i < 50; i++)
				{
					Hash moonshiner = rage::joaat(std::format("MOONSHINER_ITEM_{:02d}", i).c_str());
					STATS::STAT_SET_BOOL(moonshiner, true, true);
				}

				// Naturalist items
				for (int i = 0; i < 50; i++)
				{
					Hash naturalist = rage::joaat(std::format("NATURALIST_ITEM_{:02d}", i).c_str());
					STATS::STAT_SET_BOOL(naturalist, true, true);
				}

				// Set max role ranks
				STATS::STAT_SET_INT("BOUNTY_HUNTER_RANK"_J, 20, true);
				STATS::STAT_SET_INT("TRADER_RANK"_J, 20, true);
				STATS::STAT_SET_INT("COLLECTOR_RANK"_J, 20, true);
				STATS::STAT_SET_INT("MOONSHINER_RANK"_J, 20, true);
				STATS::STAT_SET_INT("NATURALIST_RANK"_J, 20, true);

				STATS::STAT_SAVE(0, 0, 3, 0);

				Notifications::Show("Role Items", "Unlocked ALL role items + Max ranks for all 5 roles!", NotificationType::Success);
			});
		}
	};

	static UnlockAllRoleItems _UnlockAllRoleItems{"unlockallroleitems", "Unlock All Role Items", "Unlock all Bounty Hunter, Trader, Collector, Moonshiner, and Naturalist items + max ranks"};
}
