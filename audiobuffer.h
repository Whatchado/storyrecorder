#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
inline void convertFromPlanar(unsigned char *samples,int n,int bps)
{
     unsigned char *ret=(unsigned char *)malloc(n*bps);
     for(int a=0;a<n/2;a++)
     {
         unsigned char *s=&samples[(a)*bps];
         unsigned char *d=&ret[a*2*bps];
         memcpy(d,s,bps);
         s=&samples[(a+n/2)*bps];
       d=&ret[(a*2+1)*bps];
                  memcpy(d,s,bps);
     }
     memcpy(samples,ret,n*bps);
     free(ret);
     return;
}

inline unsigned char *convertSamples(unsigned char *samples,int n,int bps_from,int bps_to)
{
    if(!samples) return NULL;
    if(n<=0) return NULL;
    unsigned char *ret=(unsigned char *)malloc(n*bps_to);
    for(int a=0;a<n;a++)
    {
        void *s=samples+a*bps_from;
        void *d=samples+a*bps_to;
        uint64_t v=0;
        switch (bps_from) {
        case 1:
            v=uint64_t(*((uint8_t *)s))<<24;
            break;
        case 2:
            v=uint64_t(*((uint16_t *)s))<<16;
            break;
        case 4:
            v=uint64_t(*((uint32_t *)s));
            break;
        }
        switch (bps_to) {
        case 1:
            (*((uint8_t *)d))=v>>24;
            break;
        case 2:
            (*((uint16_t *)d))=v>>16;
            break;
        case 4:
            (*((uint32_t *)d))=v;
            break;
        }
    }
    return ret;
}

class AUDIOBUFFER
{
public:
    AUDIOBUFFER();
    ~AUDIOBUFFER();
    void setSampleRate(int sr);
    void setSampleSize(int ss);
    void reset();
    void addAudio(double time, unsigned char *audiodata, unsigned int nsamples);
    void getAudio(double time, unsigned char *audiodata, unsigned int nsamples);
    bool hasData() {return ndata>0;};
private:
    unsigned char *data;
    int ndata;
    int position;
    int samplerate;
    int samplesize;
};

#endif // AUDIOBUFFER_H
