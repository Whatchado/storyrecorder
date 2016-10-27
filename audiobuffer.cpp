#include "audiobuffer.h"
//#include <strings.h>
#include <string.h>
#include <stdio.h>
#include "log.h"
AUDIOBUFFER::AUDIOBUFFER()
{
    data=NULL;
    ndata=0;
    position=0;
    samplerate=44100;
    samplesize=2;
}
AUDIOBUFFER::~AUDIOBUFFER()
{
}

void AUDIOBUFFER::setSampleRate(int sr)
{
    samplerate=sr;
};
void AUDIOBUFFER::setSampleSize(int ss)
{
    samplesize=ss;
};
void AUDIOBUFFER::reset()
{
    if(data) free(data);
    data=NULL;
    ndata=0;
    position=0;
};
void AUDIOBUFFER::addAudio(double time, unsigned char *audiodata, unsigned int nsamples)
{
    if(time<0) return;
    int startpos=time*samplerate;
    int requiredsize=(startpos+nsamples)*samplesize;
    if(requiredsize>ndata)
    {
        data=(unsigned char *)realloc(data,requiredsize);
    // clear new data;
        memset(&(data[ndata]),0,requiredsize-ndata);
        ndata=requiredsize;
    }
    log("AUDIOBUFFER","%i n=%i ndata=%i\n",startpos*samplesize,nsamples*samplesize,ndata);
    memcpy(&(data[startpos*samplesize]),audiodata,nsamples*samplesize);
};
void AUDIOBUFFER::getAudio(double time, unsigned char *audiodata, unsigned int nsamples)
{
    if(time<0) return;
    memset(audiodata,0,nsamples*samplesize);

    int startpos=time*samplerate;
    int ncopy=ndata-(startpos*samplesize);
    if(ncopy>int(nsamples*samplesize)) ncopy=nsamples*samplesize;
    if(ncopy>0)
        memcpy(audiodata,&(data[startpos*samplesize]),ncopy);
};

