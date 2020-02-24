#include "pch.h"
#include <gamelib_actor_component.hpp>

namespace GameLib {
	void RandomActorComponent::update(Actor& actor, World& world) {
		for (auto& a : world.dynamicActors) {
			if (a->getId() == actor.getId())
				continue;
		}
	}

	void DainNickJosephWorldCollidingActorComponent::handleCollisionWorld(Actor& actor, World& world) {
		float subTileSize = 1.0;
		float y = 0;
		int horizontalScore = 0;
		for (float x = floor(actor.position.x); x < ceil(actor.position.x + actor.size.x); x += subTileSize) {
			if (world.getTilef(x, floor(actor.position.y)).solid()) {
				if (world.getTilef(x, floor(actor.position.y) + subTileSize).empty())
					actor.position.y -= actor.dt * actor.speed * actor.velocity.y;
			}
			if (world.getTilef(x, ceil(actor.position.y + actor.size.y) - subTileSize).solid()) {
				if (world.getTilef(x, ceil(actor.position.y + actor.size.y) - 2 * subTileSize).empty())
					actor.position.y -= actor.dt * actor.speed * actor.velocity.y;
			}
		}
		for (float y = floor(actor.position.y); y < ceil(actor.position.y + actor.size.y); y += subTileSize) {
			if (world.getTilef(floor(actor.position.x), y).solid()) {
				if (world.getTilef(floor(actor.position.x) + subTileSize, y).empty())
					actor.position.x -= actor.dt * actor.speed * actor.velocity.x;
			}
			if (world.getTilef(ceil(actor.position.x + actor.size.x) - subTileSize, y).solid()) {
				if (world.getTilef(ceil(actor.position.x + actor.size.x) - 2 * subTileSize, y).empty())
					actor.position.x -= actor.dt * actor.speed * actor.velocity.x;
			}
		}
	}

	void TailonsDynamicCollidingActorComponent::handleCollisionDynamic(Actor& actor, Actor& actor2) {
		actor.velocity.x *= -1;
	}

	void TailonsStaticCollidingActorComponent::handleCollisionStatic(Actor& a, Actor& b) {}
} // namespace GameLib
