#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <memory.h>
#include <stdlib.h>
#include "grph21.h"
#include "PRCGS.H"

static void InitGraph(void)
{
	GrphOn(0xc0);
    Set400();
    Enable256();
}

static void FinishGraph(void)
{
    Disable256();
    TextOn();
}

static void MakePallete(void)
{
    for (uint8_t r=0;r<6;++r)
    {
        for (uint8_t g=0;g<6;++g)
        {
            for (uint8_t b=0;b<6;++b)
            {
                uint8_t palNo=(r*36)+(g*6)+b;
                SetPalette(palNo,r*51,g*51,b*51);
            }
        }
    }
    SetPalette(0xff,0x10,0x10,0x90); // for debug
}

int main(int argc,char* argv[])
{
    if (!is9821())
    {
        printf("PC9821系ではないようです。\n");
        return -1;
    }
    if (argc!=2)
    {
        printf("Usage look21 prcfile\n");
        return -1;
    }
    if (StartReadPRCGS(argv[1]))
    {
        return -1; // read error;
    }
    unsigned int width=GetWidth();
    unsigned int height=GetHeight();
    if ((width>320)||(height>200))
    {
        printf("320x200を超える画像には対応していません");
        return -1;
    }
    
    unsigned int bufSize=width*height;
    unsigned char* r=malloc(bufSize);
    unsigned char* g=malloc(bufSize);
    unsigned char* b=malloc(bufSize);

    if ((!r)||(!g)||(!b))
    {
        free(r);r=NULL;
        free(g);g=NULL;
        free(b);b=NULL;
        printf("ワークメモリが確保できません\n");
        return -1;
    }
    if (Extract(r,g,b))
    {
        free(r);r=NULL;
        free(g);g=NULL;
        free(b);b=NULL;
        printf("展開失敗\n");
        return -1;
    }

    InitGraph();
    ClearVRAM();
    MakePallete();
    unsigned char* rr=r,*gg=g,*bb=b;
    
    for (int y=0;y<height;++y)
    {
        for (int x=0;x<width;++x)
        {
            unsigned char pixr=*rr++;
            unsigned char pixg=*gg++;
            unsigned char pixb=*bb++;
            // 基本値を算出
            unsigned char baser=pixr/51;
            unsigned char baseg=pixg/51;
            unsigned char baseb=pixb/51;
            unsigned char modr=pixr%51;
            unsigned char modg=pixg%51;
            unsigned char modb=pixb%51;
            // 4つ分ディザ演算
            unsigned char pal[4];
            unsigned char dither[4]=
            {
                51,34,25,17
            };
            for (int d=0;d<4;d++)
            {
                unsigned char rd=baser;
                unsigned char gd=baseg;
                unsigned char bd=baseb;
                if ((modr>dither[d])&&(rd<5))
                {
                    rd++;
                }
                if ((modg>dither[d])&&(gd<5))
                {
                    gd++;
                }
                if ((modb>dither[d])&&(bd<5))
                {
                    bd++;
                }
                pal[d]=rd*36+gd*6+bd;
            }
            Pset(x*2+0,y*2+0,pal[0]);
            Pset(x*2+1,y*2+0,pal[1]);
            Pset(x*2+1,y*2+1,pal[2]);
            Pset(x*2+0,y*2+1,pal[3]);
        }
    }

    free(r);r=NULL;
    free(g);g=NULL;
    free(b);b=NULL;
    getch();
    FinishGraph();
}
