declare name "qompander";
declare version "1.2";
declare author "Bart Brouns";
declare license "GNU 3.0";
declare copyright "(c) Bart Brouns 2014";
declare credits "ported from qompander in pd by Katja Vetter";
declare see "http://www.katjaas.nl/compander/compander.html";
declare additional "filter coefficients by Olli Niemitalo";

//-----------------------------------------------
// imports
//-----------------------------------------------

import ("effect.lib");

//-----------------------------------------------
// the GUI
//-----------------------------------------------
qompanderGroup(x) = (vgroup("[0] qompander [tooltip: Reference: http://www.katjaas.nl/compander/compander.html]", x));
factor = qompanderGroup(hslider("[0] factor[unit:: 1][style:knob][OWL:PARAMETER_A]", 3, 0.8, 8, 0.01):smooth(0.999));
threshold = qompanderGroup(hslider("[1] threshold [unit: dB][style:knob][OWL:PARAMETER_B]", -40, -96, -20, 0.01):smooth(0.999));
attack = qompanderGroup(hslider("[2] attack[unit: ms][style:knob][OWL:PARAMETER_C]", 1, 1, 20, 0.01):smooth(0.999));
release = qompanderGroup(hslider("[3] release[unit: ms][style:knob][OWL:PARAMETER_D]", 20, 20, 1000, 0.01):smooth(0.999));

//-----------------------------------------------
// the DSP
//-----------------------------------------------

magnitude = (threshold):db2linear;
exponent = log(magnitude)/log(sin(factor*magnitude*PI/2));

// to go from puredata biquad coefficients to max/msp and faust notation: the first two parameters are negated and put last
olli1(x) = x: tf2(0.161758, 0, -1, 0, -0.161758):tf2(0.733029, 0, -1, 0, -0.733029):tf2(0.94535 , 0, -1, 0, -0.94535 ):tf2(0.990598, 0, -1, 0, -0.990598);
olli2(x) = x:mem: tf2(0.479401, 0, -1, 0, -0.479401):tf2(0.876218, 0, -1, 0, -0.876218):tf2(0.976599, 0, -1, 0, -0.976599):tf2(0.9975 , 0, -1, 0, -0.9975 );

pyth(x) = sqrt((olli1(x)*olli1(x))+(olli2(x)*olli2(x))):max(0.00001):min(100); //compute instantaneous amplitudes
attackDecay(x) = pyth(x) :amp_follower_ud(attack/1000,release/1000);
mapping(x) = attackDecay(x) : ((sin((min(1/factor)*(factor/4)) * (2*PI)): max(0.0000001):min(1),exponent) : pow );
qompander(x) = (mapping(x) / attackDecay(x))<: (_,olli1(x):*),(_,olli2(x):*):+:_*(sqrt(0.5));
process(x) = qompander(x);