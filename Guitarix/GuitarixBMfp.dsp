declare id "bmpf";
declare name "BigMuffFuzzPedal";
declare shortname "FuzzPedal";
declare category "Distortion";
declare description "BigMuffFuzzPedal";


import("guitarix.lib");
import("filter.lib");
import("effect.lib");
import("reduce.lib");


bigmuff      = _<: filter1,filter2:>_   with {
	tone     = vslider("tone[style:knob][OWL:PARAMETER_D]",0.5,0,1,0.01);
	filter1  = highpass( 1, 1856):*(tone);
	filter2  = lowpass( 1, 408 ) :*(1-tone);
};

process = _<:*(dry),(*(wet):*(gain):bigmuff:fuzz:fuzzy:fiz):>downfilter with {
    //fuzz(x)      = x-0.15*x^2-0.15*x^3;
    //fuzz(x)      = 1.5*x-0.5*x^3;
    fuzz(x)      = (1+drive/101)*x/(1+drive/101*abs(x));
    drive        = vslider("drive[style:knob][OWL:PARAMETER_C]", 1, -3, 100, 1);
    fuzzy        = fuzzy_tube(2,1,0.5,drive);
    fiz(x)       = x+(x^7);
    downfilter   = lowpass(1,5631): highpass(1,80);
    gain         = vslider("Input[style:knob][OWL:PARAMETER_A]",0,-24,20,0.1) : db2linear : smoothi(0.999);
    wet          = vslider("Output[style:knob][OWL:PARAMETER_B]",  100, 50, 100, 1) : /(100);
    dry          = 1 - wet;

};
