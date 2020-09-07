/*
  ==============================================================================

    Knob.cpp
    Created: 14 Jan 2020 10:26:12pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#include "Knob.h"

#pragma mark - Construction

Knob::Knob(const String &title, unsigned int width) : _widthFactor(width) {
    Component::setName(title);

    _slider.setSliderStyle(Slider::RotaryVerticalDrag);
    _slider.setMouseDragSensitivity(100);
    _slider.setTextBoxStyle(Slider::TextBoxAbove, false, 60, 16);

    addAndMakeVisible(_slider);
    setLookAndFeel(&knobLookAndFeel);
}

Knob::~Knob() {
    setLookAndFeel(nullptr);
}

#pragma mark - Acessing Private Instances

Slider &Knob::slider() {
    return _slider;
}

#pragma mark - Geometry

float Knob::minWidth() {
    return _widthFactor * defaultSize;
}

float Knob::minHeight() {
    return defaultSize;
}

int Knob::widthFactor() const {
    return _widthFactor;
}

void Knob::setWidthFactor(int width) {
    _widthFactor = width;
}

#pragma mark - Virtual Methods Overrides

void Knob::paint(Graphics &g) {
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

void Knob::resized() {
    auto sliderRect = getLocalBounds();
    sliderRect.removeFromBottom(labelHeight);

    _slider.setBounds(sliderRect);
}

#pragma mark - OscillatorKnob

OscillatorKnob::OscillatorKnob(const String &title, unsigned int width) : Knob(title, width) {
    _slider.setTextBoxStyle(Slider::NoTextBox,
                            false, 50,
                            16);
    _slider.setName(sliderName);
    _slider.setSliderStyle(Slider::Rotary);

    const auto pi = MathConstants<float>::pi;

    _slider.setRotaryParameters(1.75f * pi, 2.25f * pi, true);
}

void OscillatorKnob::resized() {
    auto sliderRect = getLocalBounds();
    sliderRect.removeFromTop(16);
    sliderRect.removeFromBottom(labelHeight);

    _slider.setBounds(sliderRect);
}
