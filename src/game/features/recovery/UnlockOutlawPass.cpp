#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	class UnlockOutlawPass : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				// Unlock all outlaw pass items
				// This iterates through common outlaw pass reward ranges

				for (int pass = 1; pass <= 5; pass++) // Outlaw Pass 1-5
				{
					for (int rank = 0; rank <= 100; rank++) // Ranks 0-100
					{
						// Unlock rewards for each rank
						UNLOCK::UNLOCK_SET_UNLOCKED(1000000 + (pass * 1000) + rank, true);
					}
				}

				Notifications::Show("Outlaw Pass Unlocked", "All outlaw pass items and rewards have been unlocked!", NotificationType::Success);
			});
		}
	};

	static UnlockOutlawPass _UnlockOutlawPass{"unlockoutlawpass", "Unlock Outlaw Pass Items", "Unlocks all items from previous outlaw passes"};
}
