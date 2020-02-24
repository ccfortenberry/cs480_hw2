#ifndef GAMELIB_WORLD_HPP
#define GAMELIB_WORLD_HPP

#include <gamelib_graphics.hpp>
#include <gamelib_object.hpp>

namespace GameLib {
	// number of screens in the X direction
	constexpr int WorldPagesX = 4;
	// number of screens in the Y direction
	constexpr int WorldPagesY = 4;

	// number of tiles on one page in the X direction
	constexpr int WorldTilesX = 40;
	// number of tiles on one page in the Y direction
	constexpr int WorldTilesY = 22;

	constexpr int CollisionTileResolution = 4;

	class Tile {
	public:
		Tile() {}
		Tile(unsigned id, char orig) : spriteId(id), charDesc(orig) {}

		static constexpr unsigned EMPTY = 0;
		static constexpr unsigned SOLID_TL = 0x01;
		static constexpr unsigned SOLID_TR = 0x02;
		static constexpr unsigned SOLID_BL = 0x04;
		static constexpr unsigned SOLID_BR = 0x08;
		static constexpr unsigned SOLID = SOLID_TL | SOLID_TR | SOLID_BL | SOLID_BR;

		bool solid() const { return flags & SOLID; }
		bool empty() const { return !solid(); }

		char charDesc{ '?' };
		unsigned spriteId{ 0 };
		unsigned flags{ EMPTY };
		int box2dId { -1 };
	};

	class Actor;
	using ActorPtr = std::shared_ptr<Actor>;
	using ActorWPtr = std::weak_ptr<Actor>;

	// World represents a composite of Objects that live in a 2D grid world
	class World : public Object {
	public:
		World();
		virtual ~World();

		void resize(unsigned sizeX, unsigned sizeY);

		void start(float t);
		void update(float deltaTime);
		void physics(float deltaTime);
		void drawTiles(Graphics& graphics);
		void draw(Graphics& graphics);

		void setTile(int x, int y, Tile ptr);
		Tile& getTile(int x, int y);
		const Tile& getTile(glm::vec3 p) const { return getTile((int)p.x, (int)p.y); }
		const Tile& getTile(glm::vec3 p, int offsetX, int offsetY) const {
			return getTile((int)p.x + offsetX, (int)p.y + offsetY);
		}
		const Tile& getTile(int x, int y) const;
		const Tile& getTilef(float x, float y) const { return getTile(int(x), int(y)); }
		int getCollisionTile(float x, float y) const;
		void setCollisionTile(float x, float y, int value);

		std::istream& readCharStream(std::istream& s) override;
		std::ostream& writeCharStream(std::ostream& s) const override;

		std::vector<Tile> tiles;
		std::vector<uint8_t> collisionTiles;

		// Dynamic actors are solid actors with game logic
		std::vector<ActorPtr> dynamicActors;
		// Static actors are solid actors with no game logic
		std::vector<ActorPtr> staticActors;
		// Trigger actors are not solid
		std::vector<ActorPtr> triggerActors;

	public:
		void addDynamicActor(ActorPtr a);
		void addStaticActor(ActorPtr a);
		void addTriggerActor(ActorPtr a);

		// number of tiles in the X direction
		int worldSizeX{ WorldPagesX * WorldTilesX };

		// number of tiles in the Y direction
		int worldSizeY{ WorldPagesY * WorldTilesY };

		// number of collision tiles in one tile
		int collisionSizeX = worldSizeX * CollisionTileResolution;

		// number of collision tiles vertically
		int collisionSizeY = worldSizeY * CollisionTileResolution;

		// Global physics constants for world
		struct PHYSICSINFO {
			float c_r{ 0.5f };				 // coefficient of resitution
			float c_f{ 0.5f };				 // coefficient of friction
			glm::vec3 g{ 0.0f, 9.8f, 0.0f }; // gravity acceleration
			float d{ 0.05f };				 // air resistance
			glm::vec3 v_wind;				 // wind velocity
		} worldPhysicsInfo;

	protected:
		virtual void _draw(Graphics& g);
		virtual void _addTileToPhysics(int x, int y);
	};
} // namespace GameLib

#endif
