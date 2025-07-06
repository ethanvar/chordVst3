#include "../include/chordtVst3/SongTimer.h"

SongTimer::SongTimer(juce::Graphics& g, const juce::Rectangle<int>& timeMeasureBounds, AudioPluginAudioProcessor& processorRef) {
    g.setColour (juce::Colours::purple);
    g.fillRect (timeMeasureBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText(std::to_string(processorRef.transportSource.getCurrentPosition()), timeMeasureBounds, juce::Justification::centred, 1);
}

SongTimer::~SongTimer() {

}

