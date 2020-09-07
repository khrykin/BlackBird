/*
  ==============================================================================

    DSPParameters.h
    Created: 17 Jan 2020 3:27:29am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace DSPParametersConstants {
    constexpr auto oscillatorWaveformParameterID = "oscillatorWaveform";
    constexpr auto oscillatorWaveformParameterName = "Oscillator Waveform";
    constexpr auto characterParameterID = "detuning";
    constexpr auto characterParameterName = "Character";

    constexpr auto filterCutoffParameterID = "filterCutoff";
    constexpr auto filterCutoffParameterName = "Filter Cutoff";
    constexpr auto filterResonanceParameterID = "filterResonance";
    constexpr auto filterResonanceParameterName = "Filter Resonance";
    constexpr auto filterDriveParameterID = "filterDrive";
    constexpr auto filterDriveParameterName = "Filter Drive";

    constexpr auto attackParameterID = "attack";
    constexpr auto attackParameterName = "Attack";
    constexpr auto decayParameterID = "decay";
    constexpr auto decayParameterName = "Decay";
    constexpr auto sustainParameterID = "sustain";
    constexpr auto sustainParameterName = "Sustain";
    constexpr auto releaseParameterID = "release";
    constexpr auto releaseParameterName = "Release";

    constexpr auto cutoffEnvelopeAmountParameterID = "cutoffEnvelopeAmount";
    constexpr auto cutoffEnvelopeAmountParameterName = "Cutoff Envelope Amount";
    constexpr auto resonanceEnvelopeAmountParameterID = "resonanceEnvelopeAmount";
    constexpr auto resonanceEnvelopeAmountParameterName = "Resonance Envelope Amount";
    constexpr auto velocityEnvelopeAmountParameterID = "velocityEnvelopeAmount";
    constexpr auto velocityEnvelopeAmountParameterName = "Velocity Envelope Amount";

    constexpr auto reverbParameterID = "reverb";
    constexpr auto reverbParameterName = "Reverb";

    constexpr auto masterGainParameterID = "masterGain";
    constexpr auto masterGainParameterName = "Master Gain";
}

struct DSPParameters {
    std::atomic<float> *oscillatorWaveform = nullptr;
    std::atomic<float> *detuningAmount = nullptr;

    std::atomic<float> *cutoff = nullptr;
    std::atomic<float> *resonance = nullptr;
    std::atomic<float> *filterDrive = nullptr;

    std::atomic<float> *attack = nullptr;
    std::atomic<float> *decay = nullptr;
    std::atomic<float> *sustain = nullptr;
    std::atomic<float> *release = nullptr;

    std::atomic<float> *cutoffEnvelopeAmount = nullptr;
    std::atomic<float> *resonanceEnvelopeAmount = nullptr;
    std::atomic<float> *velocityEnvelopeAmount = nullptr;

    std::atomic<float> *reverb = nullptr;
    std::atomic<float> *masterGain = nullptr;

    explicit DSPParameters(AudioProcessorValueTreeState &valueTreeState);

    static AudioProcessorValueTreeState::ParameterLayout makeLayout();
};
