#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EnvelParamWaveshaperProcessor::EnvelParamWaveshaperProcessor()
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ), apvts(*this, nullptr, "Parameters", createParams())
{
}

EnvelParamWaveshaperProcessor::~EnvelParamWaveshaperProcessor()
{
}

//==============================================================================
const juce::String EnvelParamWaveshaperProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EnvelParamWaveshaperProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool EnvelParamWaveshaperProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool EnvelParamWaveshaperProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double EnvelParamWaveshaperProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EnvelParamWaveshaperProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int EnvelParamWaveshaperProcessor::getCurrentProgram()
{
    return 0;
}

void EnvelParamWaveshaperProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String EnvelParamWaveshaperProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void EnvelParamWaveshaperProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

// =========== PARAMETER LAYOUT ====================
juce::AudioProcessorValueTreeState::ParameterLayout EnvelParamWaveshaperProcessor::createParams()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Input Gain (dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::driveId,
                                                           Parameters::driveName,
                                                           juce::NormalisableRange<float>(Parameters::driveMin, Parameters::driveMax, 0.1f),
                                                           Parameters::driveDefault
                                                           )
               );
    
    // Output Gain (dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::outGainId,
                                                           Parameters::outGainName,
                                                           juce::NormalisableRange<float>(Parameters::outGainMin, Parameters::outGainMax, 0.1f),
                                                           Parameters::outGainDefault
                                                           )
               );

    // Attack time
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::attackId,
                                                           Parameters::attackName,
                                                           juce::NormalisableRange<float>(Parameters::attackMin, Parameters::attackMax, 0.1f),
                                                           Parameters::attackDefault
                                                           )
              );
    
    // Release time
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::releaseId,
                                                           Parameters::releaseName,
                                                           juce::NormalisableRange<float>(Parameters::releaseMin, Parameters::releaseMax, 0.1f),
                                                           Parameters::releaseDefault
                                                           )
              );
    
    // Bias (Pre in-gain)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::preBiasId,
                                                           Parameters::preBiasName,
                                                           juce::NormalisableRange<float>(Parameters::biasMin, Parameters::biasMax, 0.01f),
                                                           Parameters::biasDefault
                                                           )
               );
    
    // Bias (Post in-gain)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::postBiasId,
                                                           Parameters::postBiasName,
                                                           juce::NormalisableRange<float>(Parameters::biasMin, Parameters::biasMax, 0.01f),
                                                           Parameters::biasDefault
                                                           )
               );
    
    // Parameter limits
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::envelAddId,
                                                           Parameters::envelAddName,
                                                           juce::NormalisableRange<float>(Parameters::envelAddMin, Parameters::envelAddMax, 0.01f),
                                                           Parameters::envelAddDefault
                                                           )
               );
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::envelMulId,
                                                           Parameters::envelMulName,
                                                           juce::NormalisableRange<float>(Parameters::envelMulMin, Parameters::envelMulMax, 0.01f),
                                                           Parameters::envelMulDefault
                                                           )
               );
    
    // Mix (%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::mixId,
                                                           Parameters::mixName,
                                                           juce::NormalisableRange<float>(Parameters::mixMin, Parameters::mixMax, 1.0f),
                                                           Parameters::mixDefault
                                                           )
               );
    
    return layout;
}

//==============================================================================
void EnvelParamWaveshaperProcessor::updateParameters()
{
    ef.setAttack( apvts.getRawParameterValue(Parameters::attackId)->load() );
    ef.setRelease( apvts.getRawParameterValue(Parameters::releaseId)->load() );

    ws.setDrive_dB( apvts.getRawParameterValue(Parameters::driveId)->load() );
    ws.setOutGain_dB( apvts.getRawParameterValue(Parameters::outGainId)->load() );
    
    ws.setBiasPre( apvts.getRawParameterValue(Parameters::preBiasId)->load() );
    ws.setBiasPost( apvts.getRawParameterValue(Parameters::postBiasId)->load() );
    
    envelope_adder = apvts.getRawParameterValue(Parameters::envelAddId)->load();
    envelope_multiplier = apvts.getRawParameterValue(Parameters::envelMulId)->load();
    
    ws.setMix( apvts.getRawParameterValue(Parameters::mixId)->load() );
}

void EnvelParamWaveshaperProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    ef.prepare(spec);
    
    updateParameters();
}

void EnvelParamWaveshaperProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool EnvelParamWaveshaperProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}

void EnvelParamWaveshaperProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                  juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Clear any unused output channel
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Update params
    updateParameters();
    
    // Process
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float envelope = envelope_multiplier * ( ef.process(channelData[sample]) + envelope_adder );
            ws.setParam(envelope);
            channelData[sample] = ws.processSample(channelData[sample]);
        }
    }
}

//==============================================================================
bool EnvelParamWaveshaperProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EnvelParamWaveshaperProcessor::createEditor()
{
    return new PluginEditor (*this);
    // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void EnvelParamWaveshaperProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void EnvelParamWaveshaperProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EnvelParamWaveshaperProcessor();
}
