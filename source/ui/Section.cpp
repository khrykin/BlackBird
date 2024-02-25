/*
  ==============================================================================

    Section.cpp
    Created: 14 Jan 2020 10:25:51pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#include "Section.h"

#pragma mark - Construction

Section::Section(const String &title, unsigned int size) : _size(size) {
  Component::setName(title);
}

#pragma mark - Adding Knobs

void Section::addKnob(Knob *knob) {
  addAndMakeVisible(knobs.add(knob));
  resized();
}

#pragma mark - Geometry

Rectangle<float> Section::availableArea() {
  auto rect = getLocalBounds();
  return {0.5f * borderThickness, titleAreaHeight,
          rect.getWidth() - borderThickness,
          rect.getHeight() - titleAreaHeight - 0.5f * borderThickness};
}

float Section::recommendedHeight() {
  return 2 * Knob::defaultSize + borderThickness + titleAreaHeight;
}

float Section::recommendedWidth() {
  return _size * Knob::defaultSize + 2 * borderThickness;
}

#pragma mark - Virtual Methods Overrides

void Section::paint(Graphics &g) {
  g.setColour(Colour(220, 220, 220));

  auto font = getDefaultBoldFont();
  font.setHeight(fontHeight);
  font.setExtraKerningFactor(0.1);

  g.setFont(font);

  auto rect = getLocalBounds();

  g.drawFittedText(getName().toUpperCase(), 0, 0, rect.getWidth(),
                   titleAreaHeight, Justification::centredTop, 1);

  g.setColour(Colour(21, 22, 22));

  g.fillRoundedRectangle(
      0.5 * borderThickness, titleAreaHeight, rect.getWidth() - borderThickness,
      rect.getHeight() - titleAreaHeight - 0.5 * borderThickness, borderRadius);

  g.setColour(Colour(220, 220, 220));

  g.drawRoundedRectangle(
      0.5 * borderThickness, titleAreaHeight, rect.getWidth() - borderThickness,
      rect.getHeight() - titleAreaHeight - 0.5 * borderThickness, borderRadius,
      borderThickness);
}

void Section::resized() {
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
