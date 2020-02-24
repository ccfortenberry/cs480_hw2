#ifndef GAMELIB_BOX2D_HPP
#define GAMELIB_BOX2D_HPP

#if __has_include(<box2d/box2d.h>)
#include <box2d/box2d.h>
#elif __has_include(<Box2D/Box2D.h>)
#include <Box2d/Box2d.h>
#else
#error "Box2D headers not found"
#endif
#include <glm/glm.hpp>
#include <hatchetfish.hpp>

namespace GameLib {
	struct PhysicsBody {
		b2BodyDef bodyDef;
		b2Body* body{ nullptr };
		b2PolygonShape shape;
		b2FixtureDef fixtureDef;

		PhysicsBody(b2BodyType type) { bodyDef.type = type; }

		void init(b2World& w, glm::vec2 position, glm::vec2 halfSize, float density, float friction) {
			bodyDef.position.x = position.x;
			bodyDef.position.y = position.y;
			body = w.CreateBody(&bodyDef);
			shape.SetAsBox(halfSize.x, halfSize.y);
			switch (bodyDef.type) {
			case b2_staticBody: body->CreateFixture(&shape, density); break;
			case b2_dynamicBody:
				fixtureDef.shape = &shape;
				fixtureDef.friction = friction;
				fixtureDef.density = density;
				body->CreateFixture(&fixtureDef);
				break;
			default:
				HFLOGERROR("Body definition type not supported");
				exit(0);
				break;
			}
		}

		// sets transform of body with no rotation
		void setPosition(glm::vec2 p) { body->SetTransform({ p.x, p.y }, 0.0f); }

		// sets linear velocity of body
		void setVelocity(glm::vec2 v) { body->SetLinearVelocity({ v.x, v.y }); }

		void applyImpulse(glm::vec2 v) { body->ApplyLinearImpulse({ v.x, v.y }, body->GetPosition(), false); }

		// returns current position of body
		glm::vec2 position() const {
			auto p = body->GetPosition();
			return { p.x, p.y };
		}

		// returns current linear of body
		glm::vec2 velocity() const {
			auto v = body->GetLinearVelocity();
			return { v.x, v.y };
		}
	};

	struct StaticBody : public PhysicsBody {
		StaticBody() : PhysicsBody(b2_staticBody) {}
	};

	struct DynamicBody : public PhysicsBody {
		DynamicBody() : PhysicsBody(b2_dynamicBody) {}
	};

	class Box2D {
	public:
		Box2D();
		~Box2D();

		void init();
		void setGravity(glm::vec2 a_g);
		void update(float timestep);

		// returns index to body in the list
		int initBody(b2BodyType type, glm::vec2 position, glm::vec2 halfSize, float density, float friction);

		PhysicsBody* getBody(int id, b2BodyType type = b2_dynamicBody) {
			switch (type) {
			case b2_staticBody: return &staticBodies[id];
			case b2_dynamicBody: return &dynamicBodies[id];
			default: return nullptr;
			}
		}


	private:
		b2Vec2 gravity_{ 0.0f, 9.8f };
		b2World world_{ gravity_ };

		std::vector<StaticBody> staticBodies;
		std::vector<DynamicBody> dynamicBodies;
	};
} // namespace GameLib

#endif
