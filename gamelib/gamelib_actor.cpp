#include "pch.h"
#include <gamelib_actor.hpp>
#include <gamelib_locator.hpp>

namespace GameLib {
	unsigned Actor::idSource_{ 0 };

	Actor::Actor(
		InputComponentPtr input,
		ActorComponentPtr actor,
		PhysicsComponentPtr physics,
		GraphicsComponentPtr graphics)
		: input_(input), actor_(actor), physics_(physics), graphics_(graphics) {
		id_ = idSource_++;
	}

	Actor::~Actor() { HFLOGDEBUG("Deleting (%d) '%s'", id_, name().c_str()); }

	void Actor::beginPlay(float t) {
		t0 = t;
		t1 = t;
		if (actor_)
			actor_->beginPlay(*this);
		if (physics_)
			physics_->beginPlay(*this);
	}

	void Actor::update(float deltaTime, World& world) {
		dt = deltaTime;
		t1 += dt;
		anim.update(deltaTime);
		if (input_)
			input_->update(*this);
		if (actor_)
			actor_->update(*this, world);
	}

	void Actor::preupdate() {
		if (physics_)
			physics_->preupdate(*this);
	}

	void Actor::postupdate() {
		if (physics_)
			physics_->postupdate(*this);
	}

	void Actor::physics(float deltaTime, World& world) {
		if (!physics_)
			return;
		lastPosition = position;
		physics_->update(*this, world);
		if (actor_) {
			if (physics_->collideWorld(*this, world))
				actor_->handleCollisionWorld(*this, world);

			for (auto b : world.staticActors) {
				if (this->getId() == b->getId())
					continue;
				if (physics_->collideStatic(*this, *b))
					actor_->handleCollisionStatic(*this, *b);
			}

			for (auto b : world.dynamicActors) {
				if (this->getId() == b->getId())
					continue;
				if (physics_->collideDynamic(*this, *b))
					actor_->handleCollisionDynamic(*this, *b);
			}

			if (triggerInfo.overlapping && triggerInfo.triggerActor.use_count()) {
				auto trigger = triggerInfo.triggerActor.lock();
				if (!physics_->collideTrigger(*this, *trigger)) {
					triggerInfo.overlapping = false;
					triggerInfo.triggerActor.reset();
					actor_->endOverlap(*this, *trigger);

					if (trigger->actor_) {
						trigger->triggerInfo.overlapping = false;
						trigger->actor_->endTriggerOverlap(*trigger, *this);
					}
				}
			} else {
				for (auto trigger : world.triggerActors) {
					if (this->getId() == trigger->getId())
						continue;
					if (!triggerInfo.overlapping && physics_->collideTrigger(*this, *trigger)) {
						triggerInfo.overlapping = true;
						actor_->beginOverlap(*this, *trigger);
						if (trigger->actor_) {
							trigger->triggerInfo.overlapping = true;
							trigger->actor_->beginTriggerOverlap(*trigger, *this);
							triggerInfo.triggerActor = trigger;
						}
					}
				}
			}
		}
		dPosition = position - lastPosition;
	}

	void Actor::draw(Graphics& graphics) {
		if (visible && graphics_)
			graphics_->draw(*this, graphics);
	}

	void Actor::switchAnim(int i) {
		if (i < 0 || i >= anims.size()) {
			anim.start(t1);
			return;
		}
		anim = anims[i];
		anim.start(t1);
	}

	void Actor::makeDynamic() { type_ = DYNAMIC; }

	void Actor::makeStatic() { type_ = STATIC; }

	void Actor::makeTrigger() { type_ = TRIGGER; }
} // namespace GameLib
