/*
  ==============================================================================

    EditorHeader.h
    Created: 23 Jan 2020 3:17:23am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "LookAndFeel.h"

#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

class BlackBirdAudioProcessorEditor;
class EditorHeader : public Component {
public:
  explicit EditorHeader(BlackBirdAudioProcessorEditor &editor);
  ~EditorHeader() override;

  void resized() override;

  static constexpr auto nextPresetButtonName = "nextPresetButtonName";
  static constexpr auto previousPresetButtonName = "previousPresetButtonName";

private:
  static constexpr auto presetsListWidth = 200.0f;

  BlackBirdAudioProcessorEditor &editor;

  ComboBox presetsList;

  TextButton nextPresetButton{">"};
  TextButton previousPresetButton{"<"};
  TextButton savePresetButton{"Save"};

  HeaderLookAndFeel lookAndFeel;

  void updatePresetsList(const String &newSelectedPreset);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorHeader)
};
