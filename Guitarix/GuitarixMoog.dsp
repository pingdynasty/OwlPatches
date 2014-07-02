declare id      "moog";
declare name    "Moog Filter";
declare category "Tone control";
declare license "BSD";


import("effect.lib");
import("guitarix.lib");

fr 		= hslider("Freq[style:knob][OWL:PARAMETER_A]", 3000, 440, 6000, 10): smoothi(0.999);
Q 		= hslider("Q[style:knob][OWL:PARAMETER_B]", 1, 0, 4, 0.1);

process = ( +(anti_denormal_ac): moogvcfN(Q,fr)), (+(anti_denormal_ac): moogvcfN(Q,fr));
