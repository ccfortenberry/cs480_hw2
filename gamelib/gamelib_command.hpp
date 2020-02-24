#ifndef GAMELIB_COMMAND_HPP
#define GAMELIB_COMMAND_HPP

#include <gamelib_base.hpp>

namespace GameLib {
	class Actor;

	class Command {
	public:
		virtual ~Command() {}

		virtual const char* type() const { return "Command"; }
	};

	class NormalCommand : public Command {
	public:
		virtual ~NormalCommand() {}

		const char* type() const override { return "NormalCommand"; }

		virtual void execute() {}
		virtual void undo() {}
	};

	class InputCommand : public Command {
	public:
		virtual ~InputCommand() {}

		// virtual method to process an input button where amount is in the range -1 to 1
		// for axes and 0 to 1 for buttons. If method returns true, then event is considered
		// one time
		virtual bool execute(float amount) {
			amount_ = amount;
			return true;
		}

		float getAmount() const { return amount_; }

		operator bool() const { return amount_ != 0.0f; }

		bool checkClear() {
			bool notzero = (amount_ != 0.0f);
			amount_ = 0.0f;
			return notzero;
		}

	private:
		float amount_;
	};

	class ActorCommand : public Command {
	public:
		virtual ~ActorCommand() {}

		const char* type() const override { return "ActorCommand"; }

		virtual void execute(Actor& actor) {}
		virtual void undo(Actor& actor) {}
	};

	template <typename CommandBaseType>
	class CommandFlyweight {
	public:
		void addCommand(CommandBaseType* command) {
			if (!command)
				return;
			commands[command->type()] = command;
		}

		CommandBaseType* getCommand(CommandBaseType* command) {
			if (!command)
				return nullptr;
			return isCommand(command->type()) ? commands[command->type()] : nullptr;
		}

		bool isCommand(const std::string& type) const { return commands.count(type) != 0; }

		std::map<std::string, CommandBaseType*> commands;
	};

	using NormalCommandFlyweight = CommandFlyweight<NormalCommand>;
	using ActorCommandFlyweight = CommandFlyweight<ActorCommand>;

	extern NormalCommandFlyweight normalCommands;
	extern ActorCommandFlyweight actorCommands;
} // namespace GameLib

#endif
