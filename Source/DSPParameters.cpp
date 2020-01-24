/*
  ==============================================================================

    DSPParameters.cpp
    Created: 17 Jan 2020 3:45:16am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#include "DSPParameters.h"
#include "Synth.h"

inline String floatToSecondsString(float value, int maximumStringLength) {
    return String(value, 2) + " s";
};

DSPParameters::DSPParameters(AudioProcessorValueTreeState &valueTreeState) {
    using namespace DSPParametersConstants;

    oscillatorType = valueTreeState.getRawParameterValue(oscillatorTypeParameterID);
    detuningAmount = valueTreeState.getRawParameterValue(detuningParameterID);

    cutoff = valueTreeState.getRawParameterValue(filterCutoffParameterID);
    resonance = valueTreeState.getRawParameterValue(filterResonanceParameterID);
    filterDrive = valueTreeState.getRawParameterValue(filterDriveParameterID);

    attack = valueTreeState.getRawParameterValue(attackParameterID);
    decay = valueTreeState.getRawParameterValue(decayParameterID);
    sustain = valueTreeState.getRawParameterValue(sustainParameterID);
    release = valueTreeState.getRawParameterValue(releaseParameterID);

    cutoffEnvelopeAmount = valueTreeState.getRawParameterValue(cutoffEnvelopeAmountParameterID);
    resonanceEnvelopeAmount = valueTreeState.getRawParameterValue(resonanceEnvelopeAmountParameterID);
    velocityEnvelopeAmount = valueTreeState.getRawParameterValue(velocityEnvelopeAmountParameterID);

    reverb = valueTreeState.getRawParameterValue(reverbParameterID);
    masterGain = valueTreeState.getRawParameterValue(masterGainParameterID);
}

AudioProcessorValueTreeState::ParameterLayout DSPParameters::makeLayout() {
    using namespace DSPParametersConstants;
    return {
            std::make_unique<AudioParameterInt>(oscillatorTypeParameterID,
                                                oscillatorTypeParameterName,
                                                0,
                                                2,
                                                0,
                                                filterCutoffParameterName),

            std::make_unique<AudioParameterFloat>(detuningParameterID,
                                                  detuningParameterName,
                                                  NormalisableRange(0.0f,
                                                                    1.0f,
                                                                    0.01f),
                                                  0.5f,
                                                  detuningParameterName),

            std::make_unique<AudioParameterFloat>(filterCutoffParameterID,
                                                  filterCutoffParameterName,
                                                  makeFrequencyRange(),
                                                  Synth::defaultCutoff,
                                                  filterCutoffParameterName,
                                                  AudioProcessorParameter::genericParameter,
                                                  [](float value, int maximumStringLength) {
                                                      return String(static_cast<int>(value)) + " Hz";
                                                  }),

            std::make_unique<AudioParameterFloat>(filterResonanceParameterID,
                                                  filterResonanceParameterName,
                                                  NormalisableRange(0.0f,
                                                                    1.0f,
                                                                    0.01f),
                                                  Synth::defaultResonance,
                                                  filterResonanceParameterName),

            std::make_unique<AudioParameterFloat>(filterDriveParameterID,
                                                  filterDriveParameterName,
                                                  NormalisableRange(1.0f,
                                                                    100.0f,
                                                                    0.01f),
                                                  1.0f,
                                                  filterDriveParameterName),

            std::make_unique<AudioParameterFloat>(attackParameterID,
                                                  attackParameterName,
                                                  makeEnvelopeRange(),
                                                  Synth::defaultAttack,
                                                  attackParameterName,
                                                  AudioProcessorParameter::genericParameter,
                                                  floatToSecondsString),

            std::make_unique<AudioParameterFloat>(decayParameterID,
                                                  decayParameterName,
                                                  makeEnvelopeRange(),
                                                  Synth::defaultDecay,
                                                  decayParameterName,
                                                  AudioProcessorParameter::genericParameter,
                                                  floatToSecondsString),

            std::make_unique<AudioParameterFloat>(sustainParameterID,
                                                  sustainParameterName,
                                                  NormalisableRange(
                                                          0.0f,
                                                          1.0f,
                                                          0.01f
                                                  ),
                                                  Synth::defaultSustain,
                                                  sustainParameterName),

            std::make_unique<AudioParameterFloat>(releaseParameterID,
                                                  releaseParameterName,
                                                  makeEnvelopeRange(),
                                                  Synth::defaultRelease,
                                                  releaseParameterName,
                                                  AudioProcessorParameter::genericParameter,
                                                  floatToSecondsString),

            std::make_unique<AudioParameterFloat>(cutoffEnvelopeAmountParameterID,
                                                  cutoffEnvelopeAmountParameterName,
                                                  NormalisableRange(
                                                          -1.0f,
                                                          1.0f,
                                                          0.01f

                                                  ),
                                                  Synth::defaultCutoffEnvelopeAmount,
                                                  cutoffEnvelopeAmountParameterName),

            std::make_unique<AudioParameterFloat>(resonanceEnvelopeAmountParameterID,
                                                  resonanceEnvelopeAmountParameterName,
                                                  NormalisableRange(
                                                          -1.0f,
                                                          1.0f,
                                                          0.01f),
                                                  Synth::defaultResonanceEnvelopeAmount,
                                                  resonanceEnvelopeAmountParameterName),

            std::make_unique<AudioParameterFloat>(velocityEnvelopeAmountParameterID,
                                                  velocityEnvelopeAmountParameterName,
                                                  NormalisableRange(0.0f,
                                                                    1.0f,
                                                                    0.01f),
                                                  Synth::defaultVelocityEnvelopeAmount,
                                                  velocityEnvelopeAmountParameterName),

            std::make_unique<AudioParameterFloat>(reverbParameterID,
                                                  reverbParameterName,
                                                  NormalisableRange(0.0f,
                                                                    1.0f,
                                                                    0.01f),
                                                  Synth::defaultReverb,
                                                  reverbParameterName
            ),
            std::make_unique<AudioParameterFloat>(masterGainParameterID,
                                                  masterGainParameterName,
                                                  makeGainRange(),
                                                  Synth::defaultMasterGain,
                                                  masterGainParameterID)
    };
}

NormalisableRange<float> DSPParameters::makeEnvelopeRange() {
    auto range = NormalisableRange{Synth::minEnvelopeDurationSeconds,
                                   Synth::maxEnvelopeDurationSeconds};
    range.setSkewForCentre(1.0f);
    return range;
}

NormalisableRange<float> DSPParameters::makeFrequencyRange() {
    auto range = NormalisableRange{Synth::minCutoff,
                                   Synth::maxCutoff};
    range.setSkewForCentre(1000.0f);

    return range;
}

NormalisableRange<float> DSPParameters::makeGainRange() {
    auto range = NormalisableRange{0.0f,
                                   4.0f,
                                   0.01f};
    range.setSkewForCentre(1.0f);

    return range;
}
