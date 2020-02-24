#include "pch.h"
#include <gamelib_locator.hpp>
#include <gamelib_input_handler.hpp>

#define CHECKPOINTER(button)                                                                                                                                   \
    if (!button)                                                                                                                                               \
        button = &nullCommand;
#define CHECKSCANCODE(x, command)                                                                                                                              \
    if (context->keyboard.scancodes[SDL_SCANCODE_##x]) {                                                                                                       \
        command;                                                                                                                                               \
    }
#define HANDLESCANCODE(x, command, value)                                                                                                                      \
    if (command && context->keyboard.scancodes[SDL_SCANCODE_##x]) {                                                                                            \
        bool result = command->execute(value);                                                                                                                 \
        if (result)                                                                                                                                            \
            context->keyboard.scancodes[SDL_SCANCODE_##x] = 0;                                                                                                 \
    }

namespace GameLib {
    void InputHandler::handle() {
        Context* context = Locator::getContext();
        _checkPointers();
        const float ONE = 1.0f;
        HANDLESCANCODE(W, dpadPosY, ONE);
        HANDLESCANCODE(S, dpadNegY, ONE);
        HANDLESCANCODE(A, dpadNegX, ONE);
        HANDLESCANCODE(D, dpadPosX, ONE);
        HANDLESCANCODE(ESCAPE, back, ONE);
        HANDLESCANCODE(RETURN, start, ONE);
        HANDLESCANCODE(SPACE, buttonA, ONE);
        glm::vec3 axis1{ 0.0f, 0.0f, 0.0f };
        CHECKSCANCODE(LEFT, axis1.x -= 1);
        CHECKSCANCODE(RIGHT, axis1.x += 1);
        CHECKSCANCODE(UP, axis1.y -= 1);
        CHECKSCANCODE(DOWN, axis1.y += 1);

        for (int i = 0; i < context->MaxGameControllers; i++) {
			// use first working controller
            if (context->gameControllers[i].enabled) {
                axis1.x += context->gameControllers[0].axis1.x;
                axis1.y += context->gameControllers[0].axis1.y;
                // axis2.x += context->gameControllers[0].axis2.x;
                // axis2.y += context->gameControllers[0].axis2.y;
                break;
            }
        }
        axis1X->execute(axis1.x);
        axis1Y->execute(axis1.y);

        HANDLESCANCODE(1, key1, ONE);
        HANDLESCANCODE(2, key2, ONE);
        HANDLESCANCODE(3, key3, ONE);
        HANDLESCANCODE(4, key4, ONE);
        HANDLESCANCODE(5, key5, ONE);
        HANDLESCANCODE(6, key6, ONE);
        HANDLESCANCODE(7, key7, ONE);
        HANDLESCANCODE(8, key8, ONE);
        HANDLESCANCODE(9, key9, ONE);
        HANDLESCANCODE(0, key0, ONE);
    }

    void InputHandler::_checkPointers() {
        CHECKPOINTER(axis1X);
        CHECKPOINTER(axis1Y);
        CHECKPOINTER(axis1Z);
        CHECKPOINTER(axis2X);
        CHECKPOINTER(axis2Y);
        CHECKPOINTER(axis2Z);
        CHECKPOINTER(buttonX);
        CHECKPOINTER(buttonY);
        CHECKPOINTER(buttonA);
        CHECKPOINTER(buttonB);
        CHECKPOINTER(dpadNegX);
        CHECKPOINTER(dpadPosX);
        CHECKPOINTER(dpadNegY);
        CHECKPOINTER(dpadPosY);
        CHECKPOINTER(shoulderL1);
        CHECKPOINTER(shoulderL2);
        CHECKPOINTER(shoulderR1);
        CHECKPOINTER(shoulderR2);
        CHECKPOINTER(start);
        CHECKPOINTER(back);
        CHECKPOINTER(home);
        CHECKPOINTER(key1);
        CHECKPOINTER(key2);
        CHECKPOINTER(key3);
        CHECKPOINTER(key4);
        CHECKPOINTER(key1);
        CHECKPOINTER(key6);
        CHECKPOINTER(key7);
        CHECKPOINTER(key8);
        CHECKPOINTER(key9);
        CHECKPOINTER(key0);
    }
}
