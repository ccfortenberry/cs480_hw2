#include "pch.h"
#include <gamelib_locator.hpp>

namespace GameLib {
    Context* Locator::context_{ nullptr };
    IAudio* Locator::audioService_{ nullptr };
    IAudio Locator::nullAudioService_;
    InputHandler* Locator::inputHandler_{ nullptr };
    IGraphics* Locator::graphicsService_{ nullptr };
    IGraphics Locator::nullGraphicsService_;
    World* Locator::world_{ nullptr };
	Box2D* Locator::box2d_ { nullptr };
}
