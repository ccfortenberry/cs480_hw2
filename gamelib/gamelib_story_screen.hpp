#ifndef GAMELIB_STORY_SCREEN_HPP
#define GAMELIB_STORY_SCREEN_HPP

#include <gamelib_actor.hpp>
#include <gamelib_font.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
	// StoryScreen
	// This presents a story screen
	class StoryScreen {
	public:
		StoryScreen();

		void play();
		bool load(const std::string& path);
		bool save(const std::string& path);

		// N characters per tick
		static constexpr int TICKS_PER_CHAR = 50;

		static constexpr int HALIGN_LEFT = Font::HALIGN_LEFT;
		static constexpr int HALIGN_CENTER = Font::HALIGN_CENTER;
		static constexpr int HALIGN_RIGHT = Font::HALIGN_RIGHT;
		static constexpr int VALIGN_TOP = Font::VALIGN_TOP;
		static constexpr int VALIGN_BOTTOM = Font::VALIGN_BOTTOM;
		static constexpr int VALIGN_CENTER = Font::VALIGN_CENTER;
		static constexpr int SHADOWED = Font::SHADOWED;

		// loads a font for use in the story screen, size should be set to 1 for ~1/25 height
		void setFont(int font, const std::string& path, float size);
		void setFontStyle(int font, int shadowed, int halign, int valign);

		// loads an image for use in the story screen
		void setImage(int image, const std::string& path, float w, float h);

		// sound effects
		void setBlipSound(int blipSoundId) { blipSoundId_ = blipSoundId; }

		void newFrame(int duration,
			int headerColor,
			int headerShadowColor,
			int textColor,
			int textShadowColor,
			int backColor1,
			int backColor2 = -1);
		void frameImage(int image, glm::vec2 position1, glm::vec2 position2, glm::vec2 scale, glm::vec2 rotRange);
		void frameImageOps(glm::vec2 fadeRange, glm::vec2 scaleLfo, glm::vec2 rotLfo, glm::vec4 lfo);
		void frameHeader(int font, const std::string& header);
		void frameLine(int font, const std::string& line);

		// writes story screen to output stream
		// this is in progress
		// format:
		// INT      numberOfActors
		// ARRAY OF
		//   STRING   actorName
		//   STRING   actorImage
		//   FLOAT    actorSizeX % of min(screen W, screen H)
		//   FLOAT    actorSizeY % of min(screen W, screen H)
		// INT      frame duration    If < 0, then no more lines of dialog
		// STRING   actorName
		// INT      backColor      4 bits = color1, 4 bits = color2, 3 bits = mix, 1 bit = negative
		// INT      textColor      4 bits = color1, 4 bits = color2, 3 bits = mix, 1 bit = negative
		// STRING   headerText
		// INT      headerFlags
		// INT      imageFlags     1 = HLEFT, 2 = HRIGHT, 3 = HCENTER, 4 = VTOP, 8 = VBOTTOM, 12 = VCENTER
		// INT      textFlags      1 = HLEFT, 2 = HRIGHT, 3 = HCENTER, 4 = COMPLEX
		// INT      animFlags      1 = POSITION, 2 = OFFSET, 2 = SCALE,
		// FLOAT    X1
		// FLOAT    Y1
		// FLOAT    X2
		// FLOAT    Y2
		// INT      OSRL flags     1 = OFFSET, 2 = SCALE, 4 = ROTATION, 8 = LFO
		// FLOAT    OFFSET X1
		// FLOAT    OFFSET Y1
		// FLOAT    OFFSET X2
		// FLOAT    OFFSET Y2
		// FLOAT    SCALE 1
		// FLOAT    SCALE 2
		// FLOAT    SCALE LFO +
		// FLOAT    SCALE LFO *
		// FLOAT    ROTATE ANGLE 1
		// FLOAT    ROTATE ANGLE 2
		// FLOAT    ROTATE LFO +
		// FLOAT    ROTATE LFO *
		// FLOAT    LFO FREQUENCY
		// FLOAT    LFO PHASE OFFSET
		// FLOAT    LFO AMPLITUDE
		// FLOAT    LFO RAMP
		// INT      line count
		// STRING[] lines
		bool writeStream(std::ostream& os);

		// returns false if EOF encountered during read
		bool readStream(std::istream& is);

	protected:
		using string_vector = std::vector<std::string>;

		struct Dialogue {
			static constexpr int NONE = 0;
			static constexpr int OFFSET = 1;
			static constexpr int SCALE = 2;
			static constexpr int ROTATION = 4;
			static constexpr int LFO = 8;

			SDL_Texture* actorSprite{ nullptr };

			std::string actorName;			   // name to display on screen
			int duration{ 0 };				   // milliseconds
			int backColor1{ 0 };			   // LibXOR Color
			int backColor2{ 0 };			   // LibXOR Color
			int textColor{ 4 };				   // LibXOR Color
			int textShadow{ 0 };			   // LibXOR Color
			std::string headerText;			   // Big 2xPt header text
			int headerColor{ 4 };			   // LibXOR Color
			int headerShadow{ 0 };			   // LibXOR Color
			int headerFont{ 0 };			   // Font index
			int imageFlags{ 0 };			   //
			int image{ -1 };				   // image to use for frame
			glm::vec2 fadeRange{ 0.1f, 0.9f }; // range for image fade
			int textFont{ 0 };				   // Font index
			int animFlags{ 0 };				   //
			glm::vec2 position1{ 0 };		   // position 1 of sprite
			glm::vec2 position2{ 0 };		   // position 2 of sprite
			int osrlFlags{ 0 };				   // offset, scale, rotation, lfo flags
			glm::vec2 offset1{ 0, 0 };		   // sprite offset 1 for rotation
			glm::vec2 offset2{ 0, 0 };		   // sprite offset 2 for rotation
			glm::vec2 scale{ 1.0f, 1.0f };	   // start and end scale of sprite
			glm::vec2 scaleLfo{ 0 };		   // {x} add, {y} mul
											   // scale + lfoAdd * lfo
											   // (lfoMul * lfo + 1) * scale
			glm::vec2 rotation{ 0.0f, 0.0f };  // (degrees) start rotation of sprite
			glm::vec2 rotLfo{ 0 };			   // {x} add, {y} mul
											   // rotation + lfoAdd * lfo
											   // (lfoMul * lfo + 1) * rotation
			float lfoAmplitude{ 1 };		   // unitless
			float lfoFrequency{ 1 };		   // degrees
			float lfoPhase{ 0 };			   // degrees
			float lfoRamp{ 0 };				   // milliseconds
			string_vector lines;
		};

		// editing variables
		// int curFont_{ 0 };
		int blipSoundId_{ 0 };

		// playback variables
		size_t lastCharsDrawn_{ 0 };

		// dialogue variables
		using dialogue_vector = std::vector<Dialogue>;
		std::map<std::string, ActorPtr> actors;
		dialogue_vector dialogue;
		int curframe{ 0 };
		float framePct{ 0 };
		int ticksLeft{ 0 };
		float tickCount{ 0 };
		float lag{ 0 };
		static constexpr float MS_PER_UPDATE = 0.001f;
		Context* context{ nullptr };
		float screenSize;
		float screenWidth;
		float screenHeight;
		bool donePlaying{ false };

		std::vector<std::pair<int, std::string>> reflow;
		struct LINEINFO {
			int width{ 0 };
			int height{ 0 };
			int first{ 0 };
			int count{ 0 };
			std::string line;
		};
		// width of entire line, height, index of first token, number of tokens
		std::vector<LINEINFO> reflowLines;

		int ptsize{ 0 };
		static constexpr int MAX_FONTS = 16;
		struct FONTINFO {
			std::unique_ptr<Font> font;
			int ptsize{ 12 };
			int spacew{ 10 };
			int h{ 12 };
			int shadow{ SHADOWED };
			int halign{ HALIGN_LEFT };
			int valign{ VALIGN_TOP };
			int flags() const { return shadow | halign | valign; }
			int calcWidth(const char* s) const { return font->calcWidth(s); }
			int calcWidth(const std::string& s) const { return font->calcWidth(s.c_str()); }
			void draw(int x, int y, const char* s, SDL_Color fg, SDL_Color bg) { font->draw(x, y, s, fg, bg, shadow); }
			void draw(int x, int y, const std::string& s, SDL_Color fg, SDL_Color bg) { draw(x, y, s.c_str(), fg, bg); }
		} fonts[MAX_FONTS];

		static constexpr int MAX_IMAGES = 16;
		struct IMAGEINFO {
			SDL_Texture* texture{ nullptr };
			SDL_Rect rect;
			glm::vec2 size;
		} images[MAX_IMAGES];

		struct TEXTRECT {
			int windowX{ 0 };
			int windowY{ 0 };
			int windowW{ 0 };
			int windowH{ 0 };
			int x{ 0 };
			int y{ 0 };

			void reset(int ix, int iy, int iw, int ih, int border) {
				windowX = ix + border;
				windowY = iy + border;
				windowW = iw - (border << 1);
				windowH = ih - (border << 1);
			}

			void calcx(int halign, int textW) {
				x = 0;
				switch (halign) {
				case HALIGN_LEFT: x = 0; break;
				case HALIGN_RIGHT: x = windowW - textW; break;
				case HALIGN_CENTER: x = (windowW - textW) >> 1; break;
				}
				x += windowX;
			}

			void calcy(int valign, int textH) {
				y = 0;
				switch (valign) {
				case VALIGN_TOP: y = 0; break;
				case VALIGN_BOTTOM: y += windowH - textH; break;
				case VALIGN_CENTER: y += (windowH - textH) >> 1; break;
				}
				y += windowY;
			}

			void calc(int halign, int valign, int textW, int textH) {
				calcx(halign, textW);
				calcy(valign, textH);
			}
		};

		void _advanceFrame(int frame);
		void _updateFrame();
		virtual void _drawFrame();
		void _reflowText(Dialogue& d);
	};

} // namespace GameLib

#endif
