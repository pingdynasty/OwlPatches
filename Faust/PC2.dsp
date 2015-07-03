declare name "Boss PC-2";
declare description "Boss PC-2 emulator";
declare author "Oli Larkin (contact@olilarkin.co.uk)";
declare copyright "Oliver Larkin";
declare version "0.1";
declare licence "GPL";

//import("math.lib"); 
//import("maxmsp.lib"); 
import("music.lib"); 
import("oscillator.lib"); 
//import("reduce.lib"); 
//import("filter.lib"); 
import("effect.lib"); 


basepitch = hslider("BasePitch [unit:semitones] [OWL:PARAMETER_A]", 60, 24, 96, 0.01) : smooth(tau2pole(0.01));
pitchmod = hslider("PitchMod [unit:semitones] [OWL:PARAMETER_B]", 24, -64, 64, 1) : smooth(tau2pole(0.005));
attack = hslider("Attack [unit:ms] [OWL:PARAMETER_C]", 1, 0, 1000, 1) : *(0.001) : max(1.0/float(SR));
release = hslider("Release [unit:ms] [OWL:PARAMETER_D]", 20, 0, 1000, 1) : *(0.001) : max(1.0/float(SR));
process= _,_ : pc2, _
with {
 pc2 = amp_follower_ud(attack, release) <: (midikey2hz(basepitch + (pitchmod * _)): triangle), _ : *;
};