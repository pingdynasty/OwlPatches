declare id 		"flanger_mono_gx";
declare name            "Flanger GX";
declare category        "Modulation";

h_flanger(dmax, curdel, fb, wet) = _ <: _, (dly : (+:dly)~*(fb)) : mix with {
    dly = component("music.lib").fdelay(dmax/2, curdel/2);
    mix = *(2-wet), _*(wet) : +;
};

flanger(dmax, curdel, fb, wet) = _ <: _, (+:dly)~*(fb) : mix with {
    dly = component("music.lib").fdelay(dmax, curdel);
    mix = *(2-wet), _*(wet) : +;
};

process = h_flanger(dmax, curdel,  fb, wet1) :
          flanger(  dmax, curdel, -fb, wet2) :
          *(0.25) with {
    SR = component("math.lib").SR;
    dflange = 0.01 * SR; 
    //dflange = 0.001 * SR *  hslider("Width[name:Width]", 5.0, 0, 10, 0.01);
    freq = hslider("Freq[log][name:Speed][unit:Hz][style:knob][OWL:PARAMETER_A]", 0.2, 0.05, 10, 0.01);
    odflange = 0.001 * SR *  hslider("Depth[name:Depth][style:knob][OWL:PARAMETER_B]", 0.5, 0, 5, 0.01);
    dmax = 2048; // > max(dflange+odflange) at rate 96000
    fb = hslider("Feedback[name:Feedback][style:knob][OWL:PARAMETER_C]", -0.707, -0.99, 0.99, 0.01) * (2+abs(mix))/3;
    lfo = component("oscillator.lib").oscrs;
    curdel = odflange+dflange*(1+lfo(freq))/2;
    //curdel = hslider("Delay", 20, 0, 400, 0.1);
    wet = hslider("Dry/Wet[name:Wet][style:knob][OWL:PARAMETER_D]", 100, 0, 100, 1)/100;
    mix = 1; //mix = hslider("Mix[name:Mix]", 0, -1, 1, 0.1);
    wet1 = wet*min(1,1+mix);
    wet2 = wet*min(1,1-mix);
};
