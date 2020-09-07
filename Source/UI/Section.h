/*
  ==============================================================================

    Section.h
    Created: 14 Jan 2020 10:25:51pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Knob.h"

class Section : public Component {
public:

#pragma mark - Construction

    explicit Section(const String &title, unsigned int size = 1);

#pragma mark - Adding Knobs

    void addKnob(Knob *knob);

#pragma mark - Geometry

    Rectangle<float> availableArea();

    float recommendedHeight();
    float recommendedWidth();

#pragma mark - Virtual Methods Overrides

    void paint(Graphics &g) override;
    void resized() override;

private:
    OwnedArray<Knob> knobs;

    unsigned int _size = 1;

    static constexpr auto fontHeight = 16.0f;
    static constexpr auto titleAreaHeight = 1.5f * fontHeight;
    static constexpr auto borderRadius = 5.0f;
    static constexpr auto borderThickness = 2.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Section)
};
