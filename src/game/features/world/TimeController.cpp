#include "core/commands/Command.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "game/rdr/Natives.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Time Controller - Control time of day and freeze time

	static BoolCommand _FreezeTime{"freezetime", "Freeze Time", "Stop time from advancing"};
	static IntCommand _SetHour{"sethour", "Hour", "Set hour (0-23)", 12, 0, 23};
	static IntCommand _SetMinute{"setminute", "Minute", "Set minute (0-59)", 0, 0, 59};

	class TimeFreezer : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		static inline int frozenHour = 12;
		static inline int frozenMinute = 0;
		static inline int frozenSecond = 0;

		virtual void OnTick() override
		{
			if (_FreezeTime.GetState())
			{
				// Get current time when first frozen
				static bool firstFreeze = true;
				if (firstFreeze)
				{
					frozenHour = CLOCK::GET_CLOCK_HOURS();
					frozenMinute = CLOCK::GET_CLOCK_MINUTES();
					frozenSecond = CLOCK::GET_CLOCK_SECONDS();
					firstFreeze = false;
				}

				// Keep time frozen
				CLOCK::SET_CLOCK_TIME(frozenHour, frozenMinute, frozenSecond);
				CLOCK::PAUSE_CLOCK(true, 0);
			}
			else
			{
				CLOCK::PAUSE_CLOCK(false, 0);
			}
		}

		virtual void OnDisable() override
		{
			CLOCK::PAUSE_CLOCK(false, 0);
		}
	};

	static TimeFreezer _TimeFreezer{"timefreezer", "Time Freezer", ""};

	class SetTimeOfDay : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			int hour = _SetHour.GetState();
			int minute = _SetMinute.GetState();

			CLOCK::SET_CLOCK_TIME(hour, minute, 0);

			std::string timeStr = std::format("{:02d}:{:02d}", hour, minute);
			Notifications::Show("Time Controller", std::format("Time set to {}", timeStr), NotificationType::Success);
		}
	};

	static SetTimeOfDay _SetTimeOfDay{"settimeofday", "Set Time", "Apply selected time"};

	// Quick time presets
	class SetTimeSunrise : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			CLOCK::SET_CLOCK_TIME(6, 0, 0);
			Notifications::Show("Time Controller", "Time set to Sunrise (06:00)", NotificationType::Success);
		}
	};

	class SetTimeNoon : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			CLOCK::SET_CLOCK_TIME(12, 0, 0);
			Notifications::Show("Time Controller", "Time set to Noon (12:00)", NotificationType::Success);
		}
	};

	class SetTimeSunset : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			CLOCK::SET_CLOCK_TIME(18, 30, 0);
			Notifications::Show("Time Controller", "Time set to Sunset (18:30)", NotificationType::Success);
		}
	};

	class SetTimeMidnight : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			CLOCK::SET_CLOCK_TIME(0, 0, 0);
			Notifications::Show("Time Controller", "Time set to Midnight (00:00)", NotificationType::Success);
		}
	};

	static SetTimeSunrise _SetTimeSunrise{"settimesunrise", "Sunrise (6:00 AM)", "Quick set to sunrise"};
	static SetTimeNoon _SetTimeNoon{"settimenoon", "Noon (12:00 PM)", "Quick set to noon"};
	static SetTimeSunset _SetTimeSunset{"settimesunset", "Sunset (6:30 PM)", "Quick set to sunset"};
	static SetTimeMidnight _SetTimeMidnight{"settimemidnight", "Midnight (12:00 AM)", "Quick set to midnight"};

	// Time scale control
	class SlowMotion : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			MISC::SET_TIME_SCALE(0.3f); // Slow motion
		}

		virtual void OnDisable() override
		{
			MISC::SET_TIME_SCALE(1.0f); // Normal speed
		}
	};

	class FastForward : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			MISC::SET_TIME_SCALE(5.0f); // Fast forward
		}

		virtual void OnDisable() override
		{
			MISC::SET_TIME_SCALE(1.0f); // Normal speed
		}
	};

	static SlowMotion _SlowMotion{"timeslowmotion", "Slow Motion", "Slow down time to 30%"};
	static FastForward _FastForward{"timefastforward", "Fast Forward", "Speed up time 5x"};
}
