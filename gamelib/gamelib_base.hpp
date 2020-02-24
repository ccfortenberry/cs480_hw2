#ifndef GAMELIB_BASE_HPP
#define GAMELIB_BASE_HPP

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <czmq.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <hatchetfish.hpp>
#if __has_include(<box2d/box2d.h>)
#include <box2d/box2d.h>
#elif __has_include(<Box2D/Box2D.h>)
#include <Box2D/Box2D.h>
#else
#error "Box2D headers not found"
#endif

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

namespace GameLib {
	static constexpr float EPSILON = 0.0001f;

	struct SPRITEINFO {
		glm::vec2 position;
		float angle;
		glm::vec2 center;
		int flipFlags;
	};

	struct TILEIMAGE {
		SDL_Texture* texture{ nullptr };
		int tileId{ 0 };
		int tilesetId{ 0 };
		int w{ 0 };
		int h{ 0 };
	};

	struct AUDIOINFO {
		Mix_Chunk* chunk{ nullptr };
		std::string name;

		operator bool() const { return chunk != nullptr; }

		void free() {
			Mix_FreeChunk(chunk);
			chunk = nullptr;
		}

		~AUDIOINFO() {
			if (chunk) {
				HFLOGERROR("Should not delete sound in destructor");
			}
		}
	};

	struct MUSICINFO {
		Mix_Music* chunk{ nullptr };
		std::string name;

		operator bool() const { return chunk != nullptr; }

		void free() {
			Mix_FreeMusic(chunk);
			chunk = nullptr;
		}

		~MUSICINFO() {
			if (chunk) {
				HFLOGERROR("Should not delete sound in destructor");
			}
		}
	};

	template <typename T>
	T clamp(T x, T a, T b) {
		return (x < a) ? a : (x > b) ? b : x;
	}

	template <typename T>
	T fract(T x) {
		return (T)(x - int(x));
	}

	// returns range 0 to 1 with 1 cycle per second
	template <typename T>
	T wave(T x, T cyclesPerSecond) {
		return (T)std::sin(x * cyclesPerSecond * 6.28318531f) * 0.5f + 0.5f;
	}

	template <typename T>
	T triangle(T x, T cyclesPerSecond) {
		float t = x * cyclesPerSecond;
		return (T)(2.0f * std::abs(t - std::floor(t + 0.5f)));
	}

	// Colors
	struct Color3f {
		static constexpr float UI8_TO_F32 = 0.003921569f;
		static constexpr float F32_TO_UI8 = 255.99f;

		Color3f() {}
		Color3f(float R, float G, float B) : r(R), g(G), b(B) {}
		Color3f(const SDL_Color& c) {
			r = c.r * UI8_TO_F32;
			g = c.g * UI8_TO_F32;
			b = c.b * UI8_TO_F32;
		}
		SDL_Color toU8() const { return { Uint8(r * F32_TO_UI8), Uint8(g * F32_TO_UI8), Uint8(b * F32_TO_UI8), 255 }; }

		Color3f toHSL() {
			float cmin = std::min(r, std::min(g, b));
			float cmax = std::max(r, std::max(g, b));
			float diff = cmax - cmin;
			float l = 0.5f * (cmin + cmax);
			float s = 0.0f;
			float h = 0.0;
			if (diff < UI8_TO_F32) {
				return { h, s, l };
			} else {
				if (l < 0.5f) {
					s = diff / (cmax + cmin);
				} else {
					s = diff / (2.0f - cmax - cmin);
				}

				float r2 = (cmax - r) / diff;
				float g2 = (cmax - g) / diff;
				float b2 = (cmax - b) / diff;

				if (r == cmax) {
					h = (g == cmin ? 5.0f + b2 : 1.0f - g2);
				} else if (g == cmax) {
					h = (b == cmin ? 1.0f + r2 : 3.0f - b2);
				} else {
					h = (r == cmin ? 3.0f + g2 : 5.0f - r2);
				}
				h /= 6.0f;

				if (h < 0.0f)
					h += 1.0f;
				else if (h > 1.0f)
					h -= 1.0f;
			}
			return { h, s, l };
		}

		Color3f toRGB() {
			float h = r;
			float s = g;
			float l = b;
			if (s == 0.0) {
				return { l, l, l }; // Luminance
			} else {
				float f2;

				if (l < 0.5f)
					f2 = l * (1.0f + s);
				else
					f2 = l + s - s * l;

				float f1 = 2.0f * l - f2;

				return { hue2rgb(f1, f2, h + (0.333333f)), hue2rgb(f1, f2, h), hue2rgb(f1, f2, h - (0.333333f)) };
			}
		}

		float hue2rgb(float f1, float f2, float hue) {
			if (hue < 0.0f)
				hue += 1.0f;
			else if (hue > 1.0f)
				hue -= 1.0f;
			float res = 0.0f;
			if ((6.0f * hue) < 1.0f)
				res = f1 + (f2 - f1) * 6.0f * hue;
			else if ((2.0f * hue) < 1.0f)
				res = f2;
			else if ((3.0f * hue) < 2.0f)
				res = f1 + (f2 - f1) * ((0.666667f) - hue) * 6.0f;
			else
				res = f1;
			return res;
		}

		Color3f& hueshift(int shift) {
			Color3f hsl = toHSL();
			switch (shift) {
			case 0: break;
			case 1: hsl.r += 7.5f / 360.0f; break;
			case 2: hsl.r += 15.0f / 360.0f; break;
			case 3: hsl.r += 180.0f / 360.0f; break;
			case 4: hsl.r += 240.0f / 360.0f; break;
			case 5: hsl.r += 120.0f / 360.0f; break;
			case 6: hsl.b *= 0.736646942f; break;
			case 7: hsl.b /= 0.736646942f; break;
			}
			*this = hsl.toRGB();
			return *this;
		}

		float r{ 0 };
		float g{ 0 };
		float b{ 0 };
	};


	static const int BLACK = 0;
	static const int GRAY33 = 1;
	static const int GRAY67 = 2;
	static const int WHITE = 3;
	static const int RED = 4;
	static const int ORANGE = 5;
	static const int YELLOW = 6;
	static const int GREEN = 7;
	static const int CYAN = 8;
	static const int AZURE = 9;
	static const int BLUE = 10;
	static const int VIOLET = 11;
	static const int ROSE = 12;
	static const int BROWN = 13;
	static const int GOLD = 14;
	static const int FORESTGREEN = 15;


	static const SDL_Color Black{ 0, 0, 0, 255 };
	static const SDL_Color Gray33{ 85, 85, 85, 255 };
	static const SDL_Color Gray67{ 170, 170, 170, 255 };
	static const SDL_Color White{ 255, 255, 255, 255 };
	static const SDL_Color Red{ 255, 0, 0, 255 };
	static const SDL_Color Orange{ 228, 114, 0, 255 };
	static const SDL_Color Yellow{ 228, 228, 0, 255 };
	static const SDL_Color Green{ 0, 255, 0, 255 };
	static const SDL_Color Cyan{ 0, 180, 180, 255 };
	static const SDL_Color Azure{ 0, 114, 228, 255 };
	static const SDL_Color Blue{ 0, 0, 255, 255 };
	static const SDL_Color Violet{ 114, 0, 228, 255 };
	static const SDL_Color Rose{ 228, 0, 114, 255 };
	static const SDL_Color Brown{ 127, 63, 0, 255 };
	static const SDL_Color Gold{ 212, 171, 56, 255 };
	static const SDL_Color ForestGreen{ 63, 127, 63, 255 };


	static const SDL_Color LibXORColors[] = {
		{ 0, 0, 0, 255 },
		{ 85, 85, 85, 255 },
		{ 170, 170, 170, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 0, 255 },
		{ 228, 114, 0, 255 },
		{ 228, 228, 0, 255 },
		{ 0, 255, 0, 255 },
		{ 0, 180, 180, 255 },
		{ 0, 114, 228, 255 },
		{ 0, 0, 255, 255 },
		{ 114, 0, 228, 255 },
		{ 228, 0, 114, 255 },
		{ 127, 63, 0, 255 },
		{ 212, 171, 56, 255 },
		{ 63, 127, 63, 255 },
	};


	// color1 (0-15), color2 (0-15), mix(0 - 7), neg(true/false)
	inline SDL_Color MakeColor(int color1, int color2, int mix, bool neg) {
		const SDL_Color& c1 = LibXORColors[color1];
		const SDL_Color& c2 = LibXORColors[color2];
		float t = clamp<float>(mix * 0.142857f, 0.0f, 1.0f);
		Uint8 r = (Uint8)clamp<int>((int)(t * c2.r + (1.0f - t) * c1.r), 0, 255);
		Uint8 g = (Uint8)clamp<int>((int)(t * c2.g + (1.0f - t) * c1.g), 0, 255);
		Uint8 b = (Uint8)clamp<int>((int)(t * c2.b + (1.0f - t) * c1.b), 0, 255);
		if (neg) {
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
		}
		return { r, g, b, 255 };
	}


	inline SDL_Color MakeColor(int color1, int color2, int mix, int shift1, int shift2, int neg) {
		Color3f c1f{ LibXORColors[color1] };
		Color3f c2f{ LibXORColors[color2] };
		c1f.hueshift(shift1);
		c2f.hueshift(shift2);
		SDL_Color c1 = c1f.toU8();
		SDL_Color c2 = c2f.toU8();
		float t = clamp<float>(mix * 0.142857f, 0.0f, 1.0f);
		Uint8 r = (Uint8)clamp<int>((int)(t * c2.r + (1.0f - t) * c1.r), 0, 255);
		Uint8 g = (Uint8)clamp<int>((int)(t * c2.g + (1.0f - t) * c1.g), 0, 255);
		Uint8 b = (Uint8)clamp<int>((int)(t * c2.b + (1.0f - t) * c1.b), 0, 255);
		if (neg) {
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
		}
		return { r, g, b, 255 };
	}

	// color1 (0-15), color2 (0-15), mix (0-1), neg(true/false)
	inline SDL_Color MakeColorHI(int color1, int color2, float mix, bool neg) {
		const SDL_Color& c1 = LibXORColors[color1];
		const SDL_Color& c2 = LibXORColors[color2];
		float t = clamp<float>(mix, 0.0f, 1.0f);
		Uint8 r = (Uint8)clamp<int>((int)(t * c2.r + (1.0f - t) * c1.r), 0, 255);
		Uint8 g = (Uint8)clamp<int>((int)(t * c2.g + (1.0f - t) * c1.g), 0, 255);
		Uint8 b = (Uint8)clamp<int>((int)(t * c2.b + (1.0f - t) * c1.b), 0, 255);
		if (neg) {
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
		}
		return { r, g, b, 255 };
	}

	constexpr int ComposeColor(int color1, int color2, int mix, int shift1, int shift2, int neg) {
		int color = color1 & 0xF;
		color |= ((color2 & 0xF) << 4);
		color |= ((mix & 0x7) << 8);
		color |= ((shift1 & 0x3) << 11);
		color |= ((shift2&0x3)<<13);
		color |= ((neg&0x1)<<15);
		return color;
	}

	inline SDL_Color MakeColor(int c) {
		int color1 = (c >> 0) & 0xF;
		int color2 = (c >> 4) & 0xF;
		int mix = (c >> 8) & 0x7;
		int shift1 = (c >> 11) & 0x03;
		int shift2 = (c >> 13) & 0x03;
		int neg = (c >> 15) & 0x1;
		return MakeColor(color1, color2, mix, shift1, shift2, neg);
	}

	inline SDL_Color mix(SDL_Color a, SDL_Color b, float mix) {
		float amix = 1.0f - mix;
		return { (Uint8)clamp(a.r * amix + b.r * mix, 0.0f, 255.99f),
			(Uint8)clamp(a.g * amix + b.g * mix, 0.0f, 255.99f),
			(Uint8)clamp(a.b * amix + b.b * mix, 0.0f, 255.99f),
			(Uint8)clamp(a.a * amix + b.a * mix, 0.0f, 255.99f) };
	}
} // namespace GameLib

#endif
