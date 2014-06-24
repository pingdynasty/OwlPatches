
DSP_SRC = $(wildcard *.dsp)
CPP_OBJS = $(DSP_SRC:%.dsp=%Patch.hpp)
SVG_OBJS = $(DSP_SRC:%.dsp=%-svg)

all: $(CPP_OBJS)

svg: $(SVG_OBJS)

%Patch.hpp : %.dsp
	faust2owl $<

%-svg : %.dsp
	faust2svg $<
#	open $@/process.svg

clean:
	rm $(CPP_OBJS)
	rm -r $(SVG_OBJS)
