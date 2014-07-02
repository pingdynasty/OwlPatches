declare name "Overdrive";
declare category "Distortion";

import("music.lib");
import("guitarix.lib");

drive = vslider("Drive[style:knob][OWL:PARAMETER_A]", 1, 1, 20, 0.1);
f = drive * -0.5 : db2linear : smoothi(0.999);

wet = vslider("Wet/Dry[tooltip:percentage of processed signal in output signal][style:knob][OWL:PARAMETER_D]",  100, 0, 100, 1) : /(100);
dry = 1 - wet;

overdrive(x) = (x*(abs(x) + drive)/(x*x + (drive-1)*abs(x) + 1)) * f;


process =  _<:*(dry),(*(wet) : overdrive):>_;
