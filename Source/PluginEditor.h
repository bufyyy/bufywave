/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class MyDigitalSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MyDigitalSynthAudioProcessorEditor (MyDigitalSynthAudioProcessor&);
    ~MyDigitalSynthAudioProcessorEditor() override;

    //==========================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MyDigitalSynthAudioProcessor& audioProcessor;

    // UI Components
    juce::ComboBox oscSelector;
    juce::Slider   attackSlider;
    juce::Slider   releaseSlider;

    // Labels
    juce::Label oscLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;

    // APVTS Attachments (must be declared AFTER the components they attach to)
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oscAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyDigitalSynthAudioProcessorEditor)
};
