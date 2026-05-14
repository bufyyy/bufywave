/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyDigitalSynthAudioProcessorEditor::MyDigitalSynthAudioProcessorEditor (MyDigitalSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // --- Oscillator ComboBox ---
    oscSelector.addItemList (juce::StringArray { "Sine", "Square", "Saw" }, 1);
    oscSelector.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (oscSelector);

    oscLabel.setText ("Oscillator", juce::dontSendNotification);
    oscLabel.setJustificationType (juce::Justification::centred);
    oscLabel.attachToComponent (&oscSelector, false);
    addAndMakeVisible (oscLabel);

    // --- Attack Slider ---
    attackSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    attackSlider.setTextValueSuffix (" s");
    addAndMakeVisible (attackSlider);

    attackLabel.setText ("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType (juce::Justification::centred);
    attackLabel.attachToComponent (&attackSlider, false);
    addAndMakeVisible (attackLabel);

    // --- Release Slider ---
    releaseSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    releaseSlider.setTextValueSuffix (" s");
    addAndMakeVisible (releaseSlider);

    releaseLabel.setText ("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType (juce::Justification::centred);
    releaseLabel.attachToComponent (&releaseSlider, false);
    addAndMakeVisible (releaseLabel);

    // --- APVTS Attachments (must be created AFTER components are fully set up) ---
    oscAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                            audioProcessor.apvts, "oscType", oscSelector);
    attackAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                            audioProcessor.apvts, "attack", attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                            audioProcessor.apvts, "release", releaseSlider);

    setSize (500, 300);
}

MyDigitalSynthAudioProcessorEditor::~MyDigitalSynthAudioProcessorEditor()
{
}

//==============================================================================
void MyDigitalSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dark gradient background
    g.fillAll (juce::Colour (0xff1a1a2e));

    auto bounds = getLocalBounds();

    // Title bar area
    auto titleArea = bounds.removeFromTop (40);
    g.setColour (juce::Colour (0xff16213e));
    g.fillRect (titleArea);

    g.setColour (juce::Colour (0xffe2e2e2));
    g.setFont (juce::FontOptions (20.0f));
    g.drawText ("MyDigitalSynth", titleArea, juce::Justification::centred, true);

    // Subtle divider line
    g.setColour (juce::Colour (0xff0f3460));
    g.drawLine (0.0f, 40.0f, static_cast<float> (getWidth()), 40.0f, 2.0f);
}

void MyDigitalSynthAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (60);   // title bar + label clearance
    bounds.reduce (20, 10);      // padding

    // Top row: oscillator selector
    auto oscArea = bounds.removeFromTop (30);
    oscArea.reduce (100, 0);     // centre it
    oscSelector.setBounds (oscArea);

    bounds.removeFromTop (30);   // spacing + label clearance

    // Bottom row: two rotary knobs side-by-side
    auto knobArea = bounds.removeFromTop (140);
    auto halfWidth = knobArea.getWidth() / 2;

    attackSlider.setBounds  (knobArea.removeFromLeft (halfWidth).reduced (10, 0));
    releaseSlider.setBounds (knobArea.reduced (10, 0));
}
