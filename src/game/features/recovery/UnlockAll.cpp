#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class UnlockAllEmotes : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock all emotes (Story Mode + Online)
				for (int i = 2000; i < 2100; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
				}
				Notifications::Show("Emotes Unlocked", "All emotes unlocked! Check your emote wheel.", NotificationType::Success);
			});
		}
	};

	static UnlockAllEmotes _UnlockAllEmotes{"unlockemotes", "Unlock All Emotes", "Unlock all story mode and online emotes"};

	class UnlockAllHorseItems : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock all horse tack, saddles, masks, etc.
				for (int i = 3000; i < 3500; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
				}
				Notifications::Show("Horse Items Unlocked", "All horse equipment unlocked! Check stables.", NotificationType::Success);
			});
		}
	};

	static UnlockAllHorseItems _UnlockAllHorseItems{"unlockhorses", "Unlock All Horse Items", "Unlock all saddles, masks, and horse equipment"};

	class UnlockAllWeapons : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock all weapons and variants
				for (int i = 4000; i < 4500; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
				}
				Notifications::Show("Weapons Unlocked", "All weapons and variants unlocked!", NotificationType::Success);
			});
		}
	};

	static UnlockAllWeapons _UnlockAllWeapons{"unlockweapons", "Unlock All Weapons", "Unlock all weapons and weapon variants"};

	class UnlockAllCampThemes : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock all camp upgrades and themes
				for (int i = 5000; i < 5100; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
				}
				Notifications::Show("Camp Unlocked", "All camp themes and upgrades unlocked!", NotificationType::Success);
			});
		}
	};

	static UnlockAllCampThemes _UnlockAllCampThemes{"unlockcamps", "Unlock All Camp Themes", "Unlock all camp themes and decorations"};

	class UnlockEverything : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// UNLOCK EVERYTHING AT ONCE!
				// Clothing (0-2000)
				// Emotes (2000-2100)
				// Horse Items (3000-3500)
				// Weapons (4000-4500)
				// Camp Themes (5000-5100)
				// Outlaw Passes (1000000+)

				int totalUnlocked = 0;

				// Clothing
				for (int i = 0; i < 2000; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
					totalUnlocked++;
				}

				// Emotes
				for (int i = 2000; i < 2100; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
					totalUnlocked++;
				}

				// Horse Items
				for (int i = 3000; i < 3500; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
					totalUnlocked++;
				}

				// Weapons
				for (int i = 4000; i < 4500; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
					totalUnlocked++;
				}

				// Camp Themes
				for (int i = 5000; i < 5100; i++)
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
					totalUnlocked++;
				}

				// All 5 Outlaw Passes
				for (int pass = 1; pass <= 5; pass++)
				{
					for (int rank = 0; rank <= 100; rank++)
					{
						UNLOCK::UNLOCK_SET_UNLOCKED(1000000 + (pass * 1000) + rank, true);
						totalUnlocked++;
					}
				}

				Notifications::Show("EVERYTHING UNLOCKED!",
					std::format("Unlocked {} items! Clothing, Emotes, Horses, Weapons, Camp, All Outlaw Passes!", totalUnlocked),
					NotificationType::Success);
			});
		}
	};

	static UnlockEverything _UnlockEverything{"unlockeverything", "UNLOCK EVERYTHING", "Unlock ALL items in the game at once! (Story Mode + Online)"};
}
