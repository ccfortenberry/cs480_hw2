#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <gamelib.hpp>

//////////////////////////////////////////////////////////////////////
// COMMANDS //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class PlaySoundCommand : public GameLib::InputCommand {
public:
	PlaySoundCommand(int audioClipId, bool stopPrevious) : musicClipId_{ audioClipId }, stopPrevious_{ stopPrevious } {}

	const char* type() const override { return "PlaySoundCommand"; }

	bool execute(float amount) override {
		GameLib::Locator::getAudio()->playAudio(musicClipId_, stopPrevious_);
		return true;
	}

private:
	int musicClipId_{ 0 };
	bool stopPrevious_{ false };
};

class PlayMusicCommand : public GameLib::InputCommand {
public:
	PlayMusicCommand(int musicClipId) : musicClipId_{ musicClipId } {}

	const char* type() const override { return "PlayMusicCommand"; }

	bool execute(float amount) override {
		GameLib::Locator::getAudio()->playMusic(musicClipId_, -1, 0);
		return true;
	}

private:
	int musicClipId_{ 0 };
	bool stopPrevious_{ false };
};

class QuitCommand : public GameLib::InputCommand {
public:
	const char* type() const override { return "QuitCommand"; }
	bool execute(float amount) override {
		GameLib::Locator::getContext()->quitRequested = true;
		return true;
	}
};

class MovementCommand : public GameLib::InputCommand {
public:
	const char* type() const override { return "MovementCommand"; }
	bool execute(float amount) override {
		// apply slight curve
		if (amount < 0.1f && amount > -0.1f)
			amount = 0.0f;
		else if (amount > 0.5f)
			amount = 1.0f;
		else if (amount < -0.5f)
			amount = -1.0f;
		else if (amount > 0.0f)
			amount = 0.5f;
		else
			amount = -0.5f;
		return InputCommand::execute(amount);
	}
};

#endif
