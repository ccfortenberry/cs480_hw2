#ifndef GAMELIB_COMPONENT_HPP
#define GAMELIB_COMPONENT_HPP

#include <gamelib_graphics.hpp>
#include <gamelib_actor.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
    class GraphicsComponent {
    public:
        virtual ~GraphicsComponent() {}
        virtual void draw(Actor& actor, Graphics& graphics) {}
    };

    class SimpleGraphicsComponent : public GraphicsComponent {
    public:
        virtual ~SimpleGraphicsComponent() {}
        void draw(Actor& actor, Graphics& graphics) override;
    };

    class DebugGraphicsComponent : public GraphicsComponent {
    public:
        void draw(Actor& actor, Graphics& graphics) override;
    };
}

#endif // GAMELIB_COMPONENT_HPP
