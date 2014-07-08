declare id "HighShelf";

import("maxmsp.lib");

G = hslider("Gain [unit:dB][style:knob][OWL:PARAMETER_A]", 0, -10, 10, 0.1);
F = hslider("Freq [style:knob][OWL:PARAMETER_B]", 1000, 100, 20000, 1);
Q = hslider("Q [style:knob][OWL:PARAMETER_C]", 1, 0.01, 100, 0.01);

process(x) = highShelf(x,F,G,Q);
