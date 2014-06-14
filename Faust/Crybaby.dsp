import("effect.lib"); 

process = crybaby(wah) with {
//   wah = hslider("[1] AhAh [OWL:PARAMETER_A]",0.6,0,1,0.01) : automat(360, 10, 0.0); // 4246 ARM cycles
   wah = hslider("[1] AhAh [OWL:PARAMETER_A]",0.6,0,1,0.01); // 441 ARM cycles
};
