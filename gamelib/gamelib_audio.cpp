#include "pch.h"
#include <gamelib_audio.hpp>
#include <gamelib_locator.hpp>

namespace GameLib {
    void Audio::playAudio(int audioClipId, bool stopPrevious) {
        if (stopPrevious)
            stopAudio(audioClipId);
        Context* context = Locator::getContext();
        lastChannel_ = context->playAudioClip(audioClipId);
        audioClipToChannelMap_[audioClipId] = lastChannel_;
    };

    void Audio::stopAudio(int audioClipId) {
        Context* context = Locator::getContext();
        int channel = audioClipToChannelMap_[audioClipId];
        context->stopAudioChannel(channel);
    }

    void Audio::setVolume(float volume) {
        Context* context = Locator::getContext();
        context->setChannelVolume(lastChannel_, volume);
    }

    float Audio::getVolume() const {
        Context* context = Locator::getContext();
        return context->getChannelVolume(lastChannel_);
    }

    void Audio::playMusic(int musicClipId, int loops, float fadems) {
        Context* context = Locator::getContext();
        context->playMusicClip(musicClipId, loops, (int)fadems);
    }

    void Audio::stopMusic() {
        Context* context = Locator::getContext();
        context->stopMusicClip();
    }
}
