cmake_minimum_required(VERSION 3.22)

# TODO: Copy version from a Git tag
project(BlackBird VERSION 0.0.1)

# TODO: Enable warnings
add_definitions(-w)

set(CXX_STANDARD 17)

# For clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(JUCE)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (BlackBird here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.
juce_add_plugin(BlackBird

    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    # COMPANY_NAME ...                          # Specify the name of the plugin's author
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE DKhr # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Blbd # A unique four-character plugin id with exactly one upper-case character

    # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "BlackBird") # The name of the final executable, which can differ from the target name

target_include_directories(BlackBird
    PRIVATE
    source/ui
    source/dsp)

target_sources(BlackBird
    PRIVATE
    source/dsp/DSPParameters.cpp
    source/dsp/LadderFilter.cpp
    source/ui/EditorHeader.cpp
    source/ui/Knob.cpp
    source/ui/PluginEditor.cpp
    source/ui/LookAndFeel.cpp
    source/ui/Section.cpp
    source/PluginProcessor.cpp)

target_compile_definitions(BlackBird
    PRIVATE
    JUCE_MODAL_LOOPS_PERMITTED)

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.
target_compile_definitions(BlackBird
    PUBLIC
    JUCE_WEB_BROWSER=0 # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0 # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.
juce_add_binary_data(AudioPluginData
    SOURCES
    resources/Jost-600-Semi.ttf
    resources/Jost-700-Bold.ttf
    resources/sawwave.svg
    resources/sinewave.svg
    resources/squarewave.svg)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.
target_link_libraries(BlackBird
    PRIVATE
    AudioPluginData
    juce::juce_audio_utils
    juce::juce_dsp

    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
