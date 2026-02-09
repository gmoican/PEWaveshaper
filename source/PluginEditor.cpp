#include "PluginEditor.h"

PluginEditor::PluginEditor (EnvelParamWaveshaperProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p)
{
    juce::ignoreUnused (processorRef);
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLnF);
    
    // --- LAYOUT ---
    header.setColour (juce::TextButton::buttonColourId, punk_dsp::UIConstants::background.brighter(0.5f)
                                                                               .withAlpha(0.25f)
                      );
    header.setEnabled(false);
    header.setColour(juce::TextButton::textColourOffId, punk_dsp::UIConstants::highlight);
    header.setColour(juce::TextButton::textColourOnId, punk_dsp::UIConstants::highlight);
    header.setButtonText ("Punk DSP - EnvelopeFollower + ParamtricWaveshaper");
    addAndMakeVisible (header);
    
    params.setColour (juce::TextButton::buttonColourId, punk_dsp::UIConstants::background.brighter(0.5f)
                                                                                         .withAlpha(0.25f)
                      );
    params.setEnabled(false);
    addAndMakeVisible (params);
    
    // Drive
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setRange(Parameters::driveMin, Parameters::driveMax, 0.1);
    driveSlider.setValue(Parameters::driveDefault);
    driveSlider.setName("Drive");
    addAndMakeVisible(driveSlider);
    
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::driveId, driveSlider);
    
    // Output Gain
    outGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setRange(Parameters::outGainMin, Parameters::outGainMax, 0.1);
    outGainSlider.setValue(Parameters::outGainDefault);
    outGainSlider.setName("Out");
    addAndMakeVisible(outGainSlider);
    
    outGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::outGainId, outGainSlider);
    
    // Bias (Pre-Drive)
    biasPreSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    biasPreSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    biasPreSlider.setRange(Parameters::biasMin, Parameters::biasMax, 0.01);
    biasPreSlider.setValue(Parameters::biasDefault);
    biasPreSlider.setName(u8"β\u2080");
    addAndMakeVisible(biasPreSlider);
    
    biasPreAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::preBiasId, biasPreSlider);

    // Bias (Post-Drive)
    biasPostSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    biasPostSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    biasPostSlider.setRange(Parameters::biasMin, Parameters::biasMax, 0.01);
    biasPostSlider.setValue(Parameters::biasDefault);
    biasPostSlider.setName(u8"β\u2081");
    addAndMakeVisible(biasPostSlider);
    
    biasPostAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::postBiasId, biasPostSlider);
    
    // Param Positive Limit
    paramPosSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    paramPosSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    paramPosSlider.setRange(Parameters::paramPosMin, Parameters::paramPosMax, 0.1);
    paramPosSlider.setValue(Parameters::paramPosDefault);
    paramPosSlider.setName(u8"+α");
    addAndMakeVisible(paramPosSlider);
    
    paramPosAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::paramPosId, paramPosSlider);
    
    // Param Negative Limit
    paramNegSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    paramNegSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    paramNegSlider.setRange(Parameters::paramNegMin, Parameters::paramNegMax, 0.1);
    paramNegSlider.setValue(Parameters::paramNegDefault);
    paramNegSlider.setName(u8"-α");
    addAndMakeVisible(paramNegSlider);
    
    paramNegAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::paramNegId, paramNegSlider);

    // Attack Time (ms)
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackSlider.setRange(Parameters::attackMin, Parameters::attackMax, 0.1);
    attackSlider.setValue(Parameters::attackDefault);
    attackSlider.setName("Att");
    addAndMakeVisible(attackSlider);
    
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::attackId, attackSlider);

    // Release Time (ms)
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    releaseSlider.setRange(Parameters::releaseMin, Parameters::releaseMax, 0.1);
    releaseSlider.setValue(Parameters::releaseDefault);
    releaseSlider.setName("Rel");
    addAndMakeVisible(releaseSlider);
    
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::releaseId, releaseSlider);

    // Mix (%)
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setRange(Parameters::mixMin, Parameters::mixMax, 1.0);
    mixSlider.setValue(Parameters::mixDefault);
    mixSlider.setName("Mix");
    addAndMakeVisible(mixSlider);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::mixId, mixSlider);
    
    // Sizing calculations
    const int numCols = 3;
    const int numRows = 3;

    const int totalWidth = (numCols * (punk_dsp::UIConstants::knobSize + 2 * punk_dsp::UIConstants::margin)) + (10 * 2);
    const int totalHeight = punk_dsp::UIConstants::headerHeight + (numRows * (punk_dsp::UIConstants::knobSize + 2 * punk_dsp::UIConstants::margin)) + (10 * 2);
    
    setSize (totalWidth, totalHeight);
}

PluginEditor::~PluginEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (punk_dsp::UIConstants::background);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    
    // --- LAYOUT SETUP ---
    auto headerArea = area.removeFromTop( punk_dsp::UIConstants::headerHeight );
    auto paramsArea = area.reduced( 10 );
    
    header.setBounds(headerArea);
    params.setBounds(paramsArea);
    
    // --- PARAMS LAYOUT ---
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.flexWrap = juce::FlexBox::Wrap::wrap;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    fb.alignContent = juce::FlexBox::AlignContent::spaceBetween;
    
    // Add sliders to the FlexBox
    fb.items.add(juce::FlexItem(driveSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                            .withMinHeight(punk_dsp::UIConstants::knobSize)
                                            .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(outGainSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                              .withMinHeight(punk_dsp::UIConstants::knobSize)
                                              .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(paramPosSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                               .withMinHeight(punk_dsp::UIConstants::knobSize)
                                               .withMargin(punk_dsp::UIConstants::margin));

    fb.items.add(juce::FlexItem(biasPreSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                              .withMinHeight(punk_dsp::UIConstants::knobSize)
                                              .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(biasPostSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                               .withMinHeight(punk_dsp::UIConstants::knobSize)
                                               .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(paramNegSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                               .withMinHeight(punk_dsp::UIConstants::knobSize)
                                               .withMargin(punk_dsp::UIConstants::margin));
    
    fb.items.add(juce::FlexItem(attackSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                             .withMinHeight(punk_dsp::UIConstants::knobSize)
                                             .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(releaseSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                              .withMinHeight(punk_dsp::UIConstants::knobSize)
                                              .withMargin(punk_dsp::UIConstants::margin));
    fb.items.add(juce::FlexItem(mixSlider).withMinWidth(punk_dsp::UIConstants::knobSize)
                                          .withMinHeight(punk_dsp::UIConstants::knobSize)
                                          .withMargin(punk_dsp::UIConstants::margin));
    
    // Perform the layout
    fb.performLayout(paramsArea);
}