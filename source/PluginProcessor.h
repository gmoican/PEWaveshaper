#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "punk_dsp/punk_dsp.h"

#if (MSVC)
#include "ipps.h"
#endif

namespace Parameters
{
    // In Gain
    constexpr auto driveId = "drive";
    constexpr auto driveName = "Drive (dB)";
    constexpr auto driveDefault = 0.0f;
    constexpr auto driveMin = -30.0f;
    constexpr auto driveMax = 60.0f;

    // Out Gain
    constexpr auto outGainId = "outGain";
    constexpr auto outGainName = "Output Gain (dB)";
    constexpr auto outGainDefault = 0.0f;
    constexpr auto outGainMin = -30.0f;
    constexpr auto outGainMax = 30.0f;

    // Bias (Pre in-gain) & (Post in-gain)
    constexpr auto preBiasId = "biasPre";
    constexpr auto preBiasName = "Bias (Pre)";
    constexpr auto postBiasId = "biasPost";
    constexpr auto postBiasName = "Bias (Post)";
    constexpr auto biasDefault = 0.0f;
    constexpr auto biasMin = -1.0f;
    constexpr auto biasMax = 1.0f;

    // Param limits
    constexpr auto envelAddId = "envel_add";
    constexpr auto envelAddName = "EnvelAdd";
    constexpr auto envelAddDefault = 0.2f;
    constexpr auto envelAddMin = 0.0f;
    constexpr auto envelAddMax = 1.0f;

    constexpr auto envelMulId = "envel_mul";
    constexpr auto envelMulName = "EnvelMul";
    constexpr auto envelMulDefault = 1.f;
    constexpr auto envelMulMin = 0.0f;
    constexpr auto envelMulMax = 2.0f;

    // Time variables
    constexpr auto attackId = "attack";
    constexpr auto attackName = "Attack";
    constexpr auto attackDefault = 0.8f;
    constexpr auto attackMin = 0.1f;
    constexpr auto attackMax = 300.0f;

    constexpr auto releaseId = "release";
    constexpr auto releaseName = "Release";
    constexpr auto releaseDefault = 50.f;
    constexpr auto releaseMin = 1.0f;
    constexpr auto releaseMax = 300.0f;

    // Harmonics Gain
    constexpr auto mixId = "mix";
    constexpr auto mixName = "Mix";
    constexpr auto mixDefault = 100.0f;
    constexpr auto mixMin = 0.0f;
    constexpr auto mixMax = 100.0f;
}

class EnvelParamWaveshaperProcessor : public juce::AudioProcessor
{
public:
    EnvelParamWaveshaperProcessor();
    ~EnvelParamWaveshaperProcessor() override;

    // DEFAULT STUFF ===============================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // ===== MY STUFF ===============================================================
    juce::AudioProcessorValueTreeState apvts;
    
    void updateParameters();

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
    punk_dsp::ParametricWaveshaper ws;
    punk_dsp::EnvelopeFollower ef;

    float envelope_multiplier   { 1.5f };
    float envelope_adder        { 0.0f };
    
    // =============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelParamWaveshaperProcessor)
};
