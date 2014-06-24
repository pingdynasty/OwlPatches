declare id   "phaser_mono";
declare name "Phaser Mono";
declare category "Modulation";

import("effect.lib");


phaser_monogx = *(level): component("phaser.dsp").phaser_mono(Notches,0,width,frqmin,fratio,frqmax,freq,depth,fb,invert)
with {
  Notches = 4;
  freq  = hslider("Speed [unit:Hz][style:knob][OWL:PARAMETER_A] ", 0.5, 0, 5, 0.005);
  
  depth  = hslider("depth [style:knob][OWL:PARAMETER_B] ", 1, 0, 2, 0.01);
  fb	 = hslider("feedback gain[style:knob][OWL:PARAMETER_C]", 0, 0, 1, 0.01);
  width  = 1000 ;//hslider("Notch width [unit:Hz]", 1000, 10, 5000, 1);
  vibr   = 1 ;//checkbox("VibratoMode[enum:direct | vibrato]");
  frqmin = 100 ;//hslider("MinNotch1Freq [unit:Hz] ", 100, 20, 5000, 1);
  frqmax = 800: max(frqmin) ;//hslider("MaxNotch1Freq [unit:Hz] ", 800, 20, 10000, 1) : max(frqmin);
  fratio = 2.0 ;//hslider("NotchFreq", 1.5, 1.1, 4, 0.01);
  //mdepth = 2 ;//mdepth = select2(vibr,depth,2); 
  
  invert   = 1 ;//checkbox("invert[enum:linear|invert]");
  level	   = 1 ;//hslider("level [unit:dB][style:knob][OWL:PARAMETER_C]", 0, -60, 10, 0.1) : db2linear;
  
};

wet = vslider("dry/wet[name:wet/dry][tooltip:percentage of processed signal in output signal][style:knob][OWL:PARAMETER_D]",  100, 0, 100, 1) : /(100);
dry = 1 - wet;

process =  _<:*(dry),(*(wet): phaser_monogx ):>_;
