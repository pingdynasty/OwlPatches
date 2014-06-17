/* Help:
 REGISTER_PATCH(yourHppFileName,
                "NameThatWillBeDisplayed",
                numberInputChannels,
                numberOutputChannels);
 
 The first two registered patch are the default green and red patches in compiled the firmware.
 
*/

REGISTER_PATCH(DroneBoxPatch, "Contest/DroneBox", 1, 1);
REGISTER_PATCH(FreeVerbPatch, "FreeVerb", 1, 1);
REGISTER_PATCH(SimpleDelayPatch, "Simple Delay", 1, 1);
REGISTER_PATCH(GainPatch, "Gain", 2, 2);
REGISTER_PATCH(CompressorPatch, "Compressor", 2, 2);
REGISTER_PATCH(ParametricEqPatch, "Parametric EQ", 1, 1);
REGISTER_PATCH(OverdrivePatch, "Overdrive", 2, 2);
REGISTER_PATCH(PhaserPatch, "Phaser", 1, 1);
REGISTER_PATCH(StateVariableFilterPatch, "State Variable Filter", 1, 1);
REGISTER_PATCH(ResonantFilterPatch, "Resonant Low Pass Filter", 1, 1);
REGISTER_PATCH(LeakyIntegratorPatch, "Leaky Integrator", 1, 1);
REGISTER_PATCH(OctaveDownPatch, "Octave Pitch Shifter", 1, 1);
REGISTER_PATCH(StereoMixerPatch, "Stereo Mixer", 2, 2);
REGISTER_PATCH(VibroFlangePatch, "Vibro-Flange", 1, 1);
REGISTER_PATCH(RingModulatorPatch, "Ring Modulator", 1, 1);
REGISTER_PATCH(SynthPatch, "Synthesizer", 1, 1);
REGISTER_PATCH(FourBandsEqPatch, "FourBandsEqPatch", 1, 1);
REGISTER_PATCH(BiasedDelayPatch, "Contest/BiasedDelayPatch", 2, 2);
REGISTER_PATCH(little_blo_bleep, "Contest/blo bleep", 2, 2);
REGISTER_PATCH(BiasPatch, "Contest/Bias", 2, 2);
REGISTER_PATCH(BitH8rPatch, "Contest/BitH8r", 2, 2);
REGISTER_PATCH(ConnyPatch, "Contest/ConnyPatch", 2, 2);
REGISTER_PATCH(DualTremoloPatch, "Contest/DualTremolo", 2, 2);
REGISTER_PATCH(MdaBandistoPatch, "mdaPorts/MdaBandisto", 2, 2);
REGISTER_PATCH(MdaStereoPatch, "mdaPorts/MdaStereo", 2, 2);
REGISTER_PATCH(MdaTransientPatch, "mdaPorts/MdaTransient", 2, 2);
REGISTER_PATCH(QompressionPatch, "Qompression", 2, 2);
REGISTER_PATCH(PsycheFilterPatch, "Psyche Filter", 2, 2);
//REGISTER_PATCH(PlateReverbPatch, "PlateReverb", 2, 2);


/*
TO BE WORKED ON
*/
// REGISTER_PATCH(JotReverbPatch,"JotReverbPatch", 2, 2);
// REGISTER_PATCH(SimpleDriveDelayPatch, "Drive Delay", 1, 1);
// REGISTER_PATCH(AutotalentPatch, "AutoTalent", 2, 2);
// REGISTER_PATCH(EnvelopeFilterPatch, "Envelope Filter", 1, 1);
// REGISTER_PATCH(TemplatePatch, "Template", 0, 0);
// REGISTER_PATCH(JumpDelay, "Contest/JumpDelay", 0, 0);
// REGISTER_PATCH(SampleJitterPatch, "Contest/SampleJitterPatch", 0, 0);
// REGISTER_PATCH(SirenPatch, "Contest/SirenPatch", 0, 0);
// REGISTER_PATCH(LpfDelayPatch, "Low Pass Filtered Delay", 1, 1);
// REGISTER_PATCH(LpfDelayPhaserPatch, "Low Pass Filtered Delay with Phaser", 1, 1);
// REGISTER_PATCH(WaveshaperPatch, "Waveshaper", 2, 2);
// REGISTER_PATCH(TestTonePatch, "Test Tone", 0, 0);
// REGISTER_PATCH(FlangerPatch, "Flanger", 0, 0);
