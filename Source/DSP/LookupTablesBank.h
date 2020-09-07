/*
  ==============================================================================

    LookupTablesBank.h
    Created: 7 Sep 2020 3:06:47pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/


#pragma once

#include <cassert>
#include <cmath>
#include <array>

#include "../JuceLibraryCode/JuceHeader.h"

#include "MultibandLookupTable.h"
#include "SineSeries.h"

/**
 * This class groups several `MultibandLookupTable`s into single callable entity.
 */
template<typename FloatType>
class LookupTablesBank {
public:

    static constexpr auto tableResolution = 1024;

    using LookupTable = MultibandLookupTable<FloatType>;

#pragma mark - Waveforms

    enum Waveform {
        Sine,
        Saw,
        Square,
        NumberOfWaveForms
    };

#pragma mark - Construction

    void initialize(double sampleRate) {
        _sampleRate = sampleRate;

        tables[Sine].setTable([](FloatType /* maxFrequnecy */, FloatType value) {
            return std::sin(value);
        }, tableResolution);

        tables[Saw].setTable([this](FloatType maxFrequnecy, FloatType value) {
            auto seriesOrder = nyquistFrequency() / maxFrequnecy;

            safequardSeriesOrder(seriesOrder);

            return sawSeries<FloatType>(seriesOrder)(value);
        }, tableResolution);

        tables[Square].setTable([this](FloatType maxFrequnecy, FloatType value) {
            auto seriesOrder = 0.5 * (nyquistFrequency() / maxFrequnecy + 1);

            safequardSeriesOrder(seriesOrder);

            return squareSeries<FloatType>(seriesOrder)(value);
        }, tableResolution);
    }

#pragma mark - Call Operator

    FloatType operator()(FloatType phase, Waveform waveform, FloatType frequency) const {
        assert(_sampleRate && "intialize() must be called before operator()");

        return tables[waveform](phase, frequency);
    }

private:

#pragma mark - Private Members

    FloatType _sampleRate = 0;

    std::array<LookupTable, NumberOfWaveForms> tables;

#pragma mark - Helpers

    template<typename T>
    inline void safequardSeriesOrder(T &order) {
        if (order < 1)
            order = 1;
    }

    inline FloatType nyquistFrequency() {
        return 0.5 * _sampleRate;
    }
};