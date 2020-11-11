/*
 ==============================================================================

 This file was auto-generated!

 It contains the basic framework code for a JUCE plugin editor.

 ==============================================================================
 */

#include "../PluginProcessor.h"
#include "PluginEditor.h"

#include <memory>

BlackBirdAudioProcessorEditor::BlackBirdAudioProcessorEditor(BlackBirdAudioProcessor &p,
                                                             AudioProcessorValueTreeState &valueTreeState)
        : AudioProcessorEditor(&p),
          processor(p),
          valueTreeState(valueTreeState) {
    setupControls();

    addAndMakeVisible(header);

    setSize(8.0f * masterSection.recommendedWidth() + 1.0f * padding + 3.0f * 0.5f * padding,
            masterSection.recommendedHeight() + headerHeight + 2.0f * padding);
}

void BlackBirdAudioProcessorEditor::setupControls() {
    using namespace DSPParametersConstants;

    addAndMakeVisible(oscillatorSection);

    auto oscillatorKnob = new OscillatorKnob("Type");
    addKnobToSection(oscillatorSection, oscillatorKnob, oscillatorWaveformParameterID);
    addParameterAsKnobToSection(oscillatorSection, characterParameterID, "Character");

    addAndMakeVisible(filterSection);
    addParameterAsKnobToSection(filterSection, filterCutoffParameterID, "Cutoff");
    addParameterAsKnobToSection(filterSection, filterResonanceParameterID, "Resonance");
    addParameterAsKnobToSection(filterSection, filterDriveParameterID, "Drive");

    addAndMakeVisible(envelopeSection);
    addParameterAsKnobToSection(envelopeSection, attackParameterID, "Attack");
    addParameterAsKnobToSection(envelopeSection, decayParameterID, "Decay");
    addParameterAsKnobToSection(envelopeSection, sustainParameterID, "Sustain");
    addParameterAsKnobToSection(envelopeSection, releaseParameterID, "Release");

    addParameterAsKnobToSection(envelopeSection, cutoffEnvelopeAmountParameterID, "C-off Amount");
    addParameterAsKnobToSection(envelopeSection, resonanceEnvelopeAmountParameterID, "Reso. Amount");
    addParameterAsKnobToSection(envelopeSection, velocityEnvelopeAmountParameterID, "Vel. Amount");

    addAndMakeVisible(masterSection);
    addParameterAsKnobToSection(masterSection, reverbParameterID, "Reverb");
    addParameterAsKnobToSection(masterSection, masterGainParameterID, "Gain");
}

Knob *BlackBirdAudioProcessorEditor::addParameterAsKnobToSection(Section &section,
                                                                 const String &parameterID,
                                                                 const String &title) {
    auto *knob = new Knob(title);
    addKnobToSection(section, knob, parameterID);

    return knob;
}

void BlackBirdAudioProcessorEditor::addKnobToSection(Section &section,
                                                     Knob *knob,
                                                     const String &parameterID) {
    sliderAttachments.emplace_back(std::make_unique<SliderAttachment>(valueTreeState,
                                                                      parameterID,
                                                                      knob->slider()));
    section.addKnob(knob);
}

BlackBirdAudioProcessorEditor::~BlackBirdAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void BlackBirdAudioProcessorEditor::paint(Graphics &g) {
    g.fillAll(Colour(13, 13, 13));

    auto font = getDefaultBoldFont();
    font.setHeight(26);
    font.setExtraKerningFactor(0.1);

    g.setFont(font);
    g.setColour(Colours::white);

    auto rect = getLocalBounds().toFloat();
    rect.setHeight(headerHeight + padding);
    rect.setX(padding + 5);

    g.drawText("BLACKBIRD", rect, Justification::centredLeft, false);

}

void BlackBirdAudioProcessorEditor::resized() {
    using Track = Grid::TrackInfo;

    Grid grid;
    grid.templateRows = {Track(1_fr)};

    auto items = {
            &oscillatorSection,
            &filterSection,
            &envelopeSection,
            &masterSection
    };

    std::for_each(std::begin(items),
                  std::end(items),
                  [&](Section *section) {
                      auto gridItem = GridItem(section);
                      gridItem.height = section->recommendedHeight();

                      grid.items.add(gridItem);
                      grid.templateColumns.add(Grid::Px{section->recommendedWidth()});
                  });

    grid.columnGap = Grid::Px{0.5f * padding};

    auto gridRect = getLocalBounds();
    gridRect.removeFromTop(headerHeight);
    gridRect.reduce(padding, padding);

    grid.performLayout(gridRect);

    auto headerRect = getLocalBounds();
    headerRect.setHeight(headerHeight + padding);
    headerRect.reduce(padding, padding);

    header.setBounds(headerRect);
}


