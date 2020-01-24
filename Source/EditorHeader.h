/*
  ==============================================================================

    EditorHeader.h
    Created: 23 Jan 2020 3:17:23am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "LookAndFeel.h"

class BlackFaceAudioProcessorEditor;
class EditorHeader : public Component {
public:
    explicit EditorHeader(BlackFaceAudioProcessorEditor &editor);
    ~EditorHeader() override;

    void paint(Graphics &) override;
    void resized() override;

    static constexpr auto nextPresetButtonName = "nextPresetButtonName";
    static constexpr auto previousPresetButtonName = "previousPresetButtonName";
private:
    static constexpr auto presetsListWidth = 200.0f;

    BlackFaceAudioProcessorEditor &editor;

    ComboBox presetsList;

    TextButton nextPresetButton{">"};
    TextButton previousPresetButton{"<"};
    TextButton savePresetButton{"Save"};

    HeaderLookAndFeel lookAndFeel;

    void updatePresetsList(const String &newSelectedPreset);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorHeader)
};
