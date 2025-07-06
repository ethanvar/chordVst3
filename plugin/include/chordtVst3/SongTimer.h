#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"


class SongTimer {
    public:
        SongTimer(juce::Graphics& g, const juce::Rectangle<int>& timeMeasureBounds, AudioPluginAudioProcessor& processorRef);
        ~SongTimer();
        juce::Rectangle<int> thumbnailBounds;
};