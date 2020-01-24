/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 14 Jan 2020 10:26:04pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "EditorHeader.h"
#include "Knob.h"

KnobLookAndFeel::KnobLookAndFeel() {
    for (auto &info : iconInfos) {
        icons.emplace_back(Drawable::createFromImageData(info.data, info.size));
    }
}

void KnobLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider &slider) {
    using namespace LookAndFeelColors;

    auto centerX = x + width / 2;
    auto centerY = y + height / 2;

    auto numberOfDots = 11;
    auto dotsAngleIncrement = (rotaryEndAngle - rotaryStartAngle)
                              / (numberOfDots - 1.0f);
    auto dotSize = 2.0f;
    auto dotsRadius = jmin(width / 2, height / 2) - dotSize;
    auto knobRadius = dotsRadius * 0.6f;

    if (slider.getName() == OscillatorKnob::sliderName) { // Draw oscillator type knob
        auto numberOfLines = (int) icons.size();
        auto lineLength = 10.0f;
        auto lineGap = 5.0f;

        auto linesAngleIncrement = (rotaryEndAngle - rotaryStartAngle)
                                   / (numberOfLines - 1.0f);

        for (auto i = 0; i < numberOfLines; i++) {
            auto lineAngle = rotaryStartAngle + i * linesAngleIncrement;
            auto percentage = (float) i / (numberOfLines - 1.0f);
            auto isHighlighted = sliderPos == percentage;

            Path line;
            line.addLineSegment({0.0f, 0.0f, 0.0f, lineLength}, 0.0f);
            line.applyTransform(AffineTransform::translation(centerX,
                                                             centerY - knobRadius - lineGap - lineLength)
                                        .rotated(lineAngle, centerX, centerY));


            g.setColour(Colours::white);
            g.strokePath(line, PathStrokeType(1.0, PathStrokeType::mitered));

            auto &icon = icons[i];

            auto imageOrigin = line.getPointAlongPath(0.1f).toFloat();
            imageOrigin.addXY((float) -1.0f * icon->getWidth() + 1.5f * lineGap * std::sin(lineAngle),
                              (float) -1.5f * icon->getHeight() - lineGap * std::cos(lineAngle));

            auto effect = DropShadowEffect();
            effect.setShadowProperties(oscShadow);

            if (!isHighlighted) {
                effect.setShadowProperties(DropShadow(Colours::transparentWhite,
                                                      1,
                                                      juce::Point(0, 0)));
            }

            icon->setComponentEffect(&effect);
            icon->draw(g, 1.0f, AffineTransform::scale(1.0f)
                    .translated(imageOrigin.getX(),
                                imageOrigin.getY()));


        }
    } else { // Draw dots
        g.setColour(dotColor);
        for (auto i = 0; i < numberOfDots; i++) {
            auto dotAngle = rotaryStartAngle + i * dotsAngleIncrement;
            auto dotPercentage = (float) i / numberOfDots;

            auto isHighlighted = sliderPos >= dotPercentage && sliderPos != 0;
            Path p;
            p.addEllipse(-dotSize,
                         -dotSize,
                         2 * dotSize,
                         2 * dotSize);
            p.applyTransform(AffineTransform::translation(centerX, centerY - dotsRadius)
                                     .rotated(dotAngle, centerX, centerY));

            if (isHighlighted) {
                dotsShadow.drawForPath(g, p);
            }

            g.setColour(isHighlighted ? selectedDotColor : dotColor);
            g.fillPath(p);
        }
    }

    { // Draw a shadow
        auto shadowShift = 3.0f;
        auto shadowAngle = -MathConstants<float>::pi / 3;

        g.setColour(shadowColor);

        Path shadow;
        shadow.addEllipse(centerX - knobRadius,
                          centerY - knobRadius,
                          2 * knobRadius,
                          2 * knobRadius);

        shadow.addEllipse(centerX - knobRadius - shadowShift,
                          centerY - knobRadius,
                          2 * knobRadius,
                          2 * knobRadius);
        shadow.addRectangle(centerX - shadowShift,
                            centerY - knobRadius,
                            shadowShift,
                            2 * knobRadius);

        shadow.applyTransform(AffineTransform::rotation(shadowAngle, centerX, centerY));

        auto knobShadow = DropShadow{shadowColor, 3, juce::Point{0, 0}};
        knobShadow.drawForPath(g, shadow);
    }

    { // Draw a knob
        g.setColour(knobColor);
        g.fillEllipse(centerX - knobRadius,
                      centerY - knobRadius,
                      2 * knobRadius,
                      2 * knobRadius);

    }

    { // Draw a knob's pointer
        auto lineThickness = 2.0f;
        auto pointerLength = knobRadius;

        Path pointer;
        pointer.addRoundedRectangle(-lineThickness * 0.5f,
                                    -knobRadius,
                                    lineThickness,
                                    pointerLength,
                                    0.5f * lineThickness);

        auto pointerAngle = rotaryStartAngle
                            + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        pointer.applyTransform(AffineTransform::rotation(pointerAngle)
                                       .translated(centerX, centerY));

        g.setColour(pointerColor);
        g.fillPath(pointer);
    }
}

void KnobLookAndFeel::drawLabel(Graphics &g, Label &label) {
    using namespace LookAndFeelColors;

    g.setColour(labelBackgroundColor);

    g.fillRoundedRectangle(label.getLocalBounds().toFloat(),
                           0.5f * label.getLocalBounds().getHeight());

    if (!label.isBeingEdited()) {
        auto font = getDefaultFont();
        font.setHeight(16);

        auto textColor = Colour(200, 200, 200);
        g.setColour(textColor);
        g.setFont(font);

        auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         jmax(1, (int) (textArea.getHeight() / font.getHeight())),
                         label.getMinimumHorizontalScale());
    }
}

Slider::SliderLayout KnobLookAndFeel::getSliderLayout(Slider &slider) {
    auto layout = getDefaultLookAndFeel().getSliderLayout(slider);

    layout.textBoxBounds.setTop(8);
    layout.textBoxBounds.removeFromBottom(-8);
    layout.sliderBounds.removeFromTop(16);

    return layout;
}

Label *KnobLookAndFeel::createSliderTextBox(Slider &slider) {
    auto l = new Label();

    auto font = getDefaultFont();
    font.setHeight(16);

    l->setFont(font);

    l->setJustificationType(Justification::centred);
    l->setKeyboardType(TextInputTarget::decimalKeyboard);

    l->setColour(Label::textColourId, slider.findColour(Slider::textBoxTextColourId));
    l->setColour(Label::backgroundColourId,
                 (slider.getSliderStyle() == Slider::LinearBar ||
                  slider.getSliderStyle() == Slider::LinearBarVertical)
                 ? Colours::transparentBlack
                 : slider.findColour(Slider::textBoxBackgroundColourId));
    l->setColour(Label::outlineColourId, slider.findColour(Slider::textBoxOutlineColourId));
    l->setColour(TextEditor::textColourId, slider.findColour(Slider::textBoxTextColourId));
    l->setColour(TextEditor::backgroundColourId,
                 slider.findColour(Slider::textBoxBackgroundColourId)
                         .withAlpha((slider.getSliderStyle() == Slider::LinearBar ||
                                     slider.getSliderStyle() == Slider::LinearBarVertical)
                                    ? 0.7f : 1.0f));

    l->setColour(TextEditor::outlineColourId, slider.findColour(Slider::textBoxOutlineColourId));
    l->setColour(TextEditor::highlightColourId, slider.findColour(Slider::textBoxHighlightColourId));

    return l;
}

void KnobLookAndFeel::fillTextEditorBackground(Graphics &g, int width, int height, TextEditor &editor) {
    g.setColour(Colour(50, 50, 50));
    g.fillRoundedRectangle(editor.getLocalBounds().toFloat(),
                           0.5f * editor.getLocalBounds().getHeight());

    // This is a dirty hack, but it works.
    // Obviously not the best place to call
    // this method.
    editor.setJustification(Justification::centred);
}

void KnobLookAndFeel::drawTextEditorOutline(Graphics &g, int width, int height, TextEditor &editor) {
//    g.drawRoundedRectangle(1, 1, width - 2, height - 2, 0.5f * height, 2.0);
}

void HeaderLookAndFeel::drawComboBox(Graphics &g,
                                     int width,
                                     int height,
                                     bool isButtonDown,
                                     int buttonX,
                                     int buttonY,
                                     int buttonW,
                                     int buttonH,
                                     ComboBox &box) {
    Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(Colour(21, 22, 22).withMultipliedSaturation(isButtonDown ? 1.3f : 0.9f)
                        .withMultipliedAlpha(!isButtonDown ? 1.0f : 0.5f));
    g.fillRect(boxBounds.toFloat());
}

Label *HeaderLookAndFeel::createComboBoxTextBox(ComboBox &) {
    auto label = new Label(String(), String());
    label->setJustificationType(Justification::centred);

    auto font = getDefaultFont();
    font.setHeight(16);

    label->setFont(font);

    return label;
}

void HeaderLookAndFeel::drawLabel(Graphics &g, Label &label) {
    using namespace LookAndFeelColors;

    if (!label.isBeingEdited()) {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;

        auto font = getDefaultFont();
        font.setHeight(18);
        font.setExtraKerningFactor(0.1);

        auto textColor = Colour(200, 200, 200).withAlpha(alpha);
        g.setColour(textColor);
        g.setFont(font);

        auto textArea = BorderSize(2).subtractedFrom(label.getLocalBounds());

        g.drawFittedText(label.getText().toUpperCase(), textArea, label.getJustificationType(),
                         jmax(1, (int) (textArea.getHeight() / font.getHeight())),
                         label.getMinimumHorizontalScale());
    }
}

void HeaderLookAndFeel::drawPopupMenuBackground(Graphics &g, int width, int height) {
    g.setColour(Colour(21, 22, 22));
    g.fillAll();
}

Font HeaderLookAndFeel::getPopupMenuFont() {
    auto font = getDefaultFont();
    font.setHeight(20);
    return font;
}

HeaderLookAndFeel::HeaderLookAndFeel() {
}

void HeaderLookAndFeel::drawButtonBackground(Graphics &g, Button &button, const Colour &backgroundColour,
                                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerSize = 0.5f * bounds.getHeight();

    auto baseColour = Colour(21, 22, 22).withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour(baseColour);

    auto flatOnLeft = button.getName() == EditorHeader::nextPresetButtonName;
    auto flatOnRight = button.getName() == EditorHeader::previousPresetButtonName;
    auto flatOnTop = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom) {
        Path path;
        path.addRoundedRectangle(bounds.getX(), bounds.getY(),
                                 bounds.getWidth(), bounds.getHeight(),
                                 cornerSize, cornerSize,
                                 !(flatOnLeft || flatOnTop),
                                 !(flatOnRight || flatOnTop),
                                 !(flatOnLeft || flatOnBottom),
                                 !(flatOnRight || flatOnBottom));

        g.fillPath(path);
    } else {
        g.fillRoundedRectangle(bounds, cornerSize);
    }
}

Font HeaderLookAndFeel::getTextButtonFont(TextButton &, int buttonHeight) {
    auto font = getDefaultFont();
    font.setHeight(20);
    return font;
}

void HeaderLookAndFeel::drawButtonText(Graphics &g,
                                       TextButton &button,
                                       bool shouldDrawButtonAsHighlighted,
                                       bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds();

    auto arrowWidth = bounds.getHeight() * 0.75;

    Rectangle<int> arrowZone(0.5 * (bounds.getWidth() - arrowWidth),
                             0,
                             arrowWidth,
                             bounds.getHeight());

    Path path;
    path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
    path.lineTo(static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
    path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);


    g.setColour(Colour(220, 220, 220).withAlpha((shouldDrawButtonAsHighlighted ? 0.9f : 0.2f)));

    if (button.getName() == EditorHeader::nextPresetButtonName) {
        path.applyTransform(
                AffineTransform::rotation(-0.5f * MathConstants<float>::pi,
                                          arrowZone.getCentreX(),
                                          arrowZone.getCentreY())
                        .translated(-2, 0));
        g.strokePath(path, PathStrokeType(2.0f));

    } else if (button.getName() == EditorHeader::previousPresetButtonName) {
        path.applyTransform(
                AffineTransform::rotation(0.5f * MathConstants<float>::pi,
                                          arrowZone.getCentreX(),
                                          arrowZone.getCentreY())
                        .translated(2, 0));

        g.strokePath(path, PathStrokeType(2.0f));

    } else {
        LookAndFeel_V4::drawButtonText(g,
                                       button,
                                       shouldDrawButtonAsHighlighted,
                                       shouldDrawButtonAsDown);
    }
}

void HeaderLookAndFeel::positionComboBoxText(ComboBox &box, Label &label) {
    label.setBounds(1, 1,
                    box.getWidth() - 2,
                    box.getHeight() - 2);

    label.setFont(getComboBoxFont(box));
}

Font getDefaultFont() {
    static auto defaultFont = Typeface::createSystemTypefaceFor(BinaryData::Jost600Semi_otf,
                                                                BinaryData::Jost600Semi_otfSize);
    return defaultFont;
}

Font getDefaultBoldFont() {
    static auto defaultBoldFont = Typeface::createSystemTypefaceFor(BinaryData::Jost700Bold_otf,
                                                                    BinaryData::Jost700Bold_otfSize);
    return defaultBoldFont;
}
