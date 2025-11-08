#include "core/commands/BoolCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Self.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"
#include <vector>

namespace YimMenu::Features
{
	// Build Mode Object Categories
	enum class ObjectCategory
	{
		TENTS_SHELTERS,
		FURNITURE,
		STORAGE,
		LIGHTING,
		DECORATION,
		TOOLS_EQUIPMENT,
		COOKING_SUPPLIES,
		BOOKS_MUSIC,
		STRUCTURES
	};

	static const std::pair<int, const char*> g_ObjectCategories[] = {
		{(int)ObjectCategory::TENTS_SHELTERS, "Tents & Shelters (10 items)"},
		{(int)ObjectCategory::FURNITURE, "Furniture (23 items)"},
		{(int)ObjectCategory::STORAGE, "Storage (19 items)"},
		{(int)ObjectCategory::LIGHTING, "Lighting (15 items)"},
		{(int)ObjectCategory::DECORATION, "Decoration (27 items)"},
		{(int)ObjectCategory::TOOLS_EQUIPMENT, "Tools & Equipment (20 items)"},
		{(int)ObjectCategory::COOKING_SUPPLIES, "Cooking & Supplies (21 items)"},
		{(int)ObjectCategory::BOOKS_MUSIC, "Books & Music (14 items)"},
		{(int)ObjectCategory::STRUCTURES, "Structures (17 items)"}
	};

	static ListCommand _BuildCategory{"buildcategory", "Object Category", "Choose category to spawn", g_ObjectCategories, 0};
	static BoolCommand _BuildModeActive{"buildmodeactive", "Build Mode", "Toggle Sims-style build camera"};

	struct PlacedObject
	{
		Object handle;
		Hash model;
		rage::fvector3 position;
		float heading;
		float scale;
	};

	static std::vector<PlacedObject> g_PlacedObjects;
	static Object g_CurrentObject = 0;
	static float g_PlacementDistance = 5.0f;
	static float g_ObjectRotation = 0.0f;
	static float g_ObjectScale = 1.0f;

	// Object pools by category (166+ total objects)
	const char* GetObjectModel(ObjectCategory category, int index)
	{
		// Tents & Shelters (10)
		static const char* tents[] = {
			"p_tent02x", "p_tent03x", "p_tent04x", "p_tent05bx", "p_tent07x",
			"p_tent09x", "p_tent12x", "p_canopy02x", "p_leantos01x", "p_leantos02x"
		};

		// Furniture (23)
		static const char* furniture[] = {
			"p_chair02x", "p_chair04x", "p_chair05x", "p_chair12x", "p_chair13x",
			"p_chaircomfy01x", "p_chaircomfy02x", "p_chaircomfy03x", "p_chaircomfy05x",
			"p_table01x", "p_table02x", "p_table03x", "p_table04x", "p_table05x",
			"p_bed01x", "p_bed02x", "p_bed03x", "p_dresser01x", "p_dresser02x",
			"p_dressingtable01x", "p_desk01x", "p_desk02x", "p_bench01x"
		};

		// Storage (19)
		static const char* storage[] = {
			"p_chest01x", "p_chest02x", "p_chest03x", "p_chest04x", "p_chest05x",
			"p_crate01x", "p_crate02x", "p_crate03x", "p_crate04x", "p_crate05x",
			"p_barrel01x", "p_barrel02x", "p_barrel03x", "p_barrel04x",
			"p_weaponrack01x", "p_trunk01x", "p_trunk02x", "p_sack01x", "p_sack02x"
		};

		// Lighting (15)
		static const char* lighting[] = {
			"p_lantern01x", "p_lantern02x", "p_lantern03x", "p_lantern04x", "p_lantern05x",
			"p_torch01x", "p_torch02x", "p_campfire01x", "p_campfire02x",
			"p_chandelier01x", "p_chandelier02x", "p_candle01x", "p_candle02x",
			"p_oillamp01x", "p_gaslamp01x"
		};

		// Decoration (27)
		static const char* decoration[] = {
			"p_rug01x", "p_rug02x", "p_rug03x", "p_rug04x", "p_rug05x",
			"p_painting01x", "p_painting02x", "p_painting03x", "p_mirror01x", "p_mirror02x",
			"p_plant01x", "p_plant02x", "p_plant03x", "p_plant04x", "p_plant05x",
			"p_statue01x", "p_statue02x", "p_vase01x", "p_vase02x", "p_vase03x",
			"p_curtain01x", "p_picture01x", "p_picture02x", "p_clock01x",
			"p_taxidermy01x", "p_taxidermy02x", "p_trophy01x"
		};

		// Tools & Equipment (20)
		static const char* tools[] = {
			"p_anvil01x", "p_grindstone01x", "p_bellows01x", "p_workbench01x", "p_workbench02x",
			"p_hitchingpost01x", "p_hitchingpost02x", "p_watertrough01x", "p_watertrough02x",
			"p_ladder01x", "p_ladder02x", "p_wheelbarrow01x", "p_bucket01x", "p_bucket02x",
			"p_shovel01x", "p_axe01x", "p_hammer01x", "p_saw01x", "p_toolbox01x", "p_rope01x"
		};

		// Cooking & Supplies (21)
		static const char* cooking[] = {
			"p_stove01x", "p_stove02x", "p_cookpot01x", "p_cookpot02x", "p_pan01x",
			"p_plate01x", "p_plate02x", "p_bowl01x", "p_bowl02x", "p_cup01x",
			"p_bottle01x", "p_bottle02x", "p_jug01x", "p_foodsack01x", "p_meatside01x",
			"p_breadloaf01x", "p_vegetable01x", "p_fruit01x", "p_fishbox01x",
			"p_waterbarrel01x", "p_supplycrate01x"
		};

		// Books & Music (14)
		static const char* books[] = {
			"p_bookshelf01x", "p_bookshelf02x", "p_book01x", "p_book02x", "p_book03x",
			"p_guitar01x", "p_guitar02x", "p_banjo01x", "p_violin01x",
			"p_piano01x", "p_gramophone01x", "p_newspaper01x", "p_map01x", "p_writing01x"
		};

		// Structures (17)
		static const char* structures[] = {
			"p_fence01x", "p_fence02x", "p_fence03x", "p_gate01x", "p_gate02x",
			"p_wall01x", "p_wall02x", "p_post01x", "p_post02x",
			"p_stairs01x", "p_stairs02x", "p_floor01x", "p_roof01x",
			"p_bridge01x", "p_railing01x", "p_pillar01x", "p_foundation01x"
		};

		switch (category)
		{
		case ObjectCategory::TENTS_SHELTERS:
			return (index < 10) ? tents[index] : tents[0];
		case ObjectCategory::FURNITURE:
			return (index < 23) ? furniture[index] : furniture[0];
		case ObjectCategory::STORAGE:
			return (index < 19) ? storage[index] : storage[0];
		case ObjectCategory::LIGHTING:
			return (index < 15) ? lighting[index] : lighting[0];
		case ObjectCategory::DECORATION:
			return (index < 27) ? decoration[index] : decoration[0];
		case ObjectCategory::TOOLS_EQUIPMENT:
			return (index < 20) ? tools[index] : tools[0];
		case ObjectCategory::COOKING_SUPPLIES:
			return (index < 21) ? cooking[index] : cooking[0];
		case ObjectCategory::BOOKS_MUSIC:
			return (index < 14) ? books[index] : books[0];
		case ObjectCategory::STRUCTURES:
			return (index < 17) ? structures[index] : structures[0];
		default:
			return tents[0];
		}
	}

	class BuildModeCamera : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline Cam buildCam = 0;

		virtual void OnTick() override
		{
			if (!_BuildModeActive.GetState())
				return;

			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return;

			auto pos = ped.GetPosition();

			// Create top-down camera (Sims/Fallout 4 style)
			if (!buildCam || !CAM::DOES_CAM_EXIST(buildCam))
			{
				buildCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", true);
				CAM::SET_CAM_ACTIVE(buildCam, true);
				CAM::RENDER_SCRIPT_CAMS(true, false, 3000, true, false, 0);
			}

			// Position camera above player looking down
			CAM::SET_CAM_COORD(buildCam, pos.x, pos.y, pos.z + 25.0f);
			CAM::POINT_CAM_AT_COORD(buildCam, pos.x, pos.y, pos.z);
			CAM::SET_CAM_FOV(buildCam, 50.0f);
		}

		virtual void OnDisable() override
		{
			if (buildCam && CAM::DOES_CAM_EXIST(buildCam))
			{
				CAM::RENDER_SCRIPT_CAMS(false, false, 3000, true, false, 0);
				CAM::SET_CAM_ACTIVE(buildCam, false);
				CAM::DESTROY_CAM(buildCam, false);
				buildCam = 0;
			}
		}
	};

	static BuildModeCamera _BuildModeCamera{"buildmodecamera", "Build Mode Camera", ""};

	class SpawnBuildObject : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped.IsValid())
					return;

				auto category = (ObjectCategory)_BuildCategory.GetState();
				const char* modelName = GetObjectModel(category, 0);
				Hash model = rage::joaat(modelName);

				STREAMING::REQUEST_MODEL(model, false);
				while (!STREAMING::HAS_MODEL_LOADED(model))
					ScriptMgr::Yield();

				auto pos = ped.GetPosition();
				pos.x += g_PlacementDistance * cos(ENTITY::GET_ENTITY_HEADING(ped.GetHandle()) * (3.14159f / 180.0f));
				pos.y += g_PlacementDistance * sin(ENTITY::GET_ENTITY_HEADING(ped.GetHandle()) * (3.14159f / 180.0f));

				Object obj = OBJECT::CREATE_OBJECT(model, pos.x, pos.y, pos.z, true, true, false, false, false);

				if (obj)
				{
					ENTITY::SET_ENTITY_HEADING(obj, g_ObjectRotation);
					OBJECT::_SET_OBJECT_SCALE(obj, g_ObjectScale);
					ENTITY::FREEZE_ENTITY_POSITION(obj, true);

					PlacedObject placed;
					placed.handle = obj;
					placed.model = model;
					placed.position = pos;
					placed.heading = g_ObjectRotation;
					placed.scale = g_ObjectScale;
					g_PlacedObjects.push_back(placed);

					Notifications::Show("Build Mode", std::format("Placed object! Total: {}", g_PlacedObjects.size()), NotificationType::Success);
				}

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			});
		}
	};

	static SpawnBuildObject _SpawnBuildObject{"spawnbuildobject", "Spawn Object", "Place object from selected category"};

	class ClearAllObjects : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto& obj : g_PlacedObjects)
			{
				if (ENTITY::DOES_ENTITY_EXIST(obj.handle))
				{
					OBJECT::DELETE_OBJECT(&obj.handle);
				}
			}
			g_PlacedObjects.clear();
			Notifications::Show("Build Mode", "Cleared all placed objects!", NotificationType::Info);
		}
	};

	static ClearAllObjects _ClearAllObjects{"clearallobjects", "Clear All Objects", "Remove all placed objects"};
}
