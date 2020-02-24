#include "pch.h"
#include <gamelib_graphics_component.hpp>
#include <gamelib_locator.hpp>

namespace GameLib {
	void debugDrawSDF(Actor& a, Actor& b) {
		World* world = Locator::getWorld();
		Graphics& g = *dynamic_cast<Graphics*>(Locator::getGraphics());
		glm::vec2 c1 = a.center2d();
		glm::vec2 c2 = b.center2d();
		glm::vec2 p1 = a.support(c2);
		glm::vec2 p2 = b.support(c1);
		c1 *= g.tileSizef();
		c2 *= g.tileSizef();
		p1 *= g.tileSizef();
		p2 *= g.tileSizef();
		if (a.touching(b) < 0.0f) {
			g.draw(c1, { 16, 16 }, Yellow);
			g.draw(c2, { 16, 16 }, Rose);
		} else {
			g.draw(c1, { 16, 16 }, Orange);
			g.draw(c2, { 16, 16 }, Violet);
		}
		g.line(c1, p1, Red);
		g.line(p1, p2, ForestGreen);
		g.line(p2, c2, Blue);
	}

	void debugDraw(Actor& a) {
		return;
		World* world = Locator::getWorld();
		auto graphics = Locator::getGraphics();

		if (world && graphics) {
			int ix1 = (int)(a.position.x);
			int iy1 = (int)(a.position.y);
			int ix2 = ix1 + 1;
			int iy2 = iy1 + 1;
			int tl = world->getTile(ix1, iy1).solid() ? 9 : 8;
			int tr = world->getTile(ix2, iy1).solid() ? 9 : 8;
			int bl = world->getTile(ix1, iy2).solid() ? 9 : 8;
			int br = world->getTile(ix2, iy2).solid() ? 9 : 8;
			float fx = fract(a.position.x);
			float fy = fract(a.position.y);
			int w = graphics->getTileSizeX();
			int h = graphics->getTileSizeY();
			ix1 *= w;
			ix2 *= w;
			iy1 *= h;
			iy2 *= h;
			graphics->draw(LIBXOR_TILESET32, tl, ix1, iy1);
			graphics->draw(LIBXOR_TILESET32, tr, ix2, iy1);
			graphics->draw(LIBXOR_TILESET32, bl, ix1, iy2);
			graphics->draw(LIBXOR_TILESET32, br, ix2, iy2);
			int dx = ix1 + (w << 1);
			int dy = iy1;
			int dw = 8;
			int dh = 8;
			if (fx < 0.5f)
				graphics->draw(dx, dy, dw, dh, Green);
			if (fx > 0.5f)
				graphics->draw(dx, dy, dw, dh, Red);
			dx += dw;
			if (fy < 0.5f)
				graphics->draw(dx, dy, dw, dh, Green);
			if (fy > 0.5f)
				graphics->draw(dx, dy, dw, dh, Red);
			dx += dw;
			if (fx < fy)
				graphics->draw(dx, dy, dw, dh, Violet);
			else if (fx > fy)
				graphics->draw(dx, dy, dw, dh, Gold);
			dx += dw;
			if (fx > 0.99f)
				graphics->draw(dx, dy, dw, dh, Green);
			else
				graphics->draw(dx, dy, dw, dh, Red);
			dx += dw;
			if (fy < 0.99f)
				graphics->draw(dx, dy, dw, dh, Green);
			else
				graphics->draw(dx, dy, dw, dh, Red);
		}
	}

	void calcAABB(glm::vec3& ap, glm::vec3& as, glm::vec3& av, glm::vec3& a, glm::vec3& b) {
		glm::vec3 amin;
		amin.x = av.x > 0 ? ap.x : ap.x + av.x;
		amin.y = av.y > 0 ? ap.y : ap.y + av.y;
		amin.z = av.z > 0 ? ap.z : ap.z + av.z;
		glm::vec3 asize;
		asize.x = av.x > 0 ? as.x + av.x : as.x - av.x;
		asize.y = av.y > 0 ? as.y + av.y : as.y - av.y;
		asize.z = av.z > 0 ? as.z + av.z : as.z - av.z;
		glm::vec amax = amin + asize;
		a = amin;
		b = amax;
	}

	void debugDrawSweptAABB(Actor& a) {
		auto graphics = Locator::getGraphics();
		if (graphics) {
			int w = graphics->getTileSizeX();
			int h = graphics->getTileSizeY();
			glm::vec3 boxmin, boxmax;
			calcAABB(a.position, a.size, a.velocity, boxmin, boxmax);
			int x1 = (int)(boxmin.x * w);
			int y1 = (int)(boxmin.y * h);
			int x4 = (int)(boxmax.x * w);
			int y4 = (int)(boxmax.y * h);
			glm::vec3 box1 = boxmin + a.size;
			glm::vec3 box2 = boxmax - a.size;
			int x2 = (int)(box1.x * w);
			int y2 = (int)(box1.y * h);
			int x3 = (int)(box2.x * w);
			int y3 = (int)(box2.y * h);
			graphics->draw(x1, y1, x4 - x1, y4 - y1, Yellow);
			graphics->draw(x1, y1, x2 - x1, y2 - y1, Orange);
			graphics->draw(x3, y3, x4 - x3, y4 - y3, Cyan);
		}
	}

	void SimpleGraphicsComponent::draw(Actor& actor, Graphics& graphics) {
		glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };
		glm::vec3 pos = actor.position * tileSize;
		int id = actor.anim.currentFrame();
		if (!id)
			id = actor.spriteId();
		graphics.draw(actor.sprite.libId, id, (int)pos.x, (int)pos.y, actor.sprite.flipFlags());
	}

	void DebugGraphicsComponent::draw(Actor& actor, Graphics& graphics) {
		glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };
		glm::vec3 pos = actor.position * tileSize;
		glm::vec3 size = actor.size * tileSize;
		graphics.draw(actor.sprite.libId, actor.sprite.id, (int)pos.x, (int)pos.y, actor.sprite.flipFlags());

		debugDraw(actor);
		graphics.draw((int)pos.x, (int)pos.y, (int)size.x, (int)size.y, ForestGreen);
	}
} // namespace GameLib
