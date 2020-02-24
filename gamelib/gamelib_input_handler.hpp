#ifndef GAMELIB_INPUT_HANDLER_HPP
#define GAMELIB_INPUT_HANDLER_HPP

#include <gamelib_command.hpp>

namespace GameLib {
    class InputHandler {
    public:
        void handle();

        InputCommand* axis1X{ nullptr };
        InputCommand* axis1Y{ nullptr };
        InputCommand* axis1Z{ nullptr };
        InputCommand* axis2X{ nullptr };
        InputCommand* axis2Y{ nullptr };
        InputCommand* axis2Z{ nullptr };
        InputCommand* buttonX{ nullptr };
        InputCommand* buttonY{ nullptr };
        InputCommand* buttonA{ nullptr };
        InputCommand* buttonB{ nullptr };
        InputCommand* dpadNegX{ nullptr };
        InputCommand* dpadPosX{ nullptr };
        InputCommand* dpadNegY{ nullptr };
        InputCommand* dpadPosY{ nullptr };
        InputCommand* shoulderL1{ nullptr };
        InputCommand* shoulderL2{ nullptr };
        InputCommand* shoulderR1{ nullptr };
        InputCommand* shoulderR2{ nullptr };
        InputCommand* start{ nullptr };
        InputCommand* back{ nullptr };
        InputCommand* home{ nullptr };

		InputCommand* key1{ nullptr };
        InputCommand* key2{ nullptr };
        InputCommand* key3{ nullptr };
        InputCommand* key4{ nullptr };
        InputCommand* key5{ nullptr };
        InputCommand* key6{ nullptr };
        InputCommand* key7{ nullptr };
        InputCommand* key8{ nullptr };
        InputCommand* key9{ nullptr };
        InputCommand* key0{ nullptr };

        InputCommand nullCommand;

        void _checkPointers();
    };
}

#endif
