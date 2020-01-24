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
    explicit Section(const String &title, unsigned int size = 1)
            : _size(size) {
        setName(title);
    };

    void addKnob(Knob *knob) {
        addAndMakeVisible(knobs.add(knob));
        resized();
    }

    void paint(Graphics &g) override {
        g.setColour(Colour(220, 220, 220));

        auto font = getDefaultBoldFont();
        font.setHeight(fontHeight);
        font.setExtraKerningFactor(0.1);

        g.setFont(font);

        auto rect = getLocalBounds();

        g.drawFittedText(getName().toUpperCase(),
                         0,
                         0,
                         rect.getWidth(),
                         titleAreaHeight,
                         Justification::centredTop, 1);

        g.setColour(Colour(21, 22, 22));

        g.fillRoundedRectangle(0.5 * borderThickness,
                               titleAreaHeight,
                               rect.getWidth() - borderThickness,
                               rect.getHeight() - titleAreaHeight - 0.5 * borderThickness,
                               borderRadius);

        g.setColour(Colour(220, 220, 220));

        g.drawRoundedRectangle(0.5 * borderThickness,
                               titleAreaHeight,
                               rect.getWidth() - borderThickness,
                               rect.getHeight() - titleAreaHeight - 0.5 * borderThickness,
                               borderRadius,
                               borderThickness);


    };

    Rectangle<float> availableArea() {
        auto rect = getLocalBounds();
        return {0.5f * borderThickness,
                titleAreaHeight,
                rect.getWidth() - borderThickness,
                rect.getHeight() - titleAreaHeight - 0.5f * borderThickness};
    }

    void resized() override {
        FlexBox fb;
        fb.flexWrap = FlexBox::Wrap::wrap;
        fb.justifyContent = FlexBox::JustifyContent::center;
        fb.alignContent = FlexBox::AlignContent::center;

        for (auto *knob : knobs)
            fb.items.add(FlexItem(*knob)
                                 .withMinWidth(knob->minWidth())
                                 .withMinHeight(knob->minHeight()));

        fb.performLayout(availableArea());
    }

    float recommendedHeight() {
        return 2 * Knob::defaultSize + borderThickness + titleAreaHeight;
    }

    float recommendedWidth() {
        return _size * Knob::defaultSize + 2 * borderThickness;
    }

private:
    OwnedArray<Knob> knobs;

    unsigned int _size = 1;

    static constexpr auto fontHeight = 16.0f;
    static constexpr auto titleAreaHeight = 1.5f * fontHeight;
    static constexpr auto borderRadius = 5.0f;
    static constexpr auto borderThickness = 2.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Section)
};
