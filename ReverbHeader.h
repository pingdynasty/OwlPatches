
#ifndef __ReverbHeader_h__
#define __ReverbHeader_h__


class comb {
public:
    comb() {
        filterstore = 0;
        bufidx = 0;
    }
    
    void  setbuffer(float *buf, int size) {
        buffer = buf;
        bufsize = size;
    }
    
    inline  float process(float input) {
        float output;
        output = buffer[bufidx];
        //    undenormalise(output);
        filterstore = (output*damp2) + (filterstore*damp1);
        //    undenormalise(filterstore);
        buffer[bufidx] = input + (filterstore*feedback);
        if(++bufidx>=bufsize)
            bufidx = 0;
        return output;
    }
    
    void  mute() {
        for (int i=0; i<bufsize; i++)
            buffer[i]=0;
    }
	
    void  setdamp(float val) {
        damp1 = val;
        damp2 = 1-val;
    }
	
    float getdamp() {
        return damp1;
    }
	
    void  setfeedback(float val) {
        feedback = val;
    }
	
    float getfeedback() {
        return feedback;
    }
    
private:
    float feedback;
    float filterstore;
    float damp1;
    float damp2;
    float *buffer;
    int   bufsize;
    int   bufidx;
};

class allpass {
public:
    allpass() {
        bufidx = 0;
    }
    void  setbuffer(float *buf, int size) {
        buffer = buf;
        bufsize = size;
    }
    
    inline  float process(float input) {
        float output;
        float bufout;
        
        bufout = buffer[bufidx];
        //    undenormalise(bufout);
        
        output = -input + bufout;
        buffer[bufidx] = input + (bufout*feedback);
        
        if(++bufidx>=bufsize) bufidx = 0;
        
        return output;
    }
    
    void  mute() {
        for (int i=0; i<bufsize; i++)
            buffer[i]=0;
    }
    void  setfeedback(float val) {
        feedback = val;
    }
    float getfeedback() {
        return feedback;
    }
    
    
    float feedback;
    float *buffer;
    int   bufsize;
    int   bufidx;
};

const float fixedgain		= 0.015f;
const float scalewet		= 5;
const float scaledry		= 2;
const float scaledamp		= 0.4f;
const float scaleroom		= 0.28f;
const float offsetroom		= 0.7f;
const float initialroom		= 0.5f;
const float initialdamp		= 0.5f;
const float initialwet		= 1/scalewet;
const float initialdry		= 0;

#endif
