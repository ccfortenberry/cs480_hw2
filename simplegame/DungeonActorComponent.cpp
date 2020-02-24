#include "DungeonActorComponent.hpp"
#include <gamelib_locator.hpp>
#include <gamelib_random.hpp>
#include <limits>

namespace GameLib {
	extern void debugDraw(Actor& a);
	extern void debugDrawSweptAABB(Actor& a);
	extern float SweptAABB(Actor& a, Actor& b, glm::vec3& normal);


	void DungeonActorComponent::update(Actor& a, World& w) {
		// debugDraw(a);
		// debugDrawSweptAABB(a);
		if (a.isStatic()) {
			staticInfo.t += a.dt;
			float movement = std::sin(staticInfo.t) * staticInfo.movement;
			a.position = staticInfo.position;
			if (staticInfo.horizontal) {
				a.position.x += movement;
			} else {
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


	void DungeonActorComponent::beginPlay(Actor& a) {
		if (a.isStatic()) {
			staticInfo.horizontal = (random.rd() & 1) == 1;
			staticInfo.movement = random.positive() * 5.0f + 2.0f;
			staticInfo.position = a.position;
		}
	}


	void DungeonActorComponent::handleCollisionStatic(Actor& a, Actor& b) {
		return;
		// backup a's position
		glm::vec3 curPosition = a.position;
		glm::vec3 curVelocity = a.velocity;
		a.velocity = a.position - a.lastPosition;
		a.position = a.lastPosition;
		// NEW SDF style

		float btop = b.position.y;
		float bbot = b.position.y + b.size.y;

		glm::vec2 P = { a.position.x, a.position.y };
		glm::vec2 Pmax = a.position2d() + a.size2d();
		glm::vec2 asize = a.size2d();
		glm::vec2 v = { a.velocity.x, a.velocity.y };
		float vlen = glm::length(v);
		// avoid NAN by ensuring vector has length
		glm::vec2 vdir = (vlen != 0.0f) ? glm::normalize(v) : glm::vec2{ 0.0f, 0.0f };
		glm::vec2 supportB = b.support(a.center2d());
		glm::vec2 supportA = a.support(b.center2d());
		float t = 0.0f;
		float dt = 0.001f;
		float touching = a.touching(b);
		if (touching < 0) {
			glm::vec2 N = b.normal(supportB);
			glm::vec2 T = b.tangent(supportB);
			float d = glm::length(supportA - supportB);
			vdir = glm::normalize(supportA - supportB);
			glm::vec2 Pnew = P + vdir * touching;
			a.position.x = Pnew.x;
			a.position.y = Pnew.y;

			//// collision, B is on bottom of A
			//if (std::abs(touching) > 0.1f) {
			//	HFLOGWARN("strange %f", d);
			//}
			//a.position = curPosition;
			//a.velocity = curVelocity;
			//if (std::abs(N.x) > std::abs(N.y)) {
			//	a.position.x = Pnew.x;
			//	a.velocity.x = 0.0f;
			//	a.velocity.y = curVelocity.y;
			//}
			//if (std::abs(N.y) > std::abs(N.x)) {
			//	a.velocity.x = curVelocity.x;
			//	// a.velocity.y = 0.0f;
			//	if (N.y > 0) {
			//		float y1 = std::min(bbot, bbot+1);
			//		float y2 = std::max(bbot, bbot+1);
			//		a.position.y = clamp(Pnew.y, y1, y2);
			//		a.velocity.y = -std::abs(v.y);
			//	} else {
			//		float y1 = std::min(a.position.y, btop - a.size.y);
			//		float y2 = std::max(a.position.y, btop - a.size.y);
			//		a.position.y = clamp(Pnew.y, y1, y2);
			//		a.velocity.y = std::abs(v.y);
			//	}
			//}
			//float lt = touching;
			//touching = a.touching(b);
			//if (touching < 0) {
			//	HFLOGWARN("still touching %f from %f", touching, lt);
			//}
			//// a.velocity.x = 0.0f;
			//// a.velocity.y = 0.0f;
			//// if (std::abs(N.x) > 0.0001f)
			////	a.velocity.x = -a.velocity.x;
			//// if (std::abs(N.y) > 0.0001f)
			////	a.velocity.y = -a.velocity.y;
			//// a.velocity.y = 0.0f;
		} else {
			a.velocity = curVelocity;
			a.position = curPosition;
		}
		return;
		// OLD AABB style
		glm::vec3 normal;
		float collisionTime = SweptAABB(a, b, normal);
		if (collisionTime >= 1.0f) {
			a.position = curPosition;
			a.velocity = curVelocity;
			return;
		}
		a.position += a.velocity * collisionTime;
		float timeLeft = 1.0f - collisionTime;
		bool deflecting{ false };
		if (deflecting) {
			a.velocity.x *= timeLeft;
			a.velocity.y *= timeLeft;
			if (std::abs(normal.x) > 0.0001f)
				a.velocity.x = -a.velocity.x;
			if (std::abs(normal.y) > 0.0001f)
				a.velocity.y = -a.velocity.y;
		} else {
			glm::vec3 tangent = { normal.y, normal.x, 0.0f };
			float cos_theta = glm::dot(a.velocity, tangent) * timeLeft;
			a.velocity = cos_theta * tangent;
		}
		a.position += a.velocity;
	}


	void DungeonActorComponent::handleCollisionDynamic(Actor& a, Actor& b) {
		return;
		// backup a's position
		glm::vec3 curPosition = a.position;
		glm::vec3 curVelocity = a.velocity;
		a.velocity = a.position - a.lastPosition;
		a.position = a.lastPosition;
		glm::vec3 normal;
		float collisionTime = SweptAABB(a, b, normal);
		if (collisionTime >= 1.0f) {
			a.position = curPosition;
			a.velocity = curVelocity;
			return;
		}
		a.position += a.velocity * collisionTime;
		float timeLeft = 1.0f - collisionTime;
		bool deflecting{ true };
		if (deflecting) {
			a.velocity.x *= timeLeft;
			a.velocity.y *= timeLeft;
			if (std::abs(normal.x) > 0.0001f)
				a.velocity.x = -a.velocity.x;
			if (std::abs(normal.y) > 0.0001f)
				a.velocity.y = -a.velocity.y;
		} else {
			glm::vec3 tangent = { normal.y, normal.x, 0.0f };
			float cos_theta = glm::dot(a.velocity, tangent) * timeLeft;
			a.velocity = cos_theta * tangent;
		}
		a.position += a.velocity;
	}


	void DungeonActorComponent::handleCollisionWorld(Actor& a, World& w) {
		return;
		// determine whether to move the player up, or to the left
		int ix1 = (int)(a.position.x);
		int iy1 = (int)(a.position.y);
		int ix2 = ix1 + 1; //(int)(a.position.x + a.size.x);
		int iy2 = iy1 + 1; //(int)(a.position.y + a.size.y);
		float fx1 = a.position.x - ix1;
		float fy1 = a.position.y - iy1;
		float fx2 = a.position.x + a.size.x - ix2;
		float fy2 = a.position.y + a.size.y - iy2;
		constexpr float tw = 1.0f;
		constexpr float th = 1.0f;
		float tx1 = std::floor(a.position.x);
		float tx2 = tx1 + tw;
		float ty1 = std::floor(a.position.y);
		float ty2 = tx2 + th;

		bool leftHalf = fx1 < 0.5f;
		bool rightHalf = fx1 > 0.5f;
		bool topHalf = fy1 < 0.5f;
		bool bottomHalf = fy1 > 0.5f;

		bool tl = w.getTile(ix1, iy1).solid();
		bool tr = w.getTile(ix2, iy1).solid();
		bool bl = w.getTile(ix1, iy2).solid();
		bool br = w.getTile(ix2, iy2).solid();
		int collision = 0;
		// ####### We handle all cases of blocks with a switch statement
		// #TL#TR#
		// #######
		// #BL#BR#
		// #######
		collision |= tl ? 1 : 0;
		collision |= tr ? 2 : 0;
		collision |= bl ? 4 : 0;
		collision |= br ? 8 : 0;

		constexpr int BLOCKS_RIGHT = 2 | 8;
		constexpr int BLOCKS_LEFT = 1 | 4;
		constexpr int BLOCKS_ABOVE = 1 | 2;
		constexpr int BLOCKS_BELOW = 4 | 8;

		// if ((collision & BLOCKS_RIGHT) && !(collision & BLOCKS_LEFT))
		//    collision = BLOCKS_RIGHT;
		// if (!(collision & BLOCKS_RIGHT) && (collision & BLOCKS_LEFT))
		//    collision = BLOCKS_LEFT;
		// if ((collision & BLOCKS_ABOVE) && !(collision & BLOCKS_BELOW))
		//    collision = BLOCKS_ABOVE;
		// if (!(collision & BLOCKS_ABOVE) && (collision & BLOCKS_BELOW))
		//    collision = BLOCKS_BELOW;

		float x1 = (float)ix2 - a.size.x;
		float x2 = (float)ix2; // ix1 + 1.0f;
		float y1 = (float)iy2 - a.size.y;
		float y2 = (float)iy2; // iy1 + 1.0f;
		int move = 0;
		constexpr int MOVE_LT = 1;
		constexpr int MOVE_RT = 2;
		constexpr int MOVE_DN = 4;
		constexpr int MOVE_UP = 8;
		switch (collision) {
		case 0: // nothing, so do nothing
			break;
		case 1: // top left single block
			if (fx1 > fy1)
				move = MOVE_RT;
			else
				move = MOVE_DN;
			break;
		case 2: // top right single block
			if (fy1 < 0.99f)
				move = MOVE_LT;
			else
				move = MOVE_DN;
			break;
		case BLOCKS_ABOVE: // top wall
			move = MOVE_DN;
			break;
		case 4: // bottom left single block
			if (fx1 > 0.99f)
				move = MOVE_RT;
			else
				move = MOVE_UP;
			break;
		case BLOCKS_LEFT: // left wall
			move = MOVE_RT;
			break;
		case 6: // bottom left single block, top right single block
				// .#
				// #.
			if (leftHalf)
				move |= MOVE_LT;
			if (topHalf)
				move |= MOVE_UP;
			if (rightHalf)
				move |= MOVE_RT;
			if (bottomHalf)
				move |= MOVE_DN;
			break;
		case 7: // top left wall
			move = MOVE_RT | MOVE_DN;
			break;
		case 8: // bottom right single block
			if (fx1 < fy1)
				move = MOVE_LT;
			else
				move = MOVE_UP;
			break;
		case 9: // top left single block, bottom right single block
				// #.
				// .#
			if (leftHalf)
				move |= MOVE_LT;
			if (bottomHalf)
				move |= MOVE_DN;
			if (rightHalf)
				move |= MOVE_RT;
			if (topHalf)
				move |= MOVE_UP;
			break;
		case BLOCKS_RIGHT: // right wall
			move = MOVE_LT;
			break;
		case 11: // top right wall
			move = MOVE_LT | MOVE_DN;
			break;
		case BLOCKS_BELOW: // bottom wall
			move = MOVE_UP;
			break;
		case 13: // bottom left wall
			move = MOVE_RT | MOVE_UP;
			break;
		case 14: // bottom right wall
			move = MOVE_LT | MOVE_UP;
			break;
		case 15: // all walls (just do nothing)
			break;
		}

		int velx = 0;
		int vely = 0;
		if (move & MOVE_LT) {
			a.position.x = x1;
			velx = 1;
		} else if (move & MOVE_RT) {
			a.position.x = x2;
			velx = 1;
		}
		if (move & MOVE_UP) {
			a.position.y = y1;
			vely = 1;
		} else if (move & MOVE_DN) {
			a.position.y = y2;
			vely = 1;
		}
		constexpr int elastic = 1;
		if (elastic) {
			if (velx)
				a.velocity.x = -a.velocity.x;
			if (vely)
				a.velocity.y = -a.velocity.y;
		} else {
			if (velx)
				a.velocity.x = 0.0f;
			if (vely)
				a.velocity.y = 0.0f;
		}
	}


	void DungeonActorComponent::beginOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Actor '%d' is now overlapping trigger actor '%d'", a.getId(), b.getId());
	}


	void DungeonActorComponent::endOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Actor '%d' is not overlapping trigger actor '%d'", a.getId(), b.getId());
	}

	void DungeonActorComponent::beginTriggerOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Trigger actor '%d' is now overlapped by actor '%d'", a.getId(), b.getId());
		if (triggerInfo.t <= 0.0f) {
			// if we are not already animating
			a.anim.baseId = a.spriteId() + 50;
		}
		triggerInfo.position = a.position;
		triggerInfo.t = 2.0f;
		b.position.x = 1 + random.positive() * (Locator::getWorld()->worldSizeX - 2);
		b.position.y = 1 + random.positive() * (Locator::getWorld()->worldSizeY - 2);
		Locator::getAudio()->playAudio(1, true);
	}


	void DungeonActorComponent::endTriggerOverlap(Actor& a, Actor& b) {
		HFLOGDEBUG("Trigger actor '%d' is not overlapped by actor '%d'", a.getId(), b.getId());
	}
} // namespace GameLib
