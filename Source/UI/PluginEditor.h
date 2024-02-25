/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../PluginProcessor.h"
#include "EditorHeader.h"
#include "LookAndFeel.h"
#include "Section.h"

class BlackBirdAudioProcessorEditor : public AudioProcessorEditor {
public:
  explicit BlackBirdAudioProcessorEditor(BlackBirdAudioProcessor &,
                                         AudioProcessorValueTreeState &);

  ~BlackBirdAudioProcessorEditor() override;

  void paint(Graphics &) override;
  void resized() override;

private:
  using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
  using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;

  friend EditorHeader;

  static constexpr auto padding = 14.0f;
  static constexpr auto headerHeight = 38.0f;

  BlackBirdAudioProcessor &processor;
  AudioProcessorValueTreeState &valueTreeState;

  Section oscillatorSection{"Oscillator"};
  Section filterSection{"Filter", 2};
  Section envelopeSection{"Envelope", 4};
  Section masterSection{"Master"};

  EditorHeader header{*this};

  std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;

  Knob *addParameterAsKnobToSection(Section &section, const String &parameterID,
                                    const String &title);
  void addKnobToSection(Section &section, Knob *knob,
                        const String &parameterID);

  void setupControls();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlackBirdAudioProcessorEditor)
};
