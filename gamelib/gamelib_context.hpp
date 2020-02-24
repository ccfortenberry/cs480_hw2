#ifndef GAMELIB_CONTEXT_HPP
#define GAMELIB_CONTEXT_HPP

#include <gamelib_base.hpp>

namespace GameLib {
    constexpr int WindowDefault = 0;
    constexpr int WindowFullscreen = SDL_WINDOW_FULLSCREEN_DESKTOP;
    constexpr int WindowBorderless = SDL_WINDOW_BORDERLESS;
    constexpr int WindowResizeable = SDL_WINDOW_RESIZABLE;
    constexpr int WindowOpenGL = SDL_WINDOW_OPENGL;

    static constexpr int LIBXOR_TILESET32 = -1;

    class Context {
    public:
        Context(int width, int height, int flags = WindowResizeable);
        ~Context();

        //////////////////////////////////////////////////////////////
        // ERROR HANDLING ////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        operator bool() const { return initialized_; }
        bool hadError() const;
        const std::string errorString() const { return errorString_; }
        bool audioInitialized() const { return audioInitialized_; }

        //////////////////////////////////////////////////////////////
        // TIMING ////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        static float currentTime_ms;
        static float currentTime_s;
        static float deltaTime;

        //////////////////////////////////////////////////////////////
        // SEARCH PATHS //////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        // add a search path for loading files
        void addSearchPath(const std::string& path);

        // clear all search paths for loading files
        void clearSearchPaths();

        // find a valid path using the configured search paths
        // if file is not a regular file, returns an empty string
        std::string findSearchPath(const std::string& filename) const;

        //////////////////////////////////////////////////////////////
        // DRAWING AND IMAGES ////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        // clear the screen to a color
        void clearScreen(SDL_Color color);

        // swap the back buffer to the front
        void swapBuffers();

        // load the filename from the current directory, or the search paths
        SDL_Texture* loadImage(const std::string& filename);

        // frees all currently loaded images
        void freeImages();

        // returns true if the resource name is loaded
        bool imageLoaded(const std::string& resourceName) const;

        // returns a pointer to the SDL_Texture, or nullptr if it does not exist
        SDL_Texture* getImage(const std::string& resourceName) const;

        // load a tileset with a given tilesetId, width, and height
        int loadTileset(int tilesetId, int w, int h, const std::string& filename);

        // frees all currently loaded tilesets
        void freeTilesets();

        // returns a pointer to the TILEIMAGE, or nullptr if it does not exist
        TILEIMAGE* getTile(int tilesetId, int tileId);

        // returns a pointer to the SDL_Texture with no error checking
        TILEIMAGE* getTileFast(int tilesetId, int tileId) { return &tilesets_[tilesetId][tileId]; }

        // returns number of tiles in a tileset
        int getTileCount(int tilesetId) { return (int)tilesets_.at(tilesetId).size(); }

        // draws a rectangle to the screen. returns 0 if success, -1 if error
        int drawTexture(glm::vec2 position, glm::vec2 size, SDL_Texture* texture);

        // draws a rectangle to the screen. returns 0 if success, -1 if error
        int drawTexture(glm::vec2 position, int tilesetId, int tileId);

        // draws a rotated, centerable, flipable rectangle to the screen. returns 0 if success, -1 if error
        int drawTexture(int tilesetId, int tileId, SPRITEINFO& spriteInfo);

        //////////////////////////////////////////////////////////////
        // AUDIO CODE ////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        // initializes a clip id for use, it frees any audio clip used, nullptr if it doesn't exist
        AUDIOINFO* initAudioClip(int clipId);
        // returns a pointer to the AUDIOINFO struct with the loaded WAV data, nullptr if it doesn't exist
        AUDIOINFO* loadAudioClip(int clipId, const std::string& filename);
        // returns a pointer to the AUDIOINFO struct used for playing this clip, nullptr if it doesn't exist
        AUDIOINFO* getAudioClip(int clipId);
        // frees all audio clips
        void freeAudioClips();
        // returns the number of audio clips currently allocated
        int getAudioClipCount() const { return (int)audioClips_.size(); }
        // play an audio clip on a channel (-1 if any free channel)
        int playAudioClip(int clipId, int channel = -1);
        // stop an audio channel from playing
        void stopAudioChannel(int channel);
        // set the volume for a specific channel in the range 0 to 1
        void setChannelVolume(int channel, float volume);
        // get the volume for a specific channel in the range 0 to 1
        float getChannelVolume(int channel);

        //////////////////////////////////////////////////////////////
        // MUSIC CODE ////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        MUSICINFO* initMusicClip(int musicId);
        MUSICINFO* loadMusicClip(int musicId, const std::string& filename);
        MUSICINFO* getMusicClip(int musicId);
        void freeMusicClips();
        int getMusicClipCount() const { return (int)musicClips_.size(); }
        bool playMusicClip(int musicId, int loops = 0, int fadems = 0);
        void stopMusicClip();

        //////////////////////////////////////////////////////////////
        // EVENT HANDLING CODE ///////////////////////////////////////
        //////////////////////////////////////////////////////////////

        // handle all SDL events
        int getEvents();

        // set quitRequested to a nonzero value to indicate the game loop should end
        int quitRequested{ 0 };

        // keyboard is used to represent the current keys pressed
        struct KEYBOARDSTATE {
            std::vector<int> scancodes;
            std::map<int, int> keys;
            int mod{ 0 };
            bool checkClear(int scancode) {
                if (scancodes[scancode]) {
                    scancodes[scancode] = 0;
                    return true;
                }
                return false;
			}
        } keyboard;

        // MaxGameControllers reflects the XInput library max of four controllers
        static constexpr int MaxGameControllers{ 4 };

        // This is the number of gameControllers available
        unsigned gameControllersConnected{ 0 };

        // This is an array of game pad information. If controller pointer is not null, it is available
        struct GAMECONTROLLERSTATE {
            bool enabled{ false };
            SDL_GameController* controller{ nullptr };
            std::string name;
            glm::vec2 axis1{ 0.0f, 0.0f };
            glm::vec2 axis2{ 0.0f, 0.0f };
            float a;
            float b;
            float x;
            float y;
            float start;
            float back;
        } gameControllers[MaxGameControllers];

        int screenWidth{ 0 };
        int screenHeight{ 0 };
        SDL_Surface* windowSurface() { return windowSurface_; }
        SDL_Renderer* renderer() { return renderer_; }
    private:
        bool initialized_{ false };
        bool audioInitialized_{ false };
        mutable bool hadError_{ false };
        std::string errorString_;
        SDL_Window* window_{ nullptr };
        SDL_Renderer* renderer_{ nullptr };
        SDL_Surface* windowSurface_{ nullptr };
        SDL_AudioSpec audioSpec_;
        SDL_AudioDeviceID audioDeviceId_{ 0 };
        std::vector<std::string> searchPaths_;
        std::map<std::string, TILEIMAGE> images_;
        std::map<int, std::vector<TILEIMAGE>> tilesets_;
        std::map<int, AUDIOINFO> audioClips_;
        std::map<int, MUSICINFO> musicClips_;

        bool _init();
        bool _initSubsystems();
        bool _initScreen(int width, int height, int windowFlags);
        void _openGameControllers();
        void _closeGameControllers();
        void _updateGameControllers();
        void _kill();
        void _setError(std::string&& errorString);

        std::vector<TILEIMAGE>& _initTileset(int i);
        int _addTile(int tilesetId, SDL_Surface* surface);
    };
}

#endif
