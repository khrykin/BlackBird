/*
  ==============================================================================

    TableLookupOscillator.h
    Created: 7 Sep 2020 8:58:41am
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <thread>

#include "../JuceLibraryCode/JuceHeader.h"

#include "LookupTablesBank.h"

template<typename ValueType>
class VCAOscillator {
public:
    using Oscillator = dsp::Oscillator<ValueType>;
    using LookupTablesBank = LookupTablesBank<ValueType>;
    using Waveform = typename LookupTablesBank::Waveform;

    VCAOscillator() noexcept = default;

#pragma mark - Initialization

    void initialize(const LookupTablesBank &lookupTable) {
        oscillator().initialise([this, &lookupTable](ValueType phase) {
            return lookupTable(phase,
                               currentWaveform,
                               oscillator().getFrequency());
        });
    }

#pragma mark - Preparing for Operation

    void prepare(const dsp::ProcessSpec &spec) {
        processorChain.prepare(spec);
        sampleRate = spec.sampleRate;
    }

#pragma mark - Setting & Getting Waveform

    void setWaveform(Waveform waveform) {
        currentWaveform = waveform;
    }

#pragma mark - Setting Properties

    void setFrequency(ValueType newValue) {
        const auto nyquistFrequency = 0.5 * sampleRate;
        if (newValue > nyquistFrequency)
            newValue = nyquistFrequency;

        oscillator().setFrequency(newValue, true);
    }

    void setLevel(ValueType newValue) {
        gainProcessor().setGainLinear(newValue);
    }

    void setRampDurationSeconds(double duration) {
        gainProcessor().setRampDurationSeconds(duration);
    }

#pragma mark - Resetting Processing

    void reset() noexcept {
        processorChain.reset();
    }

#pragma mark - Processing Audio Context

    template<typename ProcessContext>
    void process(const ProcessContext &context) noexcept {
        processorChain.process(context);
    }

private:
    enum {
        oscIndex,
        gainIndex
    };

    Waveform currentWaveform{};

    double sampleRate = 0;

    dsp::ProcessorChain<
            Oscillator,
            dsp::Gain<ValueType>
    > processorChain;

#pragma mark - Accessing Processors

    Oscillator &oscillator() {
        return processorChain.template get<oscIndex>();
    }

    dsp::Gain<ValueType> &gainProcessor() {
        return processorChain.template get<gainIndex>();
    }
};
