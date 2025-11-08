#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	static BoolCommand _AutoRemoveBounty{"autoremovebounty", "Auto Remove Bounty", "Automatically clear all bounties (free!)"};
	static BoolCommand _AutoPayBounty{"autopaybounty", "Auto Pay Bounty", "Automatically pay bounties when they appear"};

	class AntiBounty : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto player = Self::GetPlayer();
			if (!player.IsValid())
				return;

			int bounty = LAW::GET_BOUNTY(player.GetId());

			if (bounty > 0)
			{
				if (_AutoRemoveBounty.GetState())
				{
					// Clear bounty completely (free!)
					LAW::CLEAR_BOUNTY(player.GetId());

					// Also clear past crimes
					LAW::CLEAR_PLAYER_PAST_CRIMES(player.GetId());

					// Set wanted score to 0
					LAW::SET_WANTED_SCORE(player.GetId(), 0);
				}
			}
		}
	};

	static AntiBounty _AntiBounty{"antibounty", "Anti-Bounty System", "Manage bounties automatically"};
}
