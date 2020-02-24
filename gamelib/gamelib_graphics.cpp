#include "pch.h"
#include <gamelib_graphics.hpp>
#include <gamelib_locator.hpp>
#include <gamelib_world.hpp>

namespace GameLib {

	Graphics::Graphics(Context* ctx) {
		context = ctx;
		screensize = { context->screenWidth, context->screenHeight };
		origin_ = { context->screenWidth >> 1, context->screenHeight >> 1 };
		center_ = origin_;
	}

	Graphics::~Graphics() {}

	void Graphics::draw(int tileSetId, int tileId, float x, float y) { draw(tileSetId, tileId, (int)x, (int)y); }

	void Graphics::draw(int tileSetId, int tileId, int x, int y) {
		auto tileImage = context->getTile(tileSetId, tileId);
		if (!tileImage)
			return;
		glm::ivec2 p = transform({ x, y });
		if (clip(p))
			return;
		context->drawTexture(p, { tileImage->w, tileImage->h }, tileImage->texture);
	}

	void Graphics::draw(int tileSetId, int tileId, int x, int y, int flipFlags) {
		SPRITEINFO spriteInfo;
		glm::ivec2 p = transform({ x, y });
		if (clip(p))
			return;
		spriteInfo.position = p;
		spriteInfo.flipFlags = flipFlags;
		spriteInfo.center = { 0, 0 };
		spriteInfo.angle = 0.0f;
		context->drawTexture(tileSetId, tileId, spriteInfo);
	}

	void Graphics::draw(int x, int y, int w, int h, SDL_Color color) {
		glm::ivec2 p = transform({ x, y });
		if (clip(p, { w, h }))
			return;
		SDL_Rect rect{ p.x, p.y, w, h };
		SDL_SetRenderDrawColor(context->renderer(), color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(context->renderer(), &rect);
	}

	void Graphics::draw(glm::ivec2 c, glm::ivec2 size, SDL_Color color) {
		glm::ivec2 p = transform(c);
		int hx = size.x >> 1;
		int hy = size.y >> 1;
		SDL_Rect rect{ p.x - hx, p.y - hy, size.x, size.y };
		SDL_SetRenderDrawColor(context->renderer(), color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(context->renderer(), &rect);
	}

	void Graphics::line(glm::ivec2 p1, glm::ivec2 p2, SDL_Color color) {
		SDL_SetRenderDrawColor(context->renderer(), color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
		p1 = transform(p1);
		p2 = transform(p2);
		SDL_RenderDrawLine(context->renderer(), p1.x, p1.y, p2.x, p2.y);
	}
} // namespace GameLib
