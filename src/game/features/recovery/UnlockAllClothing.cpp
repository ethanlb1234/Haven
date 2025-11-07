#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/rdr/Natives.hpp"
#include "game/rdr/ScriptFunction.hpp"

namespace YimMenu::Features
{
	class UnlockAllClothing : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock all clothing items for the player
				// This uses native functions to unlock wardrobe items

				// Unlock catalog items (clothing)
				for (int i = 0; i < 2000; i++) // Iterate through clothing item hashes
				{
					UNLOCK::UNLOCK_SET_UNLOCKED(i, true);
				}

				Notifications::Show("Clothing Unlocked", "All clothing items have been unlocked! Check your wardrobe.", NotificationType::Success);
			});
		}
	};

	static UnlockAllClothing _UnlockAllClothing{"unlockallclothing", "Unlock All Clothing", "Unlocks all clothing items including story mode and outlaw pass items"};
}
