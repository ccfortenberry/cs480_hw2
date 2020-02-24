#include "pch.h"
#include <gamelib_box2d.hpp>

namespace GameLib {
	Box2D::Box2D() {}


	Box2D::~Box2D() {}


	void Box2D::init() {
		//{ world_ = b2World{ gravity_ }; }

		StaticBody groundBody;
		groundBody.init(world_, { 0.0f, -10.0f }, { 50.0f, 10.0f }, 0.0f, 0.0f);
		staticBodies.push_back(groundBody);
	}


	void Box2D::setGravity(glm::vec2 a_g) {
		gravity_.x = a_g.x;
		gravity_.y = a_g.y;
		world_.SetGravity(gravity_);
	}


	void Box2D::update(float timeStep) {
		constexpr int velocityIterations = 1;
		constexpr int positionIterations = 1;
		world_.Step(timeStep, velocityIterations, positionIterations);
	}


	int Box2D::initBody(b2BodyType type, glm::vec2 position, glm::vec2 halfSize, float density, float friction) {
		int id = 0;
		StaticBody sbody;
		DynamicBody dbody;
		switch (type) {
		case b2_staticBody:
			sbody.init(world_, position, halfSize, density, friction);
			staticBodies.push_back(std::move(sbody));
			id = static_cast<int>(staticBodies.size() - 1);
			break;
		case b2_dynamicBody:
			dbody.init(world_, position, halfSize, density, friction);
			dynamicBodies.push_back(std::move(dbody));
			id = static_cast<int>(dynamicBodies.size() - 1);
			break;
		default: break;
		}
		return id;
	}
} // namespace GameLib
