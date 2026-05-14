/*
  ==============================================================================

    SynthVoice.h
    A single polyphonic voice with selectable oscillator and ADSR envelope.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    //==========================================================================
    // Called once by the Processor after construction to give the voice
    // lock-free, real-time-safe access to the APVTS parameter values.
    void setParameterPointers (std::atomic<float>* oscTypeParam,
                               std::atomic<float>* attackParam,
                               std::atomic<float>* releaseParam)
    {
        oscTypePtr  = oscTypeParam;
        attackPtr   = attackParam;
        releasePtr  = releaseParam;
    }

    //==========================================================================
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* /*sound*/,
                    int /*currentPitchWheelPosition*/) override
    {
        noteFrequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        level = velocity;
        phase = 0.0;

        updateAdsrParameters();
        adsr.noteOn();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            adsr.reset();
            clearCurrentNote();
        }
    }

    void pitchWheelMoved (int /*newPitchWheelValue*/) override
    {
        // TODO: implement pitch bend
    }

    void controllerMoved (int /*controllerNumber*/, int /*newControllerValue*/) override
    {
        // TODO: handle MIDI CC messages
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample, int numSamples) override
    {
        if (! isVoiceActive())
            return;

        updateAdsrParameters();

        const int oscType = (oscTypePtr != nullptr)
                          ? static_cast<int> (oscTypePtr->load())
                          : 0;

        const double sampleRate = getSampleRate();
        const double phaseIncrement = noteFrequency / sampleRate;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float oscillatorSample = generateOscillatorSample (oscType);
            const float envelopeValue    = adsr.getNextSample();
            const float outputSample     = oscillatorSample * envelopeValue * level;

            // Advance phase and wrap to [0, 1)
            phase += phaseIncrement;
            if (phase >= 1.0)
                phase -= 1.0;

            // Mix into all output channels
            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                outputBuffer.addSample (channel, startSample + sample, outputSample);

            // Voice finished its release — free it
            if (! adsr.isActive())
            {
                clearCurrentNote();
                break;
            }
        }
    }

private:
    //==========================================================================
    // Raw oscillator waveform generators (phase is normalised to [0, 1))
    float generateOscillatorSample (int oscType) const
    {
        switch (oscType)
        {
            case 0:  // Sine
                return static_cast<float> (std::sin (phase * juce::MathConstants<double>::twoPi));

            case 1:  // Square
                return (phase < 0.5) ? 1.0f : -1.0f;

            case 2:  // Saw (rising, band-limited-ish via naive)
                return static_cast<float> (2.0 * phase - 1.0);

            default:
                return 0.0f;
        }
    }

    // Reads the atomic APVTS values and pushes them into the juce::ADSR.
    // Safe to call on the audio thread every block.
    void updateAdsrParameters()
    {
        const float attack  = (attackPtr  != nullptr) ? attackPtr->load()  : 0.1f;
        const float release = (releasePtr != nullptr) ? releasePtr->load() : 0.1f;

        juce::ADSR::Parameters params;
        params.attack  = attack;
        params.decay   = 0.0f;   // no decay for now — full sustain
        params.sustain = 1.0f;
        params.release = release;

        adsr.setParameters (params);
    }

    //==========================================================================
    // Oscillator state
    double phase         = 0.0;
    double noteFrequency = 0.0;
    float  level         = 0.0f;

    // Envelope
    juce::ADSR adsr;

    // APVTS parameter pointers (non-owning, lifetime managed by APVTS)
    std::atomic<float>* oscTypePtr = nullptr;
    std::atomic<float>* attackPtr  = nullptr;
    std::atomic<float>* releasePtr = nullptr;
};
