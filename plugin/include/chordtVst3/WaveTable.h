#include "PluginProcessor.h"

class AudioWaveTable  {
    public:
        AudioWaveTable();
        ~AudioWaveTable();
        void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
        void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds, const juce::Rectangle<int>& timeMeasureBounds, AudioPluginAudioProcessor& processorRef);
        void paintTimer (juce::Graphics& g, const juce::Rectangle<int>& timeMeasureBounds, AudioPluginAudioProcessor& processorRef);
};