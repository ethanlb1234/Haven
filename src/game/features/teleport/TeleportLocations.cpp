#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Teleport Locations - Quick travel to iconic RDR2 locations

	enum class TeleportLocation
	{
		VALENTINE,
		SAINT_DENIS,
		BLACKWATER,
		RHODES,
		STRAWBERRY,
		ANNESBURG,
		ARMADILLO,
		TUMBLEWEED,
		VAN_HORN,
		EMERALD_RANCH,
		GUARMA,
		BRAITHWAITE_MANOR,
		SHADY_BELLE,
		HANGING_DOG_RANCH,
		COLTER,
		WAPITI,
		MANZANITA_POST,
		THIEVES_LANDING,
		MACFARLANES_RANCH,
		SISIKA,
		TALL_TREES,
		GRIZZLIES,
		HEARTLANDS,
		ROANOKE_RIDGE,
		BIG_VALLEY,
		GREAT_PLAINS,
		BAYOU_NWA,
		MOUNT_HAGEN,
		OWANJILA_LAKE,
		FLAT_IRON_LAKE
	};

	static const std::vector<std::pair<int, const char*>> g_TeleportLocations = {
		{(int)TeleportLocation::VALENTINE, "Valentine"},
		{(int)TeleportLocation::SAINT_DENIS, "Saint Denis"},
		{(int)TeleportLocation::BLACKWATER, "Blackwater"},
		{(int)TeleportLocation::RHODES, "Rhodes"},
		{(int)TeleportLocation::STRAWBERRY, "Strawberry"},
		{(int)TeleportLocation::ANNESBURG, "Annesburg"},
		{(int)TeleportLocation::ARMADILLO, "Armadillo"},
		{(int)TeleportLocation::TUMBLEWEED, "Tumbleweed"},
		{(int)TeleportLocation::VAN_HORN, "Van Horn Trading Post"},
		{(int)TeleportLocation::EMERALD_RANCH, "Emerald Ranch"},
		{(int)TeleportLocation::GUARMA, "Guarma (Chapter 5)"},
		{(int)TeleportLocation::BRAITHWAITE_MANOR, "Braithwaite Manor"},
		{(int)TeleportLocation::SHADY_BELLE, "Shady Belle"},
		{(int)TeleportLocation::HANGING_DOG_RANCH, "Hanging Dog Ranch"},
		{(int)TeleportLocation::COLTER, "Colter"},
		{(int)TeleportLocation::WAPITI, "Wapiti Indian Reservation"},
		{(int)TeleportLocation::MANZANITA_POST, "Manzanita Post"},
		{(int)TeleportLocation::THIEVES_LANDING, "Thieves' Landing"},
		{(int)TeleportLocation::MACFARLANES_RANCH, "MacFarlane's Ranch"},
		{(int)TeleportLocation::SISIKA, "Sisika Penitentiary"},
		{(int)TeleportLocation::TALL_TREES, "Tall Trees"},
		{(int)TeleportLocation::GRIZZLIES, "Grizzlies East"},
		{(int)TeleportLocation::HEARTLANDS, "Heartlands"},
		{(int)TeleportLocation::ROANOKE_RIDGE, "Roanoke Ridge"},
		{(int)TeleportLocation::BIG_VALLEY, "Big Valley"},
		{(int)TeleportLocation::GREAT_PLAINS, "Great Plains"},
		{(int)TeleportLocation::BAYOU_NWA, "Bayou Nwa"},
		{(int)TeleportLocation::MOUNT_HAGEN, "Mount Hagen (Peak)"},
		{(int)TeleportLocation::OWANJILA_LAKE, "Lake Owanjila"},
		{(int)TeleportLocation::FLAT_IRON_LAKE, "Flat Iron Lake"}
	};

	static ListCommand _TeleportLocation{"teleportlocation", "Location", "Choose teleport destination", g_TeleportLocations, 0};

	struct LocationCoords
	{
		float x, y, z;
	};

	LocationCoords GetLocationCoords(TeleportLocation loc)
	{
		switch (loc)
		{
		case TeleportLocation::VALENTINE:
			return {-170.0f, 627.0f, 114.0f};
		case TeleportLocation::SAINT_DENIS:
			return {2635.0f, -1226.0f, 53.0f};
		case TeleportLocation::BLACKWATER:
			return {-813.0f, -1324.0f, 43.0f};
		case TeleportLocation::RHODES:
			return {1225.0f, -1293.0f, 76.0f};
		case TeleportLocation::STRAWBERRY:
			return {-1791.0f, -386.0f, 160.0f};
		case TeleportLocation::ANNESBURG:
			return {2932.0f, 1290.0f, 44.0f};
		case TeleportLocation::ARMADILLO:
			return {-3685.0f, -2623.0f, -13.0f};
		case TeleportLocation::TUMBLEWEED:
			return {-5512.0f, -2952.0f, -2.0f};
		case TeleportLocation::VAN_HORN:
			return {2969.0f, 520.0f, 44.0f};
		case TeleportLocation::EMERALD_RANCH:
			return {1400.0f, 347.0f, 88.0f};
		case TeleportLocation::GUARMA:
			return {1261.0f, -6882.0f, 43.0f};
		case TeleportLocation::BRAITHWAITE_MANOR:
			return {1007.0f, -1740.0f, 48.0f};
		case TeleportLocation::SHADY_BELLE:
			return {1800.0f, -1706.0f, 42.0f};
		case TeleportLocation::HANGING_DOG_RANCH:
			return {-1821.0f, -343.0f, 164.0f};
		case TeleportLocation::COLTER:
			return {-1361.0f, 2404.0f, 307.0f};
		case TeleportLocation::WAPITI:
			return {420.0f, 2270.0f, 248.0f};
		case TeleportLocation::MANZANITA_POST:
			return {-1925.0f, -1229.0f, 139.0f};
		case TeleportLocation::THIEVES_LANDING:
			return {-1035.0f, -1797.0f, 61.0f};
		case TeleportLocation::MACFARLANES_RANCH:
			return {-2393.0f, -2404.0f, 62.0f};
		case TeleportLocation::SISIKA:
			return {3405.0f, -680.0f, 40.0f};
		case TeleportLocation::TALL_TREES:
			return {-2140.0f, -1688.0f, 139.0f};
		case TeleportLocation::GRIZZLIES:
			return {1231.0f, 1677.0f, 185.0f};
		case TeleportLocation::HEARTLANDS:
			return {422.0f, 566.0f, 110.0f};
		case TeleportLocation::ROANOKE_RIDGE:
			return {2511.0f, 796.0f, 89.0f};
		case TeleportLocation::BIG_VALLEY:
			return {-1278.0f, 243.0f, 94.0f};
		case TeleportLocation::GREAT_PLAINS:
			return {-1604.0f, -1630.0f, 82.0f};
		case TeleportLocation::BAYOU_NWA:
			return {2216.0f, -544.0f, 41.0f};
		case TeleportLocation::MOUNT_HAGEN:
			return {-1651.0f, 3141.0f, 495.0f};
		case TeleportLocation::OWANJILA_LAKE:
			return {-1484.0f, -1093.0f, 83.0f};
		case TeleportLocation::FLAT_IRON_LAKE:
			return {-328.0f, -683.0f, 42.0f};
		default:
			return {0.0f, 0.0f, 0.0f};
		}
	}

	class TeleportToLocation : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				TeleportLocation loc = (TeleportLocation)_TeleportLocation.GetState();
				LocationCoords coords = GetLocationCoords(loc);

				// Teleport player
				ENTITY::SET_ENTITY_COORDS(ped.GetHandle(), coords.x, coords.y, coords.z, false, false, false, false);

				// Teleport horse if mounted
				if (PED::IS_PED_ON_MOUNT(ped.GetHandle()))
				{
					Ped mount = PED::GET_MOUNT(ped.GetHandle());
					if (mount.IsValid())
					{
						ENTITY::SET_ENTITY_COORDS(mount.GetHandle(), coords.x, coords.y, coords.z, false, false, false, false);
					}
				}

				Notifications::Show("Teleport", std::format("Teleported to {}!", g_TeleportLocations[(int)loc].second), NotificationType::Success);
			});
		}
	};

	static TeleportToLocation _TeleportToLocation{"teleporttolocation", "Teleport", "Travel to selected location"};

	// Waypoint teleport
	class TeleportToWaypoint : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				if (!MAP::IS_WAYPOINT_ACTIVE())
				{
					Notifications::Show("Teleport", "No waypoint set!", NotificationType::Error);
					return;
				}

				Vector3 waypointPos = MAP::_GET_WAYPOINT_COORDS();

				// Get ground Z
				float groundZ;
				if (MISC::GET_GROUND_Z_FOR_3D_COORD(waypointPos.x, waypointPos.y, waypointPos.z + 1000.0f, &groundZ, false))
					waypointPos.z = groundZ;

				ENTITY::SET_ENTITY_COORDS(ped.GetHandle(), waypointPos.x, waypointPos.y, waypointPos.z, false, false, false, false);

				Notifications::Show("Teleport", "Teleported to waypoint!", NotificationType::Success);
			});
		}
	};

	static TeleportToWaypoint _TeleportToWaypoint{"teleporttowaypoint", "Teleport to Waypoint", "Instantly travel to your map waypoint"};
}
