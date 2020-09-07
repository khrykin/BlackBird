/*
  ==============================================================================

    MultibandLookupTable.h
    Created: 7 Sep 2020 2:08:06pm
    Author:  Dmitry Khrykin

  ==============================================================================
*/

#pragma once

#include <cassert>
#include <cmath>
#include <array>

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * This class combines lookup tables for several frequency bands into one callable entity.
 */
template<typename FloatType>
class MultibandLookupTable {
public:
    using TableGenerator = std::function<FloatType(FloatType maxFrequency, FloatType value)>;

    enum Band {
        Sub,
        Bass,
        LowMid,
        Mid,
        UpperMid,
        Presence,
        Brilliance,
        NumberOfBands
    };

    static constexpr auto pi = MathConstants<FloatType>::pi;

#pragma mark - Initialization

    MultibandLookupTable() = default;

    void setTable(const TableGenerator &tableGenerator, int tableSize) {
        forEachBand([=](Band band) {
            auto bandTableGenerator = [=](FloatType phase) {
                auto maxFrequency = bandMaxFrequencies[band];
                return tableGenerator(maxFrequency, phase);
            };

            tables[band] = std::make_unique<dsp::LookupTableTransform<FloatType>>(bandTableGenerator,
                                                                                  -pi,
                                                                                  pi,
                                                                                  tableSize);
        });
    }

#pragma mark - Call Operator

    FloatType operator()(FloatType phase, FloatType frequency) const {
        assert(tables[0] != nullptr && "setTable() must be called before operator()");

        auto band = bandForFrequency(frequency);

        return (*tables[band])(phase);
    }

private:
    using SingleBandLookupTable = dsp::LookupTableTransform<FloatType>;

    static constexpr std::array<FloatType, NumberOfBands> bandMaxFrequencies = {
            60,     // Sub
            250,    // Bass
            500,    // LowMid
            2000,   // Mid
            4000,   // UpperMid
            6000,   // Presence
            20000   // Brilliance
    };

#pragma mark - Private Members

    std::array<std::unique_ptr<SingleBandLookupTable>, NumberOfBands> tables;

#pragma mark - Resolving Frequency Band

    static constexpr Band bandForFrequency(FloatType frequency) {
        for (auto &maxFrequency : bandMaxFrequencies) {
            auto bandIndex = &maxFrequency - &bandMaxFrequencies[0];
            auto band = static_cast<Band>(bandIndex);

            if (frequency < maxFrequency) {
                return band;
            }
        }

        return Brilliance;
    }

#pragma mark - Iterating Frequency Bands

    void forEachBand(const std::function<void(Band)> &fn) {
        for (int band = Sub; band < NumberOfBands; band++) {
            fn(static_cast<Band>(band));
        }
    }
};
