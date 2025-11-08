#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Weather Controller - Full control over RDR2 weather

	enum class WeatherType
	{
		SUNNY,
		CLEAR,
		CLOUDS,
		OVERCAST,
		RAIN,
		DRIZZLE,
		THUNDER,
		HURRICANE,
		SNOW,
		BLIZZARD,
		SNOWLIGHT,
		FOG,
		HIGHPRESSURE,
		SANDSTORM
	};

	static const std::vector<std::pair<int, const char*>> g_WeatherTypes = {
		{(int)WeatherType::SUNNY, "Sunny"},
		{(int)WeatherType::CLEAR, "Clear Sky"},
		{(int)WeatherType::CLOUDS, "Clouds"},
		{(int)WeatherType::OVERCAST, "Overcast"},
		{(int)WeatherType::RAIN, "Rain"},
		{(int)WeatherType::DRIZZLE, "Drizzle"},
		{(int)WeatherType::THUNDER, "Thunderstorm"},
		{(int)WeatherType::HURRICANE, "Hurricane"},
		{(int)WeatherType::SNOW, "Snow"},
		{(int)WeatherType::BLIZZARD, "Blizzard"},
		{(int)WeatherType::SNOWLIGHT, "Light Snow"},
		{(int)WeatherType::FOG, "Fog"},
		{(int)WeatherType::HIGHPRESSURE, "High Pressure"},
		{(int)WeatherType::SANDSTORM, "Sandstorm"}
	};

	static ListCommand _WeatherType{"weathertype", "Weather Type", "Choose weather", g_WeatherTypes, 0};
	static BoolCommand _FreezeWeather{"freezeweather", "Freeze Weather", "Prevent weather from changing"};
	static BoolCommand _InstantWeather{"instantweather", "Instant Change", "Change weather instantly"};

	const char* GetWeatherName(WeatherType type)
	{
		switch (type)
		{
		case WeatherType::SUNNY:
			return "sunny";
		case WeatherType::CLEAR:
			return "highnoon";
		case WeatherType::CLOUDS:
			return "clouds";
		case WeatherType::OVERCAST:
			return "overcast";
		case WeatherType::RAIN:
			return "rain";
		case WeatherType::DRIZZLE:
			return "drizzle";
		case WeatherType::THUNDER:
			return "thunder";
		case WeatherType::HURRICANE:
			return "hurricane";
		case WeatherType::SNOW:
			return "snow";
		case WeatherType::BLIZZARD:
			return "blizzard";
		case WeatherType::SNOWLIGHT:
			return "snowlight";
		case WeatherType::FOG:
			return "fog";
		case WeatherType::HIGHPRESSURE:
			return "highpressure";
		case WeatherType::SANDSTORM:
			return "sandstorm";
		default:
			return "sunny";
		}
	}

	class ApplyWeather : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			WeatherType type = (WeatherType)_WeatherType.GetState();
			const char* weatherName = GetWeatherName(type);

			if (_InstantWeather.GetState())
			{
				MISC::SET_WEATHER_TYPE(Joaat(weatherName), true, true, true, 0.0f, false);
			}
			else
			{
				MISC::SET_WEATHER_TYPE(Joaat(weatherName), true, true, false, 60.0f, false);
			}

			Notifications::Show("Weather Controller", std::format("Changed weather to {}!", g_WeatherTypes[(int)type].second), NotificationType::Success);
		}
	};

	static ApplyWeather _ApplyWeather{"applyweather", "Apply Weather", "Change to selected weather"};

	class WeatherFreezer : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (_FreezeWeather.GetState())
			{
				WeatherType type = (WeatherType)_WeatherType.GetState();
				const char* weatherName = GetWeatherName(type);
				MISC::SET_WEATHER_TYPE(Joaat(weatherName), true, true, true, 0.0f, false);
			}
		}
	};

	static WeatherFreezer _WeatherFreezer{"weatherfreezer", "Weather Freezer", ""};

	class ClearWeather : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			MISC::CLEAR_OVERRIDE_WEATHER();
			MISC::CLEAR_WEATHER_TYPE_PERSIST();
			Notifications::Show("Weather Controller", "Cleared weather override!", NotificationType::Info);
		}
	};

	static ClearWeather _ClearWeather{"clearweather", "Clear Override", "Reset weather to normal"};
}
