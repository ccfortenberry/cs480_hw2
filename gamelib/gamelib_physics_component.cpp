#include "pch.h"
#include <gamelib_base.hpp>
#include <gamelib_locator.hpp>
#include <gamelib_physics_component.hpp>
#include <limits>

namespace GameLib {
	inline bool AABBcollidesAABB(glm::vec3 amin, glm::vec3 amax, glm::vec3 bmin, glm::vec3 bmax) {
		bool overlapX = (amin.x <= bmax.x && amax.x >= bmin.x);
		bool overlapY = (amin.y <= bmax.y && amax.y >= bmin.y);
		bool overlapZ = (amin.z <= bmax.z && amax.z >= bmin.z);
		return overlapX && overlapY && overlapZ;
	}

	inline bool BroadPhaseAABB(glm::vec3 ap, glm::vec3 as, glm::vec3 av, glm::vec3 bp, glm::vec3 bs) {
		glm::vec3 amin;
		amin.x = av.x > 0 ? ap.x : ap.x + av.x;
		amin.y = av.y > 0 ? ap.y : ap.y + av.y;
		amin.z = av.z > 0 ? ap.z : ap.z + av.z;
		glm::vec3 asize;
		asize.x = av.x > 0 ? as.x + av.x : as.x - av.x;
		asize.y = av.y > 0 ? as.y + av.y : as.y - av.y;
		asize.z = av.z > 0 ? as.z + av.z : as.z - av.z;
		glm::vec amax = amin + asize;
		return AABBcollidesAABB(amin, amax, bp, bp + bs);
	}

	bool BroadPhaseAABB(Actor& a, Actor& b) {
		if (BroadPhaseAABB(a.lastPosition, a.size, a.velocity, b.position, b.size))
			return true;
		return false;
	}

	float SweptAABB(Actor& a, Actor& b, glm::vec3& normal) {
		glm::vec3 inverseEnter;
		glm::vec3 inverseLeave;
		glm::vec3 ap1 = a.position;
		glm::vec3 ap2 = a.position + a.size;
		glm::vec3 bp1 = b.position;
		glm::vec3 bp2 = b.position + b.size;

		if (a.velocity.x > 0.0f) {
			inverseEnter.x = bp1.x - ap2.x;
			inverseLeave.x = bp2.x - ap1.x;
		} else {
			inverseEnter.x = bp2.x - ap1.x;
			inverseLeave.x = bp1.x - ap2.x;
		}

		if (a.velocity.y > 0.0f) {
			inverseEnter.y = bp1.y - ap2.y;
			inverseLeave.y = bp2.y - ap1.y;
		} else {
			inverseEnter.y = bp2.y - ap1.y;
			inverseLeave.y = bp1.y - ap2.y;
		}

		glm::vec3 enter;
		glm::vec3 leave;
		constexpr float inf = std::numeric_limits<float>::infinity();

		if (a.velocity.x == 0.0f) {
			enter.x = -inf;
			leave.x = inf;
		} else {
			enter.x = inverseEnter.x / a.velocity.x;
			leave.x = inverseLeave.x / a.velocity.x;
		}

		if (a.velocity.y == 0.0f) {
			enter.y = -inf;
			leave.y = inf;
		} else {
			enter.y = inverseEnter.y / a.velocity.y;
			leave.y = inverseLeave.y / a.velocity.y;
		}

		float enterTime = std::max(enter.x, enter.y);
		float leaveTime = std::min(leave.x, leave.y);
		normal = { 0.0f, 0.0f, 0.0f };
		if (enterTime > leaveTime)
			return 1.0f;
		if (enter.x < 0.0f && enter.y < 0.0f)
			return 1.0f;
		if (enter.x < 0.0f) {
			if (ap2.x < bp1.x || ap1.x > bp2.x)
				return 1.0f;
		}
		if (enter.y < 0.0f) {
			if (ap2.y < bp1.y || ap1.y > bp2.y)
				return 1.0f;
		}
		// if (enter.x > 1.0f || enter.y > 1.0f) {
		//	if (enter.x > enter.y) {
		//		HFLOGDEBUG("missing a condition?");
		//	}
		//	return 1.0f;
		//}

		if (enter.x > enter.y) {
			if (inverseEnter.x < 0.0f) {
				normal = { 1.0f, 0.0f, 0.0f };
			} else {
				normal = { -1.0f, 0.0f, 0.0f };
			}
		} else {
			if (inverseEnter.y < 0.0f) {
				normal = { 0.0f, 1.0f, 0.0f };
			} else {
				normal = { 0.0f, -1.0f, 0.0f };
			}
		}

		return enterTime;
	}


	void SimplePhysicsComponent::beginPlay(Actor& a) {
		auto box2d = Locator::getBox2D();
		if (box2d && a.box2dId < 0) {
			a.box2dId = box2d->initBody(
				a.box2dType,
				a.center2d(),
				a.sizeHalf2d(),
				a.physicsInfo.density,
				a.physicsInfo.friction);
		}
	}


	void SimplePhysicsComponent::preupdate(Actor& a) {
		auto box2d = Locator::getBox2D();
		if (box2d && a.box2dId >= 0) {
			auto body = box2d->getBody(a.box2dId, a.box2dType);
			body->setPosition(a.center2d());
			body->setVelocity(a.velocity2d());
			body->applyImpulse({ a.physicsInfo.a.x, a.physicsInfo.a.y });
		}
	}


	void SimplePhysicsComponent::postupdate(Actor& a) {
		auto box2d = Locator::getBox2D();
		if (box2d && a.box2dId >= 0) {
			auto body = box2d->getBody(a.box2dId, a.box2dType);
			a.setCenter2d(body->position());
			auto velocity = body->velocity();
			if (glm::length(velocity) > 32) {
				velocity = glm::normalize(velocity) * 32.0f;
			}
			a.setVelocity2d(velocity);
		}
	}


	void SimplePhysicsComponent::update(Actor& a, World& w) {
		//a.position += a.dt * a.speed * a.velocity;
		if (a.clipToWorld) {
			a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
			a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
		}
	}

	bool SimplePhysicsComponent::collideWorld(Actor& a, World& w) {
		int aix = (int)a.position.x;
		int aiy = (int)a.position.y;
		bool fracX = a.position.x - aix > 0;
		bool fracY = a.position.y - aiy > 0;
		int anyhit=0;
		if (w.getTile(aix, aiy).solid())
			anyhit|=1;
		if (fracX && w.getTile(aix + 1, aiy).solid())
			anyhit |= 1;
		if (fracY && w.getTile(aix, aiy + 1).solid())
			anyhit |= 1;
		if (fracX && fracY && w.getTile(aix + 1, aiy + 1).solid())
			anyhit |= 1;
		return anyhit;
	}

	bool SimplePhysicsComponent::collideDynamic(Actor& a, Actor& b) { return collides(a, b); }

	bool SimplePhysicsComponent::collideStatic(Actor& a, Actor& b) {
		return BroadPhaseAABB(a, b); // collides(a, b);
	}

	bool SimplePhysicsComponent::collideTrigger(Actor& a, Actor& b) { return collides(a, b); }

	bool GameLib::TraceCurtisDynamicActorComponent::collideDynamic(Actor& a, Actor& b) {
		glm::vec3 amin = a.position;
		glm::vec3 amax = a.position + a.size;
		glm::vec3 bmin = b.position;
		glm::vec3 bmax = b.position + b.size;

		bool overlapX = (amin.x <= bmax.x && amax.x >= bmin.x);
		bool overlapY = (amin.y <= bmax.y && amax.y >= bmin.y);
		bool overlapZ = (amin.z <= bmax.z && amax.z >= bmin.z);
		if (a.getId() != b.getId()) {
			if (overlapX && overlapY && overlapZ) {
				if (abs(amin.x - bmin.x) > abs(amin.y - bmin.y)) {
					if (amin.x >= bmin.x) {
						// moveX
						// a.position.x += 1;
						// right
						a.position.x = clamp<float>(a.position.x, (float)bmax.x, (float)a.position.x + b.position.x);
					} else if (amax.x <= bmax.x) {
						// a.position.x -= 1;
						// left
						a.position.x = clamp<float>(a.position.x, 0, (float)b.position.x - b.size.x);
					}
				} else if (abs(amin.x - bmin.x) < abs(amin.y - bmin.y)) {
					if (amin.y >= bmin.y) {
						a.position.y = clamp<float>(a.position.y, (float)bmax.y, (float)a.position.y + b.position.y);
					} else if (amax.y <= bmax.y) {
						a.position.y = clamp<float>(a.position.y, 0, (float)b.position.y - b.size.y);
					}
				}
			}
		}
		return overlapX && overlapY && overlapZ;
	}

	void GameLib::TraceCurtisDynamicActorComponent::update(Actor& a, World& w) {
		a.position += a.dt * a.speed * a.velocity;
		if (a.clipToWorld) {
			a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
			a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
		}
	}

	bool GameLib::DainNickJosephWorldPhysicsComponent::collideWorld(Actor& a, World& w) {

		float subTileSize = 1.0;
		for (float x = floor(a.position.x); x < ceil(a.position.x + a.size.x); x += subTileSize) {
			if (w.getTilef(x, floor(a.position.y)).solid()) {
				return true;
			}
			if (w.getTilef(x, ceil(a.position.y + a.size.y) - subTileSize).solid()) {
				return true;
			}
		}
		for (float y = floor(a.position.y); y < ceil(a.position.y + a.size.y); y += subTileSize) {
			if (w.getTilef(floor(a.position.x), y).solid()) {
				return true;
			}
			if (w.getTilef(ceil(a.position.x + a.size.x) - subTileSize, y).solid()) {
				return true;
			}
		}
		return false;
	}

	void GameLib::DainNickJosephWorldPhysicsComponent::update(Actor& a, World& w) {
		a.position += a.dt * a.speed * a.velocity;
		if (a.clipToWorld) {
			a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
			a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
		}
	}

	bool GameLib::TailonsDynamicPhysicsComponent::collideDynamic(Actor& a, Actor& b) { return collides(a, b); }

	void GameLib::TailonsDynamicPhysicsComponent::update(Actor& a, World& w) {
		a.position += a.dt * a.speed * a.velocity;
		if (a.clipToWorld) {
			a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
			a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
		}
	}

	bool GameLib::TailonsStaticPhysicsComponent::collideStatic(Actor& a, Actor& b) { return collides(a, b); }

	void GameLib::TailonsStaticPhysicsComponent::update(Actor& a, World& w) {}
} // namespace GameLib
