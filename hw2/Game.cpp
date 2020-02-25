#include "Game.hpp"

void Game::init() {
	GameLib::Locator::provide(&context);
	if (context.audioInitialized())
		GameLib::Locator::provide(&audio);
	GameLib::Locator::provide(&input);
	GameLib::Locator::provide(&graphics);
	GameLib::Locator::provide(&world);
	GameLib::Locator::provide(&box2d);

	box2d.init();

	audio.setVolume(0.2f);

	input.back = &quitCommand;
	input.key1 = &play0;
	input.key2 = &play1;
	input.key3 = &play2;
	input.key4 = &play3;
	input.key5 = &playMusic1;
	input.key6 = &playMusic2;
	input.key7 = &playMusic3;
	input.start = nullptr;
	input.axis1X = &xaxisCommand;
	input.axis1Y = &yaxisCommand;
}

void Game::load() {
	for (auto sp : searchPaths) {
		context.addSearchPath(sp);
	}
	SDL_Texture* testPNG = context.loadImage("godzilla.png");
	SDL_Texture* testJPG = context.loadImage("parrot.jpg");
	graphics.setTileSize({ 32, 32 });
	int spriteCount = context.loadTileset(0, 32, 32, "Tiles32x32.png");
	if (!spriteCount) {
		HFLOGWARN("Tileset not found");
	}
	context.loadTileset(GameLib::LIBXOR_TILESET32, 32, 32, "LibXORColors32x32.png");

	context.loadAudioClip(0, "starbattle-bad.wav");
	context.loadAudioClip(1, "starbattle-dead.wav");
	context.loadAudioClip(2, "starbattle-endo.wav");
	context.loadAudioClip(3, "starbattle-exo.wav");
	context.loadAudioClip(4, "starbattle-ok.wav");
	context.loadAudioClip(5, "starbattle-pdead.wav");
	//context.loadAudioClip(SOUND_BLIP, "blip.wav");
	context.loadMusicClip(0, "starbattlemusic1.mp3");
	context.loadMusicClip(1, "starbattlemusic2.mp3");
	context.loadMusicClip(2, "distoro2.mid");

	gothicfont.load("fonts-japanese-gothic.ttf", 36);
	minchofont.load("fonts-japanese-mincho.ttf", 36);

	worldPath = context.findSearchPath(worldPath);
	if (!world.load(worldPath)) {
		HFLOGWARN("world.txt not found");
	}
}

unsigned int Game::getGameState() {
	stopwatch.start();
	startTiming();

	world.start(t0);

	graphics.setCenter(graphics.origin());

	unsigned int gameState = 0;

	while (!context.quitRequested && gameState==0) {
		updateTiming();

		context.getEvents();
		input.handle();
		_debugKeys();

		context.clearScreen(backColor);
		world.drawTiles(graphics);
		while (lag >= Game::MS_PER_UPDATE) {
			updateWorld();
			lag -= Game::MS_PER_UPDATE;
		}

		//shake();
		updateCamera();
		drawWorld();
		drawHUD();

		context.swapBuffers();
		frames++;
		std::this_thread::yield();
	}

	return gameState;
}

void Game::kill() {
	double totalTime = stopwatch.stop_s();
	HFLOGDEBUG("Sprites/sec = %5.1f", spritesDrawn / totalTime);
	HFLOGDEBUG("Frames/sec = %5.1f", frames / totalTime);

	actorPool.clear();
}

void Game::showIntro() {

}

void Game::initLevel(const int& levelID) {
	// Input Components
	auto NewSimpleInput = []() { return std::make_shared<GameLib::SimpleInputComponent>(); };
	auto NewRandomInput = []() { return std::make_shared<GameLib::RandomInputComponent>(); };
	auto NewInputForDynamic = []() { return std::make_shared<GameLib::InputComponentForDynamic>(); };
	auto NewInputForStatic = []() { return std::make_shared<GameLib::InputComponentForStatic>(); };
	// TODO: Add input for AI/Chasing

	// Actor Components
	auto NewActor = []() { return std::make_shared<GameLib::ActorComponent>(); };
	auto NewPlayerActor = []() { return std::make_shared<GameLib::PlayerActorComponent>(); };
	auto NewDoorActor = [](GameLib::ActorPtr& a) { return std::make_shared<GameLib::DoorActorComponent>(a); };
	// Other actor components?

	// Physics Components
	auto NewPhysics = []() { return std::make_shared<GameLib::SimplePhysicsComponent>(); };
	// Add Newtonian physics?

	// Graphics Component
	auto NewGraphics = []() { return std::make_shared<GameLib::SimpleGraphicsComponent>(); };

	// World parameters
	float cx = world.worldSizeX * 0.5f;
	float cy = world.worldSizeY * 0.5f;
	float speed = (float)graphics.getTileSizeX();

	// Init actors
	auto ply = _makeActor(cx, cy, speed, 1, NewSimpleInput(), NewPlayerActor(), NewPhysics(), NewGraphics());
	world.addDynamicActor(ply);
	ply->rename("Player");

	auto door = _makeActor(27, 18, 0, 853, NewInputForStatic(), NewPlayerActor(), NewPhysics(), NewGraphics());
	world.addStaticActor(door);
	door->rename("Test_Door");
	
	auto trigger = _makeActor(19, 9, 0, 40 * 15 - 8, NewInputForStatic(), NewDoorActor(door), NewPhysics(), NewGraphics());
	world.addTriggerActor(trigger);
	trigger->rename("Test_Trigger");
}

void Game::showGoodEnd() {

}

void Game::showBadEnd() {

}

void Game::updateCamera() {
	glm::ivec2 xy = world.dynamicActors[0]->pixelCenter(graphics);
	glm::ivec2 center = graphics.center();
	center.x = GameLib::clamp(center.x, xy.x - 100, xy.x + 100);
	center.y = GameLib::clamp(center.y, xy.y - 100, xy.y + 100);
	center.y = std::min(graphics.getCenterY(), center.y);
	graphics.setCenter(center);
}

void Game::updateWorld() {
	world.update(Game::MS_PER_UPDATE);
	world.physics(Game::MS_PER_UPDATE);
}

void Game::drawWorld() {
	world.draw(graphics);
}

void Game::drawHUD() {
	char fpsstr[64] = { 0 };
	snprintf(fpsstr, 64, "%3.2f", 1.0f / dt);
	minchofont.draw(
		(int)graphics.getWidth(),
		(int)graphics.getHeight() - 2,
		fpsstr,
		GameLib::Gold,
		GameLib::Font::HALIGN_RIGHT | GameLib::Font::VALIGN_BOTTOM | GameLib::Font::SHADOWED);
}

void Game::startTiming() {
	t0 = stopwatch.stop_sf();
	lag = 0.0f;
}

void Game::updateTiming() {
	t1 = stopwatch.stop_sf();
	dt = t1 - t0;
	t0 = t1;
	GameLib::Context::deltaTime = dt;
	GameLib::Context::currentTime_s = t1;
	GameLib::Context::currentTime_ms = t1 * 1000;
	lag += dt;
}

void Game::main(int argc, char** argv) {
	init();
	load();
	showIntro();
	initLevel(1);
	switch (getGameState())
	{
	case 1:
		showGoodEnd();
		break;
	default:
		showBadEnd();
		break;
	}
	kill();
}

void Game::_debugKeys() {
	if (context.keyboard.checkClear(SDL_SCANCODE_F5)) {
		if (!world.load(worldPath)) {
			HFLOGWARN("world.txt not found");
		}
	}

	/*if (shakeCommand.checkClear()) {
		shake(4, 5, 50 * MS_PER_UPDATE);
	}*/
}
