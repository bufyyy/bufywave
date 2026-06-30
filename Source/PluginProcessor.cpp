/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BufywaveAudioProcessor::BufywaveAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    for (int i = 0; i < numVoices; ++i)
        synth.addVoice (new SynthVoice());

    synth.addSound (new SynthSound());

    // Give each voice lock-free access to the APVTS parameter values
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
        {
            voice->setParameterPointers (
                apvts.getRawParameterValue ("oscType"),
                apvts.getRawParameterValue ("attack"),
                apvts.getRawParameterValue ("release"));
        }
    }
}

BufywaveAudioProcessor::~BufywaveAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
BufywaveAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "oscType",
        "Oscillator Type",
        juce::StringArray { "Sine", "Square", "Saw" },
        0));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "attack",
        "Attack",
        juce::NormalisableRange<float> (0.01f, 2.0f, 0.01f),
        0.1f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "release",
        "Release",
        juce::NormalisableRange<float> (0.01f, 3.0f, 0.01f),
        0.1f));

    return layout;
}

//==============================================================================
const juce::String BufywaveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BufywaveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BufywaveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BufywaveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BufywaveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BufywaveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BufywaveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BufywaveAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BufywaveAudioProcessor::getProgramName (int index)
{
    return {};
}

void BufywaveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BufywaveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void BufywaveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BufywaveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void BufywaveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // Push to oscilloscope FIFO
    if (buffer.getNumChannels() > 0)
    {
        auto* channelData = buffer.getReadPointer (0); // Left channel

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            scopeFifo.write (1).forEach ([&] (int destIndex)
            {
                scopeData[(size_t)destIndex] = channelData[i];
            });
        }
    }
}

//==============================================================================
bool BufywaveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BufywaveAudioProcessor::createEditor()
{
    return new BufywaveAudioProcessorEditor (*this);
}

//==============================================================================
void BufywaveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void BufywaveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BufywaveAudioProcessor();
}
