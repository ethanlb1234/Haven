#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	enum class PlayerModel
	{
		ARTHUR,
		JOHN,
		DUTCH,
		HOSEA,
		SADIE,
		CHARLES,
		BILL,
		JAVIER,
		MICAH,
		NATIVE_WOMAN,
		GUNSLINGER,
		COWBOY,
		SHERIFF,
		OUTLAW,
		GENTLEMAN,
		LADY,
		FARMER
	};

	static const std::pair<PlayerModel, const char*> g_PlayerModels[] = {
		{PlayerModel::ARTHUR, "Arthur Morgan"},
		{PlayerModel::JOHN, "John Marston"},
		{PlayerModel::DUTCH, "Dutch van der Linde"},
		{PlayerModel::HOSEA, "Hosea Matthews"},
		{PlayerModel::SADIE, "Sadie Adler"},
		{PlayerModel::CHARLES, "Charles Smith"},
		{PlayerModel::BILL, "Bill Williamson"},
		{PlayerModel::JAVIER, "Javier Escuella"},
		{PlayerModel::MICAH, "Micah Bell"},
		{PlayerModel::NATIVE_WOMAN, "Native Woman"},
		{PlayerModel::GUNSLINGER, "Gunslinger"},
		{PlayerModel::COWBOY, "Cowboy"},
		{PlayerModel::SHERIFF, "Sheriff"},
		{PlayerModel::OUTLAW, "Outlaw"},
		{PlayerModel::GENTLEMAN, "Gentleman"},
		{PlayerModel::LADY, "Lady"},
		{PlayerModel::FARMER, "Farmer"}
	};

	static ListCommand _PlayerModelType{"playermodeltype", "Player Model", "Choose who you want to be", g_PlayerModels, (int)PlayerModel::ARTHUR};

	class ChangePlayerModel : public Command
	{
		using Command::Command;

		Hash GetModelHash(PlayerModel model)
		{
			switch (model)
			{
			case PlayerModel::ARTHUR:
				return "player_zero"_J;
			case PlayerModel::JOHN:
				return "player_three"_J;
			case PlayerModel::DUTCH:
				return "cs_dutch"_J;
			case PlayerModel::HOSEA:
				return "cs_hoseamatthews"_J;
			case PlayerModel::SADIE:
				return "cs_mrsadler"_J;
			case PlayerModel::CHARLES:
				return "cs_charlessmith"_J;
			case PlayerModel::BILL:
				return "cs_billwilliamson"_J;
			case PlayerModel::JAVIER:
				return "cs_javierescuella"_J;
			case PlayerModel::MICAH:
				return "cs_micahbell"_J;
			case PlayerModel::NATIVE_WOMAN:
				return "a_f_m_waptownfolk_01"_J;
			case PlayerModel::GUNSLINGER:
				return "a_m_m_valcriminals_01"_J;
			case PlayerModel::COWBOY:
				return "a_m_m_valtownfolk_02"_J;
			case PlayerModel::SHERIFF:
				return "cs_valsheriff"_J;
			case PlayerModel::OUTLAW:
				return "a_m_m_valcriminals_01"_J;
			case PlayerModel::GENTLEMAN:
				return "a_m_m_rhdupperclass_01"_J;
			case PlayerModel::LADY:
				return "a_f_m_rhdupperclass_01"_J;
			case PlayerModel::FARMER:
				return "a_m_m_valfarmer_01"_J;
			default:
				return "player_zero"_J;
			}
		}

		virtual void OnCall() override
		{
			FiberPool::Push([this] {
				auto player = Self::GetPed();
				if (!player.IsValid())
					return;

				auto model = (PlayerModel)_PlayerModelType.GetState();
				Hash modelHash = GetModelHash(model);

				// Request model
				STREAMING::REQUEST_MODEL(modelHash, false);
				while (!STREAMING::HAS_MODEL_LOADED(modelHash))
					ScriptMgr::Yield();

				// Get current position
				auto pos = player.GetPosition();
				auto heading = ENTITY::GET_ENTITY_HEADING(player.GetHandle());

				// Change model
				PLAYER::SET_PLAYER_MODEL(Self::GetPlayer().GetId(), modelHash, false);

				// Wait for model to load
				ScriptMgr::Yield();

				// Restore position
				ENTITY::SET_ENTITY_COORDS(Self::GetPed().GetHandle(), pos.x, pos.y, pos.z, false, false, false, false);
				ENTITY::SET_ENTITY_HEADING(Self::GetPed().GetHandle(), heading);

				Notifications::Show("Model Changed", "You are now someone else!", NotificationType::Success);

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
			});
		}
	};

	static ChangePlayerModel _ChangePlayerModel{"changeplayermodel", "Change Player Model", "Transform into anyone! Perfect for roleplay and screenshots"};
}
