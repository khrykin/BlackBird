/*
  ==============================================================================

    LookAndFeel.h
    Created: 14 Jan 2020 10:26:04pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include <BinaryData.h>
#include "../JuceLibraryCode/JuceHeader.h"

Font getDefaultFont();
Font getDefaultBoldFont();

namespace LookAndFeelColors {
    inline const auto dotColor = Colour(150, 150, 150);
    inline const auto selectedDotColor = Colour(255, 255, 230);
    inline const auto shadowColor = Colour(10, 10, 10).withAlpha(0.5f);
    inline const auto knobColor = Colour(30, 30, 30);
    inline const auto pointerColor = Colour(200, 200, 200);
    inline const auto labelBackgroundColor = Colour(33, 34, 34).withAlpha(0.5f);
    inline const auto glowColor = Colour(255, 255, 0).withAlpha(0.5f);
};

class KnobLookAndFeel : public LookAndFeel_V4 {
public:
    struct IconInfo {
        const char *data;
        const int size;
    };

    std::vector<IconInfo> iconInfos = {
            {
                    BinaryData::sinewave_svg,
                    BinaryData::sinewave_svgSize
            },
            {
                    BinaryData::sawwave_svg,
                    BinaryData::sawwave_svgSize
            },
            {
                    BinaryData::squarewave_svg,
                    BinaryData::squarewave_svgSize
            }
    };

    std::vector<std::unique_ptr<Drawable>> icons;

    inline static const DropShadow dotsShadow = {LookAndFeelColors::glowColor, 3, juce::Point{0, 0}};
    inline static const DropShadow oscShadow = {LookAndFeelColors::glowColor.withAlpha(1.0f), 20, juce::Point{0, 0}};

    KnobLookAndFeel();

    void drawRotarySlider(Graphics &g,
                          int x, int y, int width, int height,
                          float sliderPos,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          Slider &slider) override;

    void drawLabel(Graphics &g, Label &label) override;

    Slider::SliderLayout getSliderLayout(Slider &slider) override;
    Label *createSliderTextBox(Slider &slider) override;

    void fillTextEditorBackground(Graphics &, int width, int height, TextEditor &) override;
    void drawTextEditorOutline(Graphics &, int width, int height, TextEditor &) override;
};

class HeaderLookAndFeel : public LookAndFeel_V4 {
public:
    HeaderLookAndFeel();

    void drawComboBox(Graphics &,
                      int width,
                      int height,
                      bool isButtonDown,
                      int buttonX,
                      int buttonY,
                      int buttonW,
                      int buttonH, ComboBox &) override;

    Label *createComboBoxTextBox(ComboBox &) override;
    void positionComboBoxText(ComboBox &, Label &labelToPosition) override;
    void drawLabel(Graphics &g, Label &label) override;
    void drawPopupMenuBackground(Graphics &, int width, int height) override;
    Font getPopupMenuFont() override;
    Font getTextButtonFont(TextButton &, int buttonHeight) override;

    void drawButtonBackground(Graphics &,
                              Button &,
                              const Colour &backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawButtonText(Graphics &,
                        TextButton &,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
};