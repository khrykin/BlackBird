/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Section.h"
#include "EditorHeader.h"

class BlackFaceAudioProcessorEditor : public AudioProcessorEditor {
public:
    explicit BlackFaceAudioProcessorEditor(BlackFaceAudioProcessor &, AudioProcessorValueTreeState &);
    ~BlackFaceAudioProcessorEditor() override;

    void paint(Graphics &) override;
    void resized() override;
private:
    using SliderAttachment =  AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
    friend EditorHeader;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BlackFaceAudioProcessor &processor;

    AudioProcessorValueTreeState &valueTreeState;

    Section oscillatorSection{"Oscillator"};
    Section filterSection{"Filter", 2};
    Section envelopeSection{"Envelope", 4};
    Section masterSection{"Master"};

    EditorHeader header{*this};

    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;

    static constexpr auto padding = 14.0f;
    static constexpr auto headerHeight = 38.0f;

    Knob *addParameterAsKnobToSection(Section &section, const String &parameterID, const String &title);
    void addKnobToSection(Section &section, Knob *knob, const String &parameterID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlackFaceAudioProcessorEditor)
    void setupControls();
};
