import("effect.lib"); 

monowah = autowah(level) with {
   level = min(1, hslider("Wah[OWL:PARAMETER_D]",0.6,0,1,0.0001) +
           	  hslider("Super Wah[OWL:PARAMETER_E]",0.0,0,0.8,0.0001));
};

process =  monowah*gain	with {
   gain = hslider("Gain[OWL:PARAMETER_A]",1.0,0,2,0.0001);
};	

