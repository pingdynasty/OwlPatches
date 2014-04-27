#include "GainPatch.hpp"

#include "Autotalent/AutotalentPatch.hpp"

#include "SimpleDelayPatch.hpp"

#include "ParametricEqPatch.hpp"
#include "OverdrivePatch.hpp"
#include "PhaserPatch.hpp"
#include "StateVariableFilterPatch.hpp"
#include "ResonantFilterPatch.hpp"

#include "LeakyIntegratorPatch.hpp"

#include "FreeVerbPatch.hpp"
#include "OctaveDownPatch.hpp"

#include "StereoMixerPatch.hpp"
#include "Contest/blo_bleep.hpp"
#include "Contest/BiasPatch.hpp"
#include "Contest/BitH8rPatch.hpp"
#include "Contest/ConnyPatch.hpp"
#include "Contest/DroneBox.hpp"
#include "Contest/DualTremoloPatch.hpp"
#include "VibroFlangePatch.hpp"
#include "RingModulatorPatch.hpp"
#include "SynthPatch.hpp"
#include "FourBandsEqPatch.hpp"
#include "EnvelopeFilterPatch.hpp"
#include "JotReverbPatch.hpp"
#include "mdaPorts/MdaBandistoPatch.cpp"
#include "mdaPorts/MdaStereoPatch.cpp"
#include "mdaPorts/MdaTransientPatch.cpp"
#include "Qompression.hpp"
#include "PsycheFilter.hpp"
#include "RyanPatch.hpp"

#include "Contest/BiasedDelayPatch.hpp" 

// #include "EnvelopeFilterPatch.hpp" /* too cpu intensive (sqrt...) */
// #include "TemplatePatch.hpp"
// #include "Contest/JumpDelay.hpp" /* uses calloc and free */
// #include "Contest/SampleJitterPatch.hpp" /* requires juce::Random */
// #include "Contest/SirenPatch.hpp" /* causes assert_failed in DMA_GetFlagStatus() */
// #include "LpfDelayPatch.hpp" /* not compatible with Windows yet */
// #include "LpfDelayPhaserPatch.hpp" /* not compatible with Windows yet */
// #include "WaveshaperPatch.hpp" /* not compatible with Windows yet */
// #include "TestTonePatch.hpp"
// #include "FlangerPatch.hpp"
