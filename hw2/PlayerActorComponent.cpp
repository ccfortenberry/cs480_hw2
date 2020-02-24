#include "PlayerActorComponent.hpp"

namespace GameLib {
	extern void debugDraw(Actor& a);
	extern void debugDrawSweptAABB(Actor& a);
	extern float SweptAABB(Actor& a, Actor& b, glm::vec3& normal);
	
	void PlayerActorComponent::update(Actor& a, World& w) {
		if (a.isStatic()) {
			staticInfo.t += a.dt;
			float movement = std::sin(staticInfo.t) * staticInfo.movement;
			a.position = staticInfo.position;
			if (staticInfo.horizontal) {
				a.position.x += movement;
			}
			else {
				a.position.y += movement;
			}
		}

		if (a.isTrigger()) {
			if (triggerInfo.t > 0.0f) {
				a.position.y = triggerInfo.position.y + std::sin(50.0f * triggerInfo.t) * 0.25f;
				triggerInfo.t -= a.dt;
				if (triggerInfo.t < 0.0f) {
					a.position = triggerInfo.position;
					a.anim.baseId = a.spriteId();
				}
			}
		}
	}

	void PlayerActorComponent::beginPlay(Actor& a) {
		if (a.isStatic()) {
			staticInfo.horizontal = (random.rd() & 1) == 1;
			staticInfo.movement = random.positive() * 5.0f + 2.0f;
			staticInfo.position = a.position;
		}
	}

	void PlayerActorComponent::handleCollisionStatic(Actor& a, Actor& b) {

	}

	void PlayerActorComponent::handleCollisionDynamic(Actor& a, Actor& b) {

	}

	void PlayerActorComponent::handleCollisionWorld(Actor& actor, World& world) {

	}

	void PlayerActorComponent::beginOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Actor '%d' is now overlapping trigger actor '%d'", a.getId(), b.getId());
	}

	void PlayerActorComponent::endOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Actor '%d' is not overlapping trigger actor '%d'", a.getId(), b.getId());
	}

	void PlayerActorComponent::beginTriggerOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Trigger actor '%d' is now overlapped by actor '%d'", a.getId(), b.getId());
	}

	void PlayerActorComponent::endTriggerOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Trigger actor '%d' is not overlapped by actor '%d'", a.getId(), b.getId());
	}
}
