#ifndef NEWTON_PHYSICS_COMPONENT_HPP
#define NEWTON_PHYSICS_COMPONENT_HPP

#include <gamelib_actor.hpp>

namespace GameLib {
	class NewtonPhysicsComponent: public SimplePhysicsComponent {
	public:
		void update(Actor& a, World& world) override;
		bool collideDynamic(Actor& a, Actor& b) override;
		bool collideStatic(Actor& a, Actor& b) override;
	};
} // namespace GameLib

#endif
