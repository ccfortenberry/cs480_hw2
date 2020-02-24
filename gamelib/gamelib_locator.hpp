#ifndef GAMELIB_LOCATOR_HPP
#define GAMELIB_LOCATOR_HPP

#include <gamelib_base.hpp>
#include <gamelib_context.hpp>
#include <gamelib_audio.hpp>
#include <gamelib_graphics.hpp>
#include <gamelib_input_handler.hpp>
#include <gamelib_box2d.hpp>

namespace GameLib {
    class World;

    class Locator {
    public:
        static Context* getContext() {
            if (!context_)
                throw std::runtime_error("Context not provided");
            return context_;
        }
        static void provide(Context* context) { context_ = context; }

        static IAudio* getAudio() {
            if (audioService_)
                return audioService_;
            return &nullAudioService_;
        }
        static void provide(IAudio* audio) { audioService_ = audio; }

        static void provide(InputHandler* input) { inputHandler_ = input; }
        static InputHandler* getInput() { return inputHandler_; }

		static IGraphics* getGraphics() {
            if (graphicsService_)
                return graphicsService_;
            return &nullGraphicsService_;
        }
		static void provide(IGraphics* graphics) { graphicsService_ = graphics; }

		static void provide(World* world) { world_ = world; }
        static World* getWorld() { return world_; }

		static void provide(Box2D* box2d) { box2d_ = box2d;}
		static Box2D* getBox2D() {return box2d_;}
    private:
        static Context* context_;

        static IAudio* audioService_;
        static IAudio nullAudioService_;

		static IGraphics* graphicsService_;
        static IGraphics nullGraphicsService_;

        static InputHandler* inputHandler_;

		static World* world_;

		static Box2D* box2d_;
    };
}

#endif
