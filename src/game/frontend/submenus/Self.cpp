#include "Self.hpp"

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/IntCommand.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/Players.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/features/Features.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/rdr/Natives.hpp"
#include "game/rdr/data/Emotes.hpp"
#include "util/Rewards.hpp"

#include <map>


namespace YimMenu::Features
{
	BoolCommand _RecoveryEnabled("recoveryenabled", "Recovery Enabled", "Is the recovery feature enabled");
}

namespace YimMenu::Submenus
{
	void RenderAnimationsCategory()
	{
		static std::string anim, dict;
		InputTextWithHint("Dictionary", "Enter Dictionary Name", &dict).Draw();
		InputTextWithHint("Animation", "Enter Animation Name", &anim).Draw();

		if (ImGui::Button("Play Animation"))
		{
			FiberPool::Push([=] {
				for (int i = 0; i < 250; i++)
				{
					if (dict.empty() || anim.empty())
						break;

					if (STREAMING::HAS_ANIM_DICT_LOADED(dict.c_str()))
						break;

					STREAMING::REQUEST_ANIM_DICT(dict.c_str());
					ScriptMgr::Yield();
				}

				TASK::TASK_PLAY_ANIM(YimMenu::Self::GetPed().GetHandle(), dict.c_str(), anim.c_str(), 8.0f, -8.0f, -1, 0, 0, false, false, false, "", 0);
			});
		}

		ImGui::Separator();

		ImGui::Text("Emote Category");
		if (ImGui::BeginCombo("##Emote Category", Emote::emoteCategories[Emote::selectedEmoteCategoryIndex]))
		{
			for (int i = 0; i < Emote::numCategories; i++)
			{
				bool isSelected = (i == Emote::selectedEmoteCategoryIndex);
				if (ImGui::Selectable(Emote::emoteCategories[i], isSelected))
				{
					Emote::selectedEmoteCategoryIndex = i;
					Emote::selectedEmoteMemberIndex   = 0;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Emote");
		if (ImGui::BeginCombo("##Emote",
		        Emote::emoteCategoryMembers[Emote::selectedEmoteCategoryIndex][Emote::selectedEmoteMemberIndex].name))
		{
			for (int i = 0; i < Emote::maxEmotesPerCategory; i++)
			{
				const auto& emote = Emote::emoteCategoryMembers[Emote::selectedEmoteCategoryIndex][i];
				if (emote.name == nullptr)
					break;
				bool isSelected = (i == Emote::selectedEmoteMemberIndex);
				if (ImGui::Selectable(emote.name, isSelected))
				{
					Emote::selectedEmoteMemberIndex = i;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Play Emote"))
		{
			if (*Pointers.IsSessionStarted)
			{
				FiberPool::Push([=] {
					int selectedCategoryIndex = Emote::selectedEmoteCategoryIndex;
					int selectedEmoteIndex    = Emote::selectedEmoteMemberIndex;
					const Emote::EmoteItemData& selectedEmote = Emote::emoteCategoryMembers[selectedCategoryIndex][selectedEmoteIndex];

					TASK::TASK_PLAY_EMOTE_WITH_HASH(YimMenu::Self::GetPed().GetHandle(),
					    static_cast<int>(selectedEmote.type),
					    EMOTE_PM_FULLBODY,
					    static_cast<Hash>(selectedEmote.hash),
					    false,
					    false,
					    false,
					    false,
					    false);
				});
			}
		}

		if (ImGui::Button("Stop Animation"))
		{
			FiberPool::Push([=] {
				TASK::CLEAR_PED_TASKS(YimMenu::Self::GetPed().GetHandle(), true, false);
			});
		}
	}

	Self::Self() :
	    Submenu::Submenu("Self")
	{
		auto main          = std::make_shared<Category>("Main");
		auto globalsGroup  = std::make_shared<Group>("Globals");
		auto movementGroup = std::make_shared<Group>("Movement");
		auto toolsGroup    = std::make_shared<Group>("Tools");
		auto customGroup   = std::make_shared<Group>("Customization");

		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("godmode"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("neverwanted"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("invis"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("offtheradar"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("noragdoll"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("antiafk"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("keepbarsfilled"_J));
		globalsGroup->AddItem(std::make_shared<ConditionalItem>("keepbarsfilled"_J, std::make_shared<BoolCommandItem>("keepdeadeyefilled"_J)));
		globalsGroup->AddItem(std::make_shared<ConditionalItem>("keepbarsfilled"_J, std::make_shared<BoolCommandItem>("keepstaminafilled"_J)));
		globalsGroup->AddItem(std::make_shared<ConditionalItem>("keepbarsfilled"_J, std::make_shared<BoolCommandItem>("keephealthfilled"_J)));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("keepcoresfilled"_J));
		globalsGroup->AddItem(std::make_shared<ConditionalItem>("keepcoresfilled"_J, std::make_shared<BoolCommandItem>("keepdeadeyecorefilled"_J)));
		globalsGroup->AddItem(std::make_shared<ConditionalItem>("keepcoresfilled"_J, std::make_shared<BoolCommandItem>("keepstaminacorefilled"_J)));
		globalsGroup->AddItem(std::make_shared<ConditionalItem>("keepcoresfilled"_J, std::make_shared<BoolCommandItem>("keephealthcorefilled"_J)));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("keepclean"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("antilasso"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("antihogtie"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("antimelee"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("drunk"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("superpunch"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("quickskin"_J));

		toolsGroup->AddItem(std::make_shared<CommandItem>("suicide"_J));
		toolsGroup->AddItem(std::make_shared<CommandItem>("clearcrimes"_J));

		toolsGroup->AddItem(std::make_shared<BoolCommandItem>("npcignore"_J));
		toolsGroup->AddItem(std::make_shared<BoolCommandItem>("eagleeye"_J));

		toolsGroup->AddItem(std::make_shared<BoolCommandItem>("overridewhistle"_J));
		toolsGroup->AddItem(std::make_shared<ConditionalItem>("overridewhistle"_J, std::make_shared<FloatCommandItem>("whistlepitch"_J, "Pitch")));
		toolsGroup->AddItem(std::make_shared<ConditionalItem>("overridewhistle"_J, std::make_shared<FloatCommandItem>("whistleclarity"_J, "Clarity")));
		toolsGroup->AddItem(std::make_shared<ConditionalItem>("overridewhistle"_J, std::make_shared<FloatCommandItem>("whistleshape"_J, "Shape")));

		static float playerScale = 1;
		toolsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::Text("Player Scale");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::InputFloat(" ", &playerScale))
				FiberPool::Push([] {
					YimMenu::Self::GetPed().SetScale(playerScale);
				});
		}));

		movementGroup->AddItem(std::make_shared<BoolCommandItem>("climbsteepslopes"_J));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("superjump"_J));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("superrun"_J));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("noclip"_J));
		movementGroup->AddItem(std::make_shared<ConditionalItem>("noclip"_J, std::make_shared<FloatCommandItem>("noclipspeed"_J)));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("freecam"_J));
		movementGroup->AddItem(std::make_shared<ConditionalItem>("freecam"_J, std::make_shared<FloatCommandItem>("freecamspeed"_J)));

		// Creative player customization
		customGroup->AddItem(std::make_shared<BoolCommandItem>("glowingplayer"_J));
		customGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Make yourself glow with a warm golden light!");
		}));

		main->AddItem(globalsGroup);
		main->AddItem(toolsGroup);
		main->AddItem(movementGroup);
		main->AddItem(customGroup);
		AddCategory(std::move(main));

		auto weapons             = std::make_shared<Category>("Weapons");
		auto weaponsGlobalsGroup = std::make_shared<Group>("Globals");

		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("infiniteammo"_J));
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("infiniteclip"_J));
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("nospread"_J));
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("autocock"_J));
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("keepgunsclean"_J));

		weapons->AddItem(weaponsGlobalsGroup);
		AddCategory(std::move(weapons));

		auto horse             = std::make_shared<Category>("Horse");
		auto horseGlobalsGroup = std::make_shared<Group>("Globals");
		auto horseCustomGroup = std::make_shared<Group>("Customization");

		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("horsegodmode"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("horsenoragdoll"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("horsesuperrun"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("keephorseclean"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("horseclimbsteepslopes"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("keephorsebarsfilled"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("keephorsecoresfilled"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("keephorseagitationlow"_J));
		horseGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("flaminghooves"_J));
		horseGlobalsGroup->AddItem(std::make_shared<CommandItem>("tpmounttoself"_J));
		static float horseScale = 1;
		horseGlobalsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::Text("Horse Scale");
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::InputFloat(" ", &horseScale))
				FiberPool::Push([] {
					YimMenu::Self::GetMount().SetScale(horseScale);
				});
		}));

		// Creative customization features
		horseCustomGroup->AddItem(std::make_shared<BoolCommandItem>("rainbowhorse"_J));
		horseCustomGroup->AddItem(std::make_shared<IntCommandItem>("customhorsecolor"_J));
		horseCustomGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Color Guide: 0=Red, 6=Orange, 24=Yellow, 66=Green, 73=Cyan, 67=Blue, 49=Purple, 71=Pink");
		}));

		horse->AddItem(horseGlobalsGroup);
		horse->AddItem(horseCustomGroup);
		AddCategory(std::move(horse));

		auto vehicle             = std::make_shared<Category>("Vehicle");
		auto vehicleGlobalsGroup = std::make_shared<Group>("Globals");
		auto vehicleFunGroup     = std::make_shared<Group>("Fun");
		auto vehicleCustomGroup  = std::make_shared<Group>("Customization");

		vehicleGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("vehiclegodmode"_J));
		vehicleGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("vehiclenodetach"_J));
		vehicleGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("flaminghoovesdraft"_J));
		vehicleGlobalsGroup->AddItem(std::make_shared<CommandItem>("repairvehicle"_J));

		vehicleFunGroup->AddItem(std::make_shared<BoolCommandItem>("superdrive"_J));
		vehicleFunGroup->AddItem(std::make_shared<ConditionalItem>("superdrive"_J, std::make_shared<BoolCommandItem>("superdrivedirectional"_J, "Directional")));
		vehicleFunGroup->AddItem(std::make_shared<ConditionalItem>("superdrive"_J, std::make_shared<IntCommandItem>("superdriveforce"_J, "Force")));
		vehicleFunGroup->AddItem(std::make_shared<BoolCommandItem>("superbrake"_J));

		// Creative customization features
		vehicleCustomGroup->AddItem(std::make_shared<BoolCommandItem>("rainbowvehicle"_J));

		vehicle->AddItem(vehicleGlobalsGroup);
		vehicle->AddItem(vehicleFunGroup);
		vehicle->AddItem(vehicleCustomGroup);
		AddCategory(std::move(vehicle));

		auto animations = std::make_shared<Category>("Animations");

		animations->AddItem(std::make_shared<ImGuiItem>([] {
			RenderAnimationsCategory();
		}));

		AddCategory(std::move(animations));

		// CREATIVE MODE - Ultimate peaceful and creative features
		auto creative = std::make_shared<Category>("Creative");
		auto visualsGroup = std::make_shared<Group>("Visual Effects");
		auto particlesGroup = std::make_shared<Group>("Particle Effects");
		auto playerEffectsGroup = std::make_shared<Group>("Player Effects");
		auto worldGroup = std::make_shared<Group>("World Control");
		auto companionsGroup = std::make_shared<Group>("Companions");
		auto vehiclesGroup = std::make_shared<Group>("Flying Machines");
		auto ridablesGroup = std::make_shared<Group>("Ridable Animals");
		auto explorationGroup = std::make_shared<Group>("Exploration");
		auto peacefulGroup = std::make_shared<Group>("Peaceful Features");

		// Visual Effects & Filters
		visualsGroup->AddItem(std::make_shared<BoolCommandItem>("visualfilterapplier"_J));
		visualsGroup->AddItem(std::make_shared<ConditionalItem>("visualfilterapplier"_J, std::make_shared<ListCommandItem>("visualfilter"_J)));
		visualsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Choose from 10 cinematic filters: Sepia Western, Film Noir, Golden Hour, Dreamy, and more!");
		}));
		visualsGroup->AddItem(std::make_shared<BoolCommandItem>("customfov"_J));
		visualsGroup->AddItem(std::make_shared<ConditionalItem>("customfov"_J, std::make_shared<FloatCommandItem>("customfovvalue"_J)));
		visualsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Adjust your camera FOV (30=zoomed, 90=wide, 120=fish eye) for epic screenshots!");
		}));

		// Particle Trails
		particlesGroup->AddItem(std::make_shared<BoolCommandItem>("particletrails"_J));
		particlesGroup->AddItem(std::make_shared<ConditionalItem>("particletrails"_J, std::make_shared<ListCommandItem>("particletrail"_J)));
		particlesGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Leave magical trails: Butterflies, Fireflies, Sparkles, Fire, Snow, Flower Petals & more!");
		}));

		// Player Effects
		playerEffectsGroup->AddItem(std::make_shared<BoolCommandItem>("playerscale"_J));
		playerEffectsGroup->AddItem(std::make_shared<ConditionalItem>("playerscale"_J, std::make_shared<FloatCommandItem>("playerscalevalue"_J)));
		playerEffectsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Resize yourself! 0.2=tiny, 1.0=normal, 5.0=giant. See the world from new perspectives!");
		}));
		playerEffectsGroup->AddItem(std::make_shared<BoolCommandItem>("flaminglasso"_J));
		playerEffectsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Your lasso burns with fire! Super cool visual effect!");
		}));

		// World Control
		worldGroup->AddItem(std::make_shared<BoolCommandItem>("slowmotion"_J));
		worldGroup->AddItem(std::make_shared<ConditionalItem>("slowmotion"_J, std::make_shared<FloatCommandItem>("slowmotionspeed"_J)));
		worldGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Cinematic slow motion! 0.1=super slow, 1.0=normal. Perfect for epic moments!");
		}));
		worldGroup->AddItem(std::make_shared<BoolCommandItem>("freezetime"_J));
		worldGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Freeze time to hold perfect moments and explore at your own pace!");
		}));
		worldGroup->AddItem(std::make_shared<BoolCommandItem>("gravitymodifier"_J));
		worldGroup->AddItem(std::make_shared<ConditionalItem>("gravitymodifier"_J, std::make_shared<ListCommandItem>("gravitymode"_J)));
		worldGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Experience different gravity: Moon jumps, Zero-G, Reverse (fall up!), or Heavy!");
		}));

		// Companion Animals
		companionsGroup->AddItem(std::make_shared<ListCommandItem>("companiontype"_J));
		companionsGroup->AddItem(std::make_shared<CommandItem>("spawncompanion"_J));
		companionsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Spawn friendly animal companions: Dog, Wolf, Bear, Eagle, Deer & more! They'll follow you everywhere!");
		}));

		// Flying Machines
		vehiclesGroup->AddItem(std::make_shared<ListCommandItem>("flyingmachinetype"_J));
		vehiclesGroup->AddItem(std::make_shared<CommandItem>("spawnflyingmachine"_J));
		vehiclesGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Fly airships, planes, UFOs, hot air balloons & more! Explore the skies!");
		}));

		// Ridable Animals
		ridablesGroup->AddItem(std::make_shared<ListCommandItem>("ridableanimaltype"_J));
		ridablesGroup->AddItem(std::make_shared<CommandItem>("spawnridableanimal"_J));
		ridablesGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Ride eagles through the sky, lions across the plains, bears through forests!");
		}));

		// Exploration
		explorationGroup->AddItem(std::make_shared<ListCommandItem>("playermodeltype"_J));
		explorationGroup->AddItem(std::make_shared<CommandItem>("changeplayermodel"_J));
		explorationGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Transform into anyone! Roleplay as Arthur, Sadie, Dutch, or any NPC!");
		}));
		explorationGroup->AddItem(std::make_shared<BoolCommandItem>("nightvision"_J));
		explorationGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("See clearly in the dark with enhanced vision!");
		}));
		explorationGroup->AddItem(std::make_shared<BoolCommandItem>("walkonwater"_J));
		explorationGroup->AddItem(std::make_shared<BoolCommandItem>("walkonair"_J));
		explorationGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Walk across water or create invisible platforms in the sky!");
		}));
		explorationGroup->AddItem(std::make_shared<CommandItem>("teleporttoguarma"_J));
		explorationGroup->AddItem(std::make_shared<CommandItem>("teleporttosisika"_J));
		explorationGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Visit restricted areas: Guarma island, Sisika prison, & more!");
		}));

		// Peaceful Features
		peacefulGroup->AddItem(std::make_shared<BoolCommandItem>("antibounty"_J));
		peacefulGroup->AddItem(std::make_shared<BoolCommandItem>("autoremovebounty"_J));
		peacefulGroup->AddItem(std::make_shared<BoolCommandItem>("autopaybounty"_J));
		peacefulGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Automatically remove or pay bounties - play in peace!");
		}));
		peacefulGroup->AddItem(std::make_shared<CommandItem>("opennearbydo ors"_J));
		peacefulGroup->AddItem(std::make_shared<CommandItem>("unlockall doors"_J));
		peacefulGroup->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextWrapped("Open locked doors: banks, shops, stables - explore everywhere!");
		}));

		creative->AddItem(visualsGroup);
		creative->AddItem(particlesGroup);
		creative->AddItem(playerEffectsGroup);
		creative->AddItem(worldGroup);
		creative->AddItem(companionsGroup);
		creative->AddItem(vehiclesGroup);
		creative->AddItem(ridablesGroup);
		creative->AddItem(explorationGroup);
		creative->AddItem(peacefulGroup);
		AddCategory(std::move(creative));
	}
}
