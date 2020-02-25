#ifndef DOOR_ACTOR_COMPONENT_HPP_INCLUDED
#define DOOR_ACTOR_COMPONENT_HPP_INCLUDED

#include <gamelib_actor.hpp>
#include <gamelib_actor_component.hpp>
#include <gamelib_random.hpp>

namespace GameLib {
	class DoorActorComponent : public ActorComponent {
	public:
		DoorActorComponent(ActorPtr& a);
		virtual ~DoorActorComponent() {}

		void linkDoor(ActorPtr& a);

		void update(Actor& a, World& w) override;
		void beginPlay(Actor& a) override;
		void handleCollisionStatic(Actor& a, Actor& b) override;
		void handleCollisionDynamic(Actor& a, Actor& b) override;
		void handleCollisionWorld(Actor& actor, World& world) override;
		void beginOverlap(Actor& a, Actor& b) override;
		void endOverlap(Actor& a, Actor& b) override;
		void beginTriggerOverlap(Actor& a, Actor& b) override;
		void endTriggerOverlap(Actor& a, Actor& b) override;

	private:
		struct STATICINFO {
			bool horizontal{ false };
			float movement{ 2.0f };
			glm::vec3 position;
			float t{ 0.0f };
		} staticInfo;

		struct TRIGGERINFO {
			glm::vec3 position;
			float t{ 0.0f };
		} triggerInfo;

		ActorPtr door;
	};
}

#endif // !DOOR_ACTOR_COMPONENT_HPP_INCLUDED
