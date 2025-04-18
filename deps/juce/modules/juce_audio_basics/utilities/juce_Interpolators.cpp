/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

#if JUCE_UNIT_TESTS

class InterpolatorTests final : public UnitTest
{
public:
    InterpolatorTests()
        : UnitTest ("InterpolatorTests", UnitTestCategories::audio)
    {
    }

private:
    template <typename InterpolatorType>
    void runInterplatorTests (const String& interpolatorName)
    {
        auto createGaussian = [] (std::vector<float>& destination, float scale, float centreInSamples, float width)
        {
            for (size_t i = 0; i < destination.size(); ++i)
            {
                auto x = (((float) i) - centreInSamples) * width;
                destination[i] = std::exp (-(x * x));
            }

            FloatVectorOperations::multiply (destination.data(), scale, (int) destination.size());
        };

        auto findGaussianPeak = [] (const std::vector<float>& input) -> float
        {
            auto max = std::max_element (std::begin (input), std::end (input));
            auto maxPrev = max - 1;
            jassert (maxPrev >= std::begin (input));
            auto maxNext = max + 1;
            jassert (maxNext < std::end (input));
            auto quadraticMaxLoc = (*maxPrev - *maxNext) / (2.0f * ((*maxNext + *maxPrev) - (2.0f * *max)));
            return quadraticMaxLoc + (float) std::distance (std::begin (input), max);
        };

        auto expectAllElementsWithin = [this] (const std::vector<float>& v1, const std::vector<float>& v2, float tolerance)
        {
            expectEquals ((int) v1.size(), (int) v2.size());

            for (size_t i = 0; i < v1.size(); ++i)
                expectWithinAbsoluteError (v1[i], v2[i], tolerance);
        };

        InterpolatorType interpolator;

        constexpr size_t inputSize = 1001;
        static_assert (inputSize > 800 + InterpolatorType::getBaseLatency(),
                       "The test InterpolatorTests input buffer is too small");

        std::vector<float> input (inputSize);
        constexpr auto inputGaussianMidpoint = (float) (inputSize - 1) / 2.0f;
        constexpr auto inputGaussianValueAtEnds = 0.000001f;
        const auto inputGaussianWidth = std::sqrt (-std::log (inputGaussianValueAtEnds)) / inputGaussianMidpoint;

        createGaussian (input, 1.0f, inputGaussianMidpoint, inputGaussianWidth);

        for (auto speedRatio : { 0.4, 0.8263, 1.0, 1.05, 1.2384, 1.6 })
        {
            const auto expectedGaussianMidpoint = (inputGaussianMidpoint + InterpolatorType::getBaseLatency()) / (float) speedRatio;
            const auto expectedGaussianWidth = inputGaussianWidth * (float) speedRatio;

            const auto outputBufferSize = (size_t) std::floor ((float) input.size() / speedRatio);

            for (int numBlocks : { 1, 5 })
            {
                const auto inputBlockSize = (float) input.size() / (float) numBlocks;
                const auto outputBlockSize = (int) std::floor (inputBlockSize / speedRatio);

                std::vector<float> output (outputBufferSize, std::numeric_limits<float>::min());

                beginTest (interpolatorName + " process " + String (numBlocks) + " blocks ratio " + String (speedRatio));

                interpolator.reset();

                {
                    auto* inputPtr = input.data();
                    auto* outputPtr = output.data();

                    for (int i = 0; i < numBlocks; ++i)
                    {
                        auto numInputSamplesRead = interpolator.process (speedRatio, inputPtr, outputPtr, outputBlockSize);
                        inputPtr += numInputSamplesRead;
                        outputPtr += outputBlockSize;
                    }
                }

                expectWithinAbsoluteError (findGaussianPeak (output), expectedGaussianMidpoint, 0.1f);

                std::vector<float> expectedOutput (output.size());
                createGaussian (expectedOutput, 1.0f, expectedGaussianMidpoint, expectedGaussianWidth);

                expectAllElementsWithin (output, expectedOutput, 0.02f);

                beginTest (interpolatorName + " process adding " + String (numBlocks) + " blocks ratio " + String (speedRatio));

                interpolator.reset();

                constexpr float addingGain = 0.7384f;

                {
                    auto* inputPtr = input.data();
                    auto* outputPtr = output.data();

                    for (int i = 0; i < numBlocks; ++i)
                    {
                        auto numInputSamplesRead = interpolator.processAdding (speedRatio, inputPtr, outputPtr, outputBlockSize, addingGain);
                        inputPtr += numInputSamplesRead;
                        outputPtr += outputBlockSize;
                    }
                }

                expectWithinAbsoluteError (findGaussianPeak (output), expectedGaussianMidpoint, 0.1f);

                std::vector<float> additionalOutput (output.size());
                createGaussian (additionalOutput, addingGain, expectedGaussianMidpoint, expectedGaussianWidth);
                FloatVectorOperations::add (expectedOutput.data(), additionalOutput.data(), (int) additionalOutput.size());

                expectAllElementsWithin (output, expectedOutput, 0.02f);
            }

            beginTest (interpolatorName + " process wrap 0 ratio " + String (speedRatio));

            std::vector<float> doubleLengthOutput (2 * outputBufferSize, std::numeric_limits<float>::min());

            interpolator.reset();
            interpolator.process (speedRatio, input.data(), doubleLengthOutput.data(), (int) doubleLengthOutput.size(),
                                  (int) input.size(), 0);

            std::vector<float> expectedDoubleLengthOutput (doubleLengthOutput.size());
            createGaussian (expectedDoubleLengthOutput, 1.0f, expectedGaussianMidpoint, expectedGaussianWidth);

            expectAllElementsWithin (doubleLengthOutput, expectedDoubleLengthOutput, 0.02f);

            beginTest (interpolatorName + " process wrap double ratio " + String (speedRatio));

            interpolator.reset();
            interpolator.process (speedRatio, input.data(), doubleLengthOutput.data(), (int) doubleLengthOutput.size(),
                                  (int) input.size(), (int) input.size());

            std::vector<float> secondGaussian (doubleLengthOutput.size());
            createGaussian (secondGaussian, 1.0f, expectedGaussianMidpoint + (float) outputBufferSize, expectedGaussianWidth);
            FloatVectorOperations::add (expectedDoubleLengthOutput.data(), secondGaussian.data(), (int) expectedDoubleLengthOutput.size());

            expectAllElementsWithin (doubleLengthOutput, expectedDoubleLengthOutput, 0.02f);
        }
    }

public:
    void runTest() override
    {
        runInterplatorTests<WindowedSincInterpolator> ("WindowedSincInterpolator");
        runInterplatorTests<LagrangeInterpolator>     ("LagrangeInterpolator");
        runInterplatorTests<CatmullRomInterpolator>   ("CatmullRomInterpolator");
        runInterplatorTests<LinearInterpolator>       ("LinearInterpolator");
    }
};

static InterpolatorTests interpolatorTests;

#endif

} // namespace juce
