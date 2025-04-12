#include "../include/chordtVst3/PluginProcessor.h"
#include "../include/chordtVst3/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // formatManager.registerBasicFormats();

    addAndMakeVisible (openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    
    addAndMakeVisible (playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled (false);
    
    addAndMakeVisible (stopButton);
    stopButton.setButtonText ("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled (false);
    
    processorRef.transportSource.addChangeListener (this);

    setSize (500, 300);
    setResizable (false, false);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    processorRef.transportSource.removeChangeListener(this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    playButton.setBounds(area.removeFromTop(40));
    stopButton.setBounds(area.removeFromTop(40));
    openButton.setBounds(area.removeFromTop(40));
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


void AudioPluginAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster* source) {
    if (source == &processorRef.transportSource) { // [2]
        if (processorRef.transportSource.isPlaying()) {
            changeState (Playing);
        }
        else {
            changeState (Stopped);
        }
    }
}

void AudioPluginAudioProcessorEditor::changeState (TransportState newState) {
    if (state != newState) {
        state = newState;
        switch (state) {
            case Stopped: // [3]
                stopButton.setEnabled (false);
                playButton.setEnabled (true);
                processorRef.transportSource.setPosition (0.0);
                break;
            case Starting: // [4]
                playButton.setEnabled (false);
                processorRef.transportSource.start();
                break;
            case Playing: // [5]
                stopButton.setEnabled (true);
                break;
            case Stopping: // [6]
                processorRef.transportSource.stop();
                break;
        }
    }
}

void AudioPluginAudioProcessorEditor::stopButtonClicked() {
    changeState (Stopping);
}

void AudioPluginAudioProcessorEditor::playButtonClicked() {
    std::cout << processorRef.transportSource.getLengthInSeconds() << std::endl; 
    changeState (Starting);
}

void AudioPluginAudioProcessorEditor::openButtonClicked()
{   
    std::cout << "Open button clicked!" << std::endl;

    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...", 
                                                    juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
                                                    "*.*");
    int chooseFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    chooser->launchAsync(chooseFlags, [this](const juce::FileChooser& fc) {   
        std::cout << "File chooser callback triggered." << std::endl;
        auto file = fc.getResult();
        processorRef.loadFile(file);
        playButton.setEnabled (true);
    });
}
