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
    processorRef.thumbnail.addChangeListener (this);

    setSize (600, 360);
    setResizable (false, false);
    startTimer (40);
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

    juce::Rectangle<int> thumbnailBounds (10, 150, getWidth() - 20, getHeight() - 180);
    if (processorRef.thumbnail.getNumChannels() == 0) {
        paintIfNoFileLoaded (g, thumbnailBounds);
    } else {
        paintIfFileLoaded (g, thumbnailBounds);
    }
}

void AudioPluginAudioProcessorEditor::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds) {
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::paintIfFileLoaded (juce::Graphics & g, const juce::Rectangle<int>& thumbnailBounds) {
    g.setColour (juce::Colours::white);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::red);
    processorRef.thumbnail.drawChannels (g, 
        thumbnailBounds,
        0.0,
        processorRef.thumbnail.getTotalLength(),
        1.0
    );
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
        else if ((state == Stopping) || (state == Playing)){
            changeState (Stopped);
        }
        else if (state == Pausing) {
            changeState (Paused);
        }
    }
    if (source == &processorRef.thumbnail) {
        repaint();
    } 
}

void AudioPluginAudioProcessorEditor::thumbnailChanged () {
    repaint();
}

void AudioPluginAudioProcessorEditor::changeState (TransportState newState) {
    if (state != newState) {
        state = newState;
        switch (state) {
            case Stopped:
                playButton.setButtonText ("Play");
                stopButton.setButtonText ("Stop");
                // stopButton.setEnabled (false);
                playButton.setEnabled (true);
                processorRef.transportSource.setPosition (0.0);
                break;
            case Starting: // [4]
                processorRef.transportSource.start();
                break;
            case Playing: // [5]
                playButton.setButtonText ("Pause");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (true);
                break;
            case Pausing:
                processorRef.transportSource.stop();
                break;
            case Paused:
                playButton.setButtonText ("Resume");
                stopButton.setButtonText ("Set to 0:00");
                break;
            case Stopping:
                processorRef.transportSource.stop();
                break;
        }
    }
}

void AudioPluginAudioProcessorEditor::stopButtonClicked() {
    if (state == Paused) {
        changeState (Stopped);
    }
    else {
        changeState (Stopping);
    }
}

void AudioPluginAudioProcessorEditor::playButtonClicked() {
    std::cout << processorRef.transportSource.getLengthInSeconds() << std::endl;
    if ((state == Paused) || (state == Stopped)) {
        changeState (Starting);
    } else if (state == Playing) {
        changeState (Pausing);
    }
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

void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}
