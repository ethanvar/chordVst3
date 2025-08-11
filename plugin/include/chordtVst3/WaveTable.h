#include "PluginProcessor.h"

class AudioWaveTable  {
    public:
        AudioWaveTable();
        ~AudioWaveTable();
        void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds, const juce::Rectangle<int>& timeMeasureBounds, const juce::Rectangle<int>& liveSpectogramPlaceholder, const juce::Rectangle<int>& spectogramPlaceholder);
        void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds, const juce::Rectangle<int>& timeMeasureBounds, const juce::Rectangle<int>& liveSpectogramPlaceholder, const juce::Rectangle<int>& spectogramPlaceholder, AudioPluginAudioProcessor& processorRef);
        void paintTimer (juce::Graphics& g, const juce::Rectangle<int>& timeMeasureBounds, AudioPluginAudioProcessor& processorRef);
    private:
        juce::Image myimage;
};