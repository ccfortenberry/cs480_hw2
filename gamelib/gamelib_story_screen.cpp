#include "pch.h"
#include <algorithm>
#include <cmath>
#include <gamelib_locator.hpp>
#include <gamelib_story_screen.hpp>

namespace GameLib {
	StoryScreen::StoryScreen() {
		context = Locator::getContext();
		screenWidth = (float)context->screenWidth;
		screenHeight = (float)context->screenHeight;
		screenSize = std::min(screenWidth, screenHeight);
		ptsize = (int)(screenSize / 25.0f);
		for (auto i = 0; i < MAX_FONTS; ++i) {
			fonts[i].font = std::make_unique<Font>(context);
		}
		newFrame(0, 0, 0, 0, 0, 0);
	}


	void StoryScreen::setFont(int font, const std::string& path, float size) {
		if (font < 0 || font >= MAX_FONTS)
			return;

		FONTINFO& f = fonts[font];
		f.ptsize = (int)std::floor(0.5f + size * ptsize);
		if (!f.font->load(path, f.ptsize)) {
			HFLOGERROR("Font '%s' not found", path.c_str())
		}
		f.spacew = f.calcWidth(" ");
		f.h = f.font->calcHeight();
	}


	void StoryScreen::setFontStyle(int font, int shadow, int halign, int valign) {
		if (font < 0 || font >= MAX_FONTS)
			return;
		FONTINFO& f = fonts[font];
		if (shadow & SHADOWED)
			f.shadow = shadow & SHADOWED;
		if (halign & HALIGN_CENTER)
			f.halign = halign & HALIGN_CENTER;
		if (valign & VALIGN_CENTER)
			f.valign = valign & VALIGN_CENTER;
	}


	void StoryScreen::setImage(int image, const std::string& path, float w, float h) {
		if (image < 0 || image >= MAX_IMAGES)
			return;
		images[image].texture = context->loadImage(path);
		if (!images[image].texture) {
			HFLOGWARN("Image not found '%s'", path.c_str());
		}
		images[image].size.x = w * ptsize;
		images[image].size.y = h * ptsize;
	}

	void StoryScreen::play() {
		curframe = 0;
		Hf::StopWatch stopwatch;

		InputCommand abutton;
		InputCommand enterButton;
		InputCommand escapeButton;
		InputHandler input;
		input.buttonA = &abutton;
		input.start = &enterButton;
		input.back = &escapeButton;
		GameLib::InputHandler* oldinput = Locator::getInput();
		Locator::provide(&input);

		float t0 = stopwatch.stop_msf();
		tickCount = 0;
		_advanceFrame(0);
		while (!donePlaying) {
			context->getEvents();
			input.handle();
			float t1 = stopwatch.stop_sf();
			float dt = t1 - t0;
			lag += dt;
			t0 = t1;
			framePct = tickCount / (float)dialogue[curframe].duration;
			while (lag >= MS_PER_UPDATE) {
				_updateFrame();
				lag -= MS_PER_UPDATE;
				ticksLeft--;
				tickCount++;
			}
			_drawFrame();
			bool a = abutton.checkClear();
			bool b = enterButton.checkClear();
			bool c = escapeButton.checkClear();
			bool anykey = a || b || c;
			if (ticksLeft < 0 || anykey) {
				_advanceFrame(curframe + 1);
				tickCount = 0.0f;
			}
			context->swapBuffers();
		}

		Locator::provide(oldinput);
	}


	bool StoryScreen::load(const std::string& path) {
		std::string foundPath = context->findSearchPath(path);
		std::ifstream fin(foundPath);
		if (!fin)
			return false;
		return readStream(fin);
	}


	bool StoryScreen::save(const std::string& path) {
		std::ofstream fout(path);
		if (!fout)
			return false;
		return writeStream(fout);
	}


	void StoryScreen::newFrame(int duration,
		int headerColor,
		int headerShadowColor,
		int textColor,
		int textShadowColor,
		int backColor1,
		int backColor2) {
		Dialogue d;
		d.duration = duration;
		d.headerColor = headerColor;
		d.headerShadow = headerShadowColor;
		d.textColor = textColor;
		d.textShadow = textShadowColor;
		d.backColor1 = backColor1;
		d.backColor2 = backColor2 < 0 ? backColor1 : backColor2;
		dialogue.push_back(d);
	}


	void StoryScreen::frameImage(int image, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 scale, glm::vec2 rot) {
		Dialogue& d = dialogue.back();
		d.image = image;
		d.position1 = pos1 * (float)ptsize;
		d.position2 = pos2 * (float)ptsize;
		d.scale = scale;
		d.rotation = rot;
	}


	void StoryScreen::frameImageOps(glm::vec2 fadeRange, glm::vec2 scaleLfo, glm::vec2 rotLfo, glm::vec4 lfo) {
		Dialogue& d = dialogue.back();
		d.fadeRange = fadeRange;
		d.scaleLfo = scaleLfo;
		d.rotLfo = rotLfo;
		d.lfoAmplitude = lfo.x;
		d.lfoFrequency = lfo.y; // * 3.141592653f / 180.0f;
		d.lfoPhase = lfo.z * 3.141592653f / 180.0f;
		d.lfoRamp = lfo.w;
	}


	void StoryScreen::frameHeader(int font, const std::string& text) {
		Dialogue& d = dialogue.back();
		d.headerText = text;
		d.headerFont = clamp(font, 0, MAX_FONTS - 1);
	}


	void StoryScreen::frameLine(int font, const std::string& line) {
		Dialogue& d = dialogue.back();
		d.textFont = clamp(font, 0, MAX_FONTS - 1);
		d.lines.push_back(line);
	}


	void StoryScreen::_advanceFrame(int frame) {
		curframe = frame;
		if (curframe >= dialogue.size()) {
			donePlaying = true;
			HFLOGDEBUG("no more frames");
			return;
		}
		ticksLeft = dialogue[curframe].duration;
		lastCharsDrawn_ = 0;
		Dialogue& d = dialogue[curframe];
		HFLOGDEBUG("Story frame %d for %d ticks", frame, ticksLeft);
		HFLOGDEBUG("Story frame # %s", d.headerText.c_str());
		_reflowText(d);
	}


	void StoryScreen::_updateFrame() {}


	void StoryScreen::_drawFrame() {
		// determine frame and fade frame
		int prevframe = curframe - 1;
		if (prevframe < 0)
			prevframe = 0;
		int nextframe = curframe + 1;
		if (nextframe >= dialogue.size())
			nextframe = curframe;
		Dialogue& frame = dialogue[curframe];
		Dialogue& fadeFrame = dialogue[framePct < 0.5f ? prevframe : nextframe];

		// determine curves

		auto ucurve = [](float t, float tmin, float tmax, int flags) {
			float pct = clamp(t, 0.0f, 1.0f);
			t = 1.0f;
			if (flags & 1 && pct < tmin)
				t = pct / tmin;
			if (flags & 2 && pct > tmax)
				t = 1.0f - (pct - tmax) / (1.0f - tmax);
			t = clamp(t, 0.0f, 1.0f);
			if (flags & 4)
				return 1.0f - t;
			return t;
		};


		auto osc = [](float t, float A, float omega, float phi, int flags) {
			float lfo = A * std::cos((t * 3.1415926f / 180.0f) * omega + phi);
			// positive 0 to 1?
			if (flags & 1)
				lfo = 0.5f * lfo + 0.5f;
			// inverse (1 - lfo)
			if (flags & 2)
				lfo = 1.0f - lfo;
			return lfo;
		};

		auto calcLfo = [](float x, float lfo, glm::vec2 lfoParams) {
			float mix = glm::mix(lfoParams.x, lfoParams.y, lfo);
			return x + mix;
		};

		// frameCurve ramps up to 1, then back down
		float frameCurve = 1.0f;
		if (framePct < 0.1f)
			frameCurve = framePct * 10.0f;
		if (framePct > 0.9f)
			frameCurve = 1.0f - (framePct - 0.9f) * 10.0f;
		frameCurve = clamp(frameCurve, 0.0f, 1.0f);

		// fadeCurve is 0, then ramps up to 1
		float fadeCurve = framePct > 0.9f ? 1.0f - frameCurve : 0.0f;
		// textFade is 1, then ramps down to 0
		float inverseFadeCurve = 1.0f - fadeCurve;
		// imageCurve is
		float imageCurve = ucurve(framePct, 0.3f, 0.7f, 3);

		// determine colors based on curves

		SDL_Color bcolor1 = MakeColor(frame.backColor1);
		SDL_Color bcolor2 = MakeColor(frame.backColor2);
		SDL_Color color1 = mix(bcolor1, bcolor2, framePct);
		SDL_Color color2 = MakeColor(fadeFrame.backColor1);
		SDL_Color backColor = mix(color1, color2, fadeCurve);
		SDL_Color headerFG = mix(backColor, MakeColor(frame.headerColor), frameCurve);
		SDL_Color headerBG = mix(backColor, MakeColor(frame.headerShadow), frameCurve);
		SDL_Color textFG = mix(backColor, MakeColor(frame.textColor), inverseFadeCurve);
		SDL_Color textBG = mix(backColor, MakeColor(frame.textShadow), inverseFadeCurve);
		context->clearScreen(backColor);

		// image
		if (frame.image >= 0) {
			IMAGEINFO& img = images[frame.image];
			glm::vec2 center = { screenWidth * 0.5f, screenHeight * 0.5f };
			glm::vec2 size2 = img.size * 0.5f;
			glm::vec2 position = glm::mix(frame.position1, frame.position2, framePct);

			float lfoCurve = ucurve(tickCount * MS_PER_UPDATE, frame.lfoRamp / frame.duration, 1.0f, 1);
			float lfo = osc(tickCount, lfoCurve * frame.lfoAmplitude, frame.lfoFrequency, frame.lfoPhase, 1);
			float angle = glm::mix<float>(frame.rotation.x, frame.rotation.y, framePct);
			float scale = glm::mix<float>(frame.scale.x, frame.scale.y, framePct);
			scale = scale * calcLfo(1.0f, lfo, frame.scaleLfo);
			angle = calcLfo(angle, lfo, frame.rotLfo);

			glm::vec2 location = position + center - size2 * scale;
			SDL_Rect dstrect{ (int)location.x, (int)location.y, (int)(img.size.x * scale), (int)(img.size.y * scale) };
			if (img.texture) {
				SDL_SetTextureBlendMode(img.texture, SDL_BLENDMODE_BLEND);
				SDL_SetTextureAlphaMod(img.texture, (Uint8)(255 * imageCurve));
				SDL_RenderCopyEx(context->renderer(),
					img.texture,
					nullptr,
					&dstrect,
					angle,
					nullptr,
					SDL_RendererFlip::SDL_FLIP_NONE);
				SDL_SetTextureAlphaMod(img.texture, 255);
			}
		}

		// header text
		if (!frame.headerText.empty()) {
			FONTINFO& f = fonts[frame.headerFont];
			int w = f.calcWidth(frame.headerText);
			TEXTRECT tr;
			tr.reset(0, 0, context->screenWidth, context->screenHeight >> 1, 10);
			tr.calc(f.halign, f.valign, w, f.h);
			f.draw(tr.x, tr.y, dialogue[curframe].headerText, headerFG, headerBG);
		}

		// story text
		if (framePct > 0.1f) {
			int adjTickCount = (int)(tickCount - frame.duration * 0.1f);
			size_t charsDrawn = 0;
			size_t maxChars = (size_t)(adjTickCount / TICKS_PER_CHAR);

			FONTINFO& f = fonts[frame.textFont];
			TEXTRECT tr;
			tr.reset(context->screenWidth >> 2,
				context->screenHeight >> 1,
				context->screenWidth >> 1,
				context->screenHeight >> 1,
				10);
			tr.calcy(f.valign, (int)reflowLines.size() * f.h);

			std::string substr;
			substr.reserve(200);
			for (const auto r : reflowLines) {
				tr.calcx(f.halign, r.width);
				if (r.line.empty()) {
					tr.y += f.h;
					continue;
				}
				size_t possibleChars = charsDrawn + r.line.size();
				if (possibleChars < maxChars) {
					f.draw(tr.x, tr.y, r.line, textFG, textBG);
					charsDrawn += r.line.size();
				} else if (maxChars > charsDrawn) {
					substr = r.line.substr(0, maxChars - charsDrawn);
					f.draw(tr.x, tr.y, substr, textFG, textBG);
					charsDrawn += substr.size();
				}
				tr.y += f.h;
			}

			// play sound effects

			if (lastCharsDrawn_ != charsDrawn) {
				lastCharsDrawn_ = charsDrawn;
				context->playAudioClip(blipSoundId_);
			}
		}
	} // namespace GameLib


	void StoryScreen::_reflowText(Dialogue& d) {
		FONTINFO& f = fonts[d.textFont];
		int fh = f.h;
		reflow.clear();
		reflowLines.clear();
		int maxWidth = context->screenWidth >> 1;
		int x = 0;
		reflowLines.push_back({ 0, f.h, 0, 0 });
		int first = 0;
		int count = 0;
		std::string totalLine;
		totalLine.reserve(200);
		for (auto& line : d.lines) {
			std::istringstream is(line);
			while (is) {
				std::string s;
				is >> s;
				if (s.empty())
					continue;
				int tw = f.calcWidth(s);
				int w = f.spacew + tw;
				if (x + w > maxWidth) {
					reflowLines.back() = { x, f.h, first, count, totalLine };
					reflowLines.push_back({ 0, f.h, 0, 0 });
					first += count;
					count = 0;
					totalLine.clear();
					totalLine.reserve(200);

					x = 0;
				}
				reflow.push_back({ tw, s });
				totalLine.append(" ");
				totalLine.append(s);
				x += w;
				count++;
			}
			if (reflowLines.back().width != 0)
				reflowLines.push_back({ 0, f.h, 0, 0 });
		}
		if (reflowLines.back().width == 0)
			reflowLines.back() = { x, f.h, first, count, totalLine };
	}


	bool StoryScreen::readStream(std::istream& is) {
		int actorCount;
		is >> actorCount;
		actors.clear();

		for (int i = 0; i < actorCount; i++) {
			std::string actorName;
			std::getline(is, actorName);
			actors[actorName] = makeActor(actorName,
				std::make_shared<InputComponent>(),
				std::make_shared<ActorComponent>(),
				std::make_shared<PhysicsComponent>(),
				std::make_shared<GraphicsComponent>());
			auto actor = actors[actorName];

			// get name of image (a PNG or JPG)
			std::getline(is, actor->imageName);
			if (!context->loadImage(actor->imageName)) {
				HFLOGERROR("Can't load image '%s'", actor->imageName.c_str());
				exit(0);
			}

			std::string line;
			std::getline(is, line);
			std::istringstream istr(line);
			istr >> actor->size.x;
			istr >> actor->size.y;
			actor->size *= ptsize;
		}

		dialogue.clear();
		int duration = 0;
		while (is) {
			int duration;
			is >> duration;
			if (duration < 0)
				break;
			Dialogue d;
			d.duration = duration;
			std::getline(is, d.actorName);

			int size;
			is >> size;
			d.lines.resize(size);
			for (std::string& line : d.lines) {
				std::getline(is, line);
			}
			dialogue.push_back(d);
		}
		return is.eof();
	}


	bool StoryScreen::writeStream(std::ostream& os) {
		os << actors.size() << "\n";
		for (auto& [k, actor] : actors) {
			os << actor->name() << "\n";
			os << actor->imageName << "\n";
			os << std::setprecision(4) << actor->size.x << "\n";
			os << std::setprecision(4) << actor->size.y << "\n";
		}
		for (auto d : dialogue) {
			os << d.duration << "\n";
			os << d.actorName << "\n";
			for (auto line : d.lines) {
				os << line << "\n";
			}
		}
		os << "-1\n";
		return true;
	}
} // namespace GameLib
