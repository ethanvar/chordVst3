#pragma once

#include "PluginProcessor.h"
#include "WaveTable.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, public AudioWaveTable, public juce::ChangeListener, private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    enum TransportState {
        Stopped,
        Starting,
        Playing,
        Stopping,
        Pausing,
        Paused
    };

    void changeListenerCallback (juce::ChangeBroadcaster* source);
    void changeState (TransportState newState);
    void thumbnailChanged();
    void timerCallback();
    
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton openButton;
    juce::TextButton stopButton;
    juce::TextButton playButton;
    TransportState state;

    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
