#ifndef GAMELIB_INPUT_COMPONENT_HPP
#define GAMELIB_INPUT_COMPONENT_HPP

#include <gamelib_actor.hpp>

namespace GameLib {
    class InputComponent {
    public:
        virtual ~InputComponent() {}
        virtual void update(Actor& actor) {}
    };

    class SimpleInputComponent : public InputComponent {
    public:
        virtual ~SimpleInputComponent() {}
        void update(Actor& actor) override;
    };

    class RandomInputComponent : public InputComponent {
    public:
        virtual ~RandomInputComponent() {}
        void update(Actor& actor) override;

    private:
        int count{ 0 };
    };

	 class InputComponentForDynamic : public InputComponent {
    public:
        virtual ~InputComponentForDynamic() {}

        void update(Actor& actor) override;
    };

    class InputComponentForStatic : public InputComponent {
    public:
        virtual ~InputComponentForStatic() {}

        void update(Actor& actor) override;
    };
}

#endif
