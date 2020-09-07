/*
  ==============================================================================

    EditorHeader.cpp
    Created: 23 Jan 2020 3:17:23am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "EditorHeader.h"
#include "PluginEditor.h"

EditorHeader::EditorHeader(BlackFaceAudioProcessorEditor &editor) : editor(editor) {
    addAndMakeVisible(presetsList);
    setLookAndFeel(&lookAndFeel);

    presetsList.addItemList(editor.processor.getPresetsNames(), 1);

    presetsList.setSelectedId(1);
    presetsList.setJustificationType(Justification::centred);

    presetsList.setColour(PopupMenu::backgroundColourId, Colours::transparentBlack);

    presetsList.onChange = [&]() {
        auto selectedPresetIndex = presetsList.getSelectedItemIndex();

        editor.processor.silentlySetCurrentProgram(selectedPresetIndex);
    };

    previousPresetButton.onClick = [this] {
        auto selectedIndex = presetsList.getSelectedItemIndex();
        if (presetsList.getSelectedItemIndex() == 0)
            selectedIndex = presetsList.getNumItems();

        presetsList.setSelectedItemIndex(--selectedIndex);
    };

    previousPresetButton.setName(previousPresetButtonName);

    addAndMakeVisible(previousPresetButton);

    nextPresetButton.onClick = [this] {
        auto selectedIndex = presetsList.getSelectedItemIndex();

        if (presetsList.getSelectedItemIndex() == presetsList.getNumItems() - 1)
            selectedIndex = -1;

        presetsList.setSelectedItemIndex(++selectedIndex);
    };

    nextPresetButton.setName(nextPresetButtonName);

    addAndMakeVisible(nextPresetButton);
    addAndMakeVisible(savePresetButton);

    savePresetButton.onClick = [this, &editor] {
        FileChooser fc(("Save current preset"), editor.processor.getPresetsDirectory(), "*.blackFace");

        if (fc.browseForFileToSave(true)) {
            MemoryBlock data;
            editor.processor.getStateInformation(data);

            auto file = fc.getResult();
            if (!file.replaceWithData(data.getData(), data.getSize())) {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                                 TRANS("Error whilst saving"),
                                                 TRANS("Couldn't write to the specified file!"));
            } else {
                auto newPreset = file.getFileNameWithoutExtension();
                updatePresetsList(newPreset);
            }
        }
    };

    editor.processor.onProgramChange = [this](int index) {
        presetsList.setSelectedItemIndex(index);
    };

    savePresetButton.setColour(TextButton::textColourOffId, Colour(200, 200, 200));
}

EditorHeader::~EditorHeader() {
    setLookAndFeel(nullptr);
}

void EditorHeader::resized() {
    auto presetsComboRect = getLocalBounds();
    auto totalWidth = presetsComboRect.getWidth();

    presetsComboRect.setWidth(presetsListWidth);
    presetsComboRect.setX(0.5f * (totalWidth - presetsListWidth));

    presetsList.setBounds(presetsComboRect);

    auto presetButtonRect = presetsComboRect;
    presetButtonRect.setWidth(30);
    presetButtonRect.setX(presetsComboRect.getX() + presetsComboRect.getWidth());

    nextPresetButton.setBounds(presetButtonRect);

    presetButtonRect.setX(presetButtonRect.getX() + presetButtonRect.getWidth() + (int) editor.padding);

    savePresetButton.setBounds(presetButtonRect.withWidth(50));

    presetButtonRect.setX(presetsComboRect.getX() - 30);
    previousPresetButton.setBounds(presetButtonRect);
}

void EditorHeader::updatePresetsList(const String &newSelectedPreset) {
    auto newPresets = editor.processor.getPresetsNames();
    auto newPresetIndex = newPresets.indexOf(newSelectedPreset);
    if (newPresetIndex < 0) {
        newPresetIndex = 0;
    }

    presetsList.clear();
    presetsList.addItemList(newPresets, 1);

    presetsList.setSelectedId(newPresetIndex + 1);
}