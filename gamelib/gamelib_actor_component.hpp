#ifndef GAMELIB_ACTOR_COMPONENT_HPP
#define GAMELIB_ACTOR_COMPONENT_HPP

#include <gamelib_actor.hpp>
#include <gamelib_graphics.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
	class ActorComponent {
	public:
		virtual ~ActorComponent() {}
		// update() is called to update internal game logic
		virtual void update(Actor& actor, World& world) {}
		// beginPlay() is called when actor is added to the game world
		virtual void beginPlay(Actor& actor) {}
		// handleCollisionStatic() is called when an actor interacts with a dynamic actor
		virtual void handleCollisionStatic(Actor& a, Actor& b) {}
		// handleCollisionDynamic() is called when an actor interacts with a dynamic actor
		virtual void handleCollisionDynamic(Actor& a, Actor& b) {}
		// handleCollisionWorld() is called when an actor has a collision in the game world
		virtual void handleCollisionWorld(Actor& actor, World& world) {}
		// beginOverlap() is called when the actor collides with a trigger
		virtual void beginOverlap(Actor& a, Actor& b) {}
		// endOverlap() is called when the actor stops colliding with a trigger
		virtual void endOverlap(Actor& a, Actor& b) {}
		// beginTriggerOverlap() is called when the trigger is overlapped by an actor
		virtual void beginTriggerOverlap(Actor& a, Actor& b) {}
		// endTriggerOverlap() is called when the trigger is not overlapped by an actor
		virtual void endTriggerOverlap(Actor& a, Actor& b) {}
	};

	class RandomActorComponent : public ActorComponent {
	public:
		virtual ~RandomActorComponent() {}
		void update(Actor& actor, World& world) override;
	};

	class DainNickJosephWorldCollidingActorComponent : public ActorComponent {
	public:
		virtual ~DainNickJosephWorldCollidingActorComponent() {}
		void handleCollisionWorld(Actor& actor, World& world) override;
	};

	class TailonsDynamicCollidingActorComponent : public ActorComponent {
	public:
        virtual ~TailonsDynamicCollidingActorComponent() {}
        void handleCollisionDynamic(Actor& actor, Actor& actor2) override;
    };

    class TailonsStaticCollidingActorComponent : public ActorComponent {
	public:
        virtual ~TailonsStaticCollidingActorComponent() {}
        void handleCollisionStatic(Actor& actor, Actor& actor2) override;
    };
} // namespace GameLib

#endif
