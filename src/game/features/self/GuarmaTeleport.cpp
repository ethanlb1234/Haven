#include "core/commands/Command.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "util/Notifications.hpp"
#include "util/teleport.hpp"

namespace YimMenu::Features
{
	class TeleportToGuarma : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				// Guarma coordinates (island south of map)
				rage::fvector3 guarmaPos = {1245.0f, -6950.0f, 40.0f};

				// Teleport player
				Teleport::TeleportPlayerToCoords(Self::GetPlayer(), guarmaPos);

				// Load the area
				STREAMING::_LOAD_SCENARIO_BLOCKING(guarmaPos.x, guarmaPos.y, guarmaPos.z, 100.0f, 0);

				Notifications::Show("Guarma", "Welcome to the restricted island! Explore freely!", NotificationType::Success);
			});
		}
	};

	static TeleportToGuarma _TeleportToGuarma{"teleporttoguarma", "Teleport to Guarma", "Visit the hidden island! Fully functional with NPCs"};

	class TeleportToSisika : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				// Sisika Penitentiary coordinates
				rage::fvector3 sisikaPos = {3300.0f, -660.0f, 44.0f};

				Teleport::TeleportPlayerToCoords(Self::GetPlayer(), sisikaPos);

				STREAMING::_LOAD_SCENARIO_BLOCKING(sisikaPos.x, sisikaPos.y, sisikaPos.z, 100.0f, 0);

				Notifications::Show("Sisika Penitentiary", "Explore the prison island!", NotificationType::Success);
			});
		}
	};

	static TeleportToSisika _TeleportToSisika{"teleporttosisika", "Teleport to Sisika", "Visit the prison island!"};
}
