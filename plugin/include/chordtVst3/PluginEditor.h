#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, public juce::ChangeListener, private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);

    // juce::AudioFormatManager formatManager;
    // std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    // juce::AudioTransportSource transportSource;


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
    void timerCallback() override;
    // void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    // void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    
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
