#include "../include/chordtVst3/PluginProcessor.h"
#include "../include/chordtVst3/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
                       , thumbnailCache (5), thumbnail (512, formatManager, thumbnailCache),forwardFFT (fftOrder),
      spectrogramImage (juce::Image::RGB, 384, 384, true), instantSpectogram(juce::Image::RGB, 384, 384, true), instantSpectogramTwo(juce::Image::RGB, 384, 384, true)
{
    formatManager.registerBasicFormats();
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return "ChordVst3";
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    // auto* editor = dynamic_cast<AudioPluginAudioProcessorEditor*>(getActiveEditor());
    // if (editor != nullptr)
    std::cout << "Preparing transport source with sample rate: " << sampleRate << " and block size: " << samplesPerBlock << std::endl;
    transportSource.prepareToPlay (samplesPerBlock, sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    // auto* editor = dynamic_cast<AudioPluginAudioProcessorEditor*>(getActiveEditor());
    // if (editor != nullptr)
    transportSource.releaseResources();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    juce::AudioSourceChannelInfo bufferToFill = juce::AudioSourceChannelInfo(buffer);
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    transportSource.getNextAudioBlock(bufferToFill);

    if (bufferToFill.buffer->getNumChannels() > 0) {
        auto* channelData = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);
        for (auto i = 0; i < bufferToFill.numSamples; ++i) {
            pushNextSampleIntoFifo (channelData[i]);
        }
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

void AudioPluginAudioProcessor::loadFile(const juce::File& audioFile)
{
    if (audioFile.existsAsFile()) {
        std::cout<< "Selected file: " << audioFile.getFullPathName() << std::endl;
        auto* reader = formatManager.createReaderFor (audioFile); // [10]
        
        if (reader != nullptr)
        {   
            processEntireSpectogram(reader);
            // processEntireSpectogramTwo(reader);
            std::cout << "Format created: " << reader->getFormatName() << std::endl;
            auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
            // playButton.setEnabled (true); // [13]
            // std::cout << transportSource.getLengthInSeconds() << std::endl;
            thumbnail.setSource (new juce::FileInputSource (audioFile));
            readerSource.reset (newSource.release());
        }
    }
}

void AudioPluginAudioProcessor::processEntireSpectogram(juce::AudioFormatReader *reader) {
    // use tranposrtSource to read ahead of live processBlock and process spectogram in advance
    // std::cout << "total sample length: " << reader->lengthInSamples << std::endl;
    std::cout << "bit shifting: " << (1 << fftOrder) << std::endl;

    juce::AudioBuffer<float> fullBuffer((int)reader->numChannels, (int)reader->lengthInSamples);
    // juce::AudioSourceChannelInfo bufferToFill = juce::AudioSourceChannelInfo(fullBuffer);

    std::array<float, fftSize> firstOut;
    std::array<float, fftSize * 2> fourierData;

    int sliceWidth = juce::jlimit(1, spectrogramImage.getWidth(), 3);
    std::cout << "Print slice Width: " << sliceWidth << std::endl;

    if (!reader->read(&fullBuffer, 0,(int)reader->lengthInSamples, 0, true, true)) {
        std::cout << "Failed to read audio file" << std::endl;
        return;
    }

    int index = 0;

    const int leftoverSampleNumbers = fullBuffer.getNumSamples() % fftSize;

    for (int i = 0; i <= fullBuffer.getNumSamples(); i++) {
        firstOut[(size_t) index++] = fullBuffer.getSample(0, i);
        
        if ((i%fftSize) == 0) {
            std::fill (fourierData.begin(), fourierData.end(), 0.0f);
            std::copy (firstOut.begin(), firstOut.end(), fourierData.begin());
            drawNextLineOfSpectrogram(instantSpectogram, fourierData);
            index = 0;
        }
    }
}


void AudioPluginAudioProcessor::pushNextSampleIntoFifo(float sample) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next line should now be rendered..
    if (fifoIndex == fftSize) // [8]
    {
        if (!nextFFTBlockReady) // [9]
        {
            std::fill (fftData.begin(), fftData.end(), 0.0f);
            std::copy (fifo.begin(), fifo.end(), fftData.begin());
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[(size_t) fifoIndex++] = sample;
}

void AudioPluginAudioProcessor::drawingWrapper() {
    drawNextLineOfSpectrogram(spectrogramImage, fftData, 3);
}

void AudioPluginAudioProcessor::drawNextLineOfSpectrogram(juce::Image &specImage, 
                                                           std::array<float, fftSize * 2> &fourierData, int sliceWidth)  {
    //Add spectogram,  parameter 
    auto w = specImage.getWidth();
    sliceWidth = juce::jlimit(1, w, sliceWidth);
    const int drawX = w - sliceWidth;


    auto imageHeight = specImage.getHeight();

    spectrogramImage.moveImageSection(0, 0, sliceWidth, 0, w - sliceWidth, imageHeight);

    juce::dsp::WindowingFunction<float> window(fftSize, juce::dsp::WindowingFunction<float>::hann, true);
    window.multiplyWithWindowingTable(fourierData.data(), fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform (fourierData.data());

    auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fourierData.data(), fftSize / 2);
    juce::Image::BitmapData bitmap { specImage, drawX, 0, sliceWidth, imageHeight, juce::Image::BitmapData::writeOnly };
    for (auto y = 1; y < imageHeight; ++y) {
        auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.9f);
        auto fftDataIndex = (size_t) juce::jlimit (0, fftSize / 2, (int) (skewedProportionY * fftSize / 2));
        auto level = juce::jmap (fourierData[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
        if (sliceWidth > 1) {
            for (int x = 0; x < sliceWidth; ++x)
                bitmap.setPixelColour(x, y, mapFFTValueToColour(level));
        } else {
            bitmap.setPixelColour(0, y, mapFFTValueToColour(level));
        }
    }
}

juce::Colour AudioPluginAudioProcessor::mapFFTValueToColour(float& value) {
    float scaledValue = std::max(0.0f, std::min(1.0f, value));
    float hue = (scaledValue * 360.0f);
    return juce::Colour::fromHSV(hue, 1.0f, value, 1.0f);
}

juce::Image& AudioPluginAudioProcessor::getSpectrogram() {
    return spectrogramImage;
}

juce::Image& AudioPluginAudioProcessor::getInstantSpectogram() {
    return instantSpectogram;
}

juce::Image& AudioPluginAudioProcessor::getInstantSpectogramTwo() {
    return instantSpectogramTwo;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}


