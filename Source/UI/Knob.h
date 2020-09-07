/*
  ==============================================================================

    Knob.h
    Created: 14 Jan 2020 10:26:12pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "LookAndFeel.h"


#pragma mark - Knob

class Knob : public Component {
public:
    static constexpr auto defaultSize = 110.0f;

#pragma mark - Construction

    explicit Knob(const String &title, unsigned int width = 1);
    ~Knob() override;

#pragma mark - Acessing Private Instances

    Slider &slider();

#pragma mark - Geometry

    float minWidth();
    float minHeight();

    int widthFactor() const;
    void setWidthFactor(int width);

#pragma mark - Virtual Methods Overrides

    void paint(Graphics &g) override;
    void resized() override;

protected:
    static constexpr auto labelHeight = 20.0f;
    static constexpr auto fontHeight = 16.0f;

    Slider _slider;
    KnobLookAndFeel knobLookAndFeel;

    int _widthFactor = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Knob)
};

#pragma mark - OscillatorKnob

class OscillatorKnob : public Knob {
public:
    static constexpr auto sliderName = "OscillatorSlider";

    explicit OscillatorKnob(const String &title, unsigned int width = 1);

    void resized() override;
};

