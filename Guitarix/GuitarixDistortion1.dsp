declare id 		"distortion1";
declare version 	"0.01";
declare author 		"brummer";
declare license 	"BSD";
declare copyright 	"(c)brummer 2008";

import("math.lib");
import("music.lib");
import("effect.lib"); 
import("filter.lib");
import("guitarix.lib");

//----------distortion---------

//-speaker emulation

/*

sbp1    = vslider("low_freq[name:low freq][tooltip:low-freq cutoff Hz]",130,20,1000,10);
sbp2    = vslider("high_freq[name:high freq][tooltip:high-freq cutoff Hz]",5000,1000,12000,10);
switch1 = checkbox("on_off[name:low highcutoff]");
sbp = hgroup("low_highcutoff", bypass(switch1, +(anti_denormal_ac) : speakerbp(sbp1,sbp2)));

*/

//------------------------------ low/high-passfilters --------------------------------------

/* commented out section not in use - needs revisiting, faust compiler says lowpass1 and highpass1 undefined
//-low and highpass

lowpassfreq  	= nentry("low_freq[name:low freq]", 5000, 20, 12000, 10);
highpassfreq 	= nentry("high_freq[name:high freq]", 130, 20, 7040, 10);
switch       	= checkbox("on_off[name:low highpass]");
passo 		 	= +(anti_denormal_ac) : lowpass1(lowpassfreq) : highpass1(highpassfreq);
pass 		 	= hgroup("low_highpass", bypass(switch, passo));

*/

//-distortion
drivelevel      = vslider("Level[style:knob][OWL:PARAMETER_A]", 0.01, 0, 0.5, 0.01);
drivegain1      = vslider("Gain[style:knob][OWL:PARAMETER_B]", 2, -10, 10, 0.1)-10 : db2linear : smoothi(0.999);
drive		= vslider("Drive[style:knob][OWL:PARAMETER_C]", 0.64, 0, 1, 0.01);
distortion 	= cubicnl(drive,drivelevel); 


//-resonator

//switch2       	= checkbox("resonator.on_off[name:resonat]");
resonator = (+ <: (delay(4096, d-1) + delay(4096, d)) / 2) ~ *(1.0-a)
with {
  d = 1; //d = vslider("vibrato", 1, 0, 1, 0.01);
  a = 1; //a = vslider("trigger", 0.12, 0, 1, 0.01);
};


process =  resonator : +(anti_denormal_ac) : add_dc : component("HighShelf.dsp").hs : distortion : *(drivegain1) : component("HighShelf.dsp").hs ;
