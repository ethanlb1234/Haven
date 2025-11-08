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
			// TODO: RDR2 doesn't have GET_BOUNTY, CLEAR_PLAYER_CRIMINAL_RECORD, or PLAYER_MONEY_CHANGE natives
			// Need to find RDR2-specific bounty handling
			/*
			auto player = Self::GetPlayer();
			if (!player.IsValid())
				return;

			float bounty = PLAYER::GET_BOUNTY(player.GetId());

			if (bounty > 0.0f)
			{
				if (_AutoRemoveBounty.GetState())
				{
					// Clear bounty completely (free!)
					PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player.GetId());

					// Also clear crimes
					LAW::CLEAR_PLAYER_CRIMINAL_RECORD(player.GetId());
				}
				else if (_AutoPayBounty.GetState())
				{
					// Pay off bounty (costs money but legit)
					MONEY::PLAYER_MONEY_CHANGE(-bounty, 0);
					PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player.GetId());
				}
			}
			*/
		}
	};

	static AntiBounty _AntiBounty{"antibounty", "Anti-Bounty System", "Manage bounties automatically"};
}
