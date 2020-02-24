#ifndef GAMELIB_AUDIO_HPP
#define GAMELIB_AUDIO_HPP

#include <gamelib_context.hpp>

namespace GameLib {
    class IAudio {
    public:
        virtual ~IAudio() {}

        virtual void playAudio(int audioClipId, bool stopPrevious) {}
        virtual void stopAudio(int audioClipId) {}
        virtual void setVolume(float volume) {}
        virtual float getVolume() const { return 0.0f; }
        virtual void playMusic(int musicClipId, int loops, float fadems) {}
        virtual void stopMusic() {}
    };

    class Audio : public IAudio {
    public:
        void playAudio(int audioClipId, bool stopPrevious) override;
        void stopAudio(int audioClipId) override;
        void setVolume(float volume) override;
        float getVolume() const override;
        void playMusic(int musicClipId, int loops, float fadems) override;
        void stopMusic() override;
    private:
        int lastChannel_{ -1 };
        std::map<int, int> audioClipToChannelMap_;
    };
}

#endif
