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


class Knob : public Component {
public:
    static constexpr auto defaultSize = 110.0f;
    KnobLookAndFeel knobLookAndFeel;

    explicit Knob(const String &title, unsigned int width = 1) : _widthFactor(width) {
        setName(title);

        _slider.setSliderStyle(Slider::RotaryVerticalDrag);
        _slider.setMouseDragSensitivity(100);
        _slider.setTextBoxStyle(Slider::TextBoxAbove,
                                false, 60,
                                16);

        addAndMakeVisible(_slider);
        setLookAndFeel(&knobLookAndFeel);
    }

    ~Knob() override {
        setLookAndFeel(nullptr);
    }

    Slider &slider() {
        return _slider;
    }

    void paint(Graphics &g) override {
        g.setColour(Colour(220, 220, 220));

        auto font = getDefaultBoldFont();
        font.setHeight(fontHeight);
        font.setExtraKerningFactor(0.1);

        g.setFont(font);

        auto rect = getLocalBounds().toFloat();

        g.drawFittedText(getName().toUpperCase(),
                         0,
                         rect.getHeight() - labelHeight,
                         rect.getWidth(),
                         labelHeight,
                         Justification::centred, 2);
    }

    void resized() override {
        auto sliderRect = getLocalBounds();
        sliderRect.removeFromBottom(labelHeight);

        _slider.setBounds(sliderRect);
    }

    float minWidth() {
        return _widthFactor * defaultSize;
    }

    float minHeight() {
        return defaultSize;
    }

    int widthFactor() const {
        return _widthFactor;
    }

    void setWidthFactor(int width) {
        _widthFactor = width;
    }

protected:
    Slider _slider;

    int _widthFactor = 1;
    static constexpr auto labelHeight = 20.0f;
    static constexpr auto fontHeight = 16.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Knob)
};

class OscillatorKnob : public Knob {
public:
    static constexpr auto sliderName = "OscillatorSlider";

    explicit OscillatorKnob(const String &title, unsigned int width = 1) : Knob(title, width) {
        _slider.setTextBoxStyle(Slider::NoTextBox,
                                false, 50,
                                16);
        _slider.setName(sliderName);
        _slider.setSliderStyle(Slider::Rotary);

        const auto pi = MathConstants<float>::pi;

        _slider.setRotaryParameters(1.75f * pi, 2.25f * pi, true);
    }

    void resized() override {
        auto sliderRect = getLocalBounds();
        sliderRect.removeFromTop(16);
        sliderRect.removeFromBottom(labelHeight);

        _slider.setBounds(sliderRect);
    }
};

