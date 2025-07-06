// #include "../include/chordtVst3/PluginEditor.h"
#include "../include/chordtVst3/WaveTable.h"

AudioWaveTable::AudioWaveTable () {

};

AudioWaveTable::~AudioWaveTable () {

};


void AudioWaveTable::paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds, const juce::Rectangle<int>& timeMeasureBounds, AudioPluginAudioProcessor& processorRef) {
    g.setColour (juce::Colours::white);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::red);
    processorRef.thumbnail.drawChannels (g, 
        thumbnailBounds,
        0.0,
        processorRef.thumbnail.getTotalLength(),
        1.0
    );
    paintTimer(g, timeMeasureBounds, processorRef);
};

void AudioWaveTable::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds) {
    
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
};

void AudioWaveTable::paintTimer (juce::Graphics& g, const juce::Rectangle<int>& timeMeasureBounds, AudioPluginAudioProcessor& processorRef) {
    g.setColour (juce::Colours::purple);
    g.fillRect (timeMeasureBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText(std::to_string(processorRef.transportSource.getCurrentPosition()), timeMeasureBounds, juce::Justification::centred, 1);
}