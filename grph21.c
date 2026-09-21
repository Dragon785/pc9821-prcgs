#include <i86.h>
#include <dos.h>
#include <conio.h>
#include <stdint.h>
#include <string.h>

void Enable256(void)
{
	outp(0x6a,0x07);
	outp(0x6a,0x21);
	outp(0x6a,0x69);
	outp(0x6a,0x06);

#if defined(__386__)&&defined(__DOS__)
	*(uint8_t*)(0xe0100)=0x0; // packed pixel
#else
	*(uint8_t __far*)MK_FP(0xe000,0x0100)=0x0;
#endif
	uint8_t work=*(uint8_t*)(0x0054d);
	work|=0x80;
	*(uint8_t*)(0x0054d)=work;
}

void Disable256(void)
{
	outp(0x6a,0x07);
	outp(0x6a,0x68);
	outp(0x6a,0x20);
	outp(0x6a,0x06);
	
	uint8_t work=*(uint8_t*)(0x0054d);
	work&=0x7f;
	*(uint8_t*)(0x0054d)=work;
	
	outp(0x6a,1);
}

int is9821(void)
{
	uint8_t check=*(uint8_t*)(0x045c);
	return (check&0x40);
}

void Set480(void)
{
	union REGS regs;
	regs.h.ah=0x30;
	regs.h.al=0x0c; // 31KHz
	regs.h.bh=0x31; // 480line 25
#if defined(__386__)&&defined(__DOS__)
	int386(0x18,&regs,&regs);
#else
	int86(0x18,&regs,&regs);
#endif
}

void Set400(void)
{
	union REGS regs;
	regs.h.ah=0x30;
	regs.h.al=0x08; // 24KHz
	regs.h.bh=0x21; // 400line 25
#if defined(__386__)&&defined(__DOS__)
	int386(0x18,&regs,&regs);
#else
	int86(0x18,&regs,&regs);
#endif
	uint8_t work=*(uint8_t*)(0x54c);
	work&=0xcc; // 400 line
	work|=0x31; // 25 row
	*(uint8_t*)(0x54c)=work;
}

void TextOn(void)
{
	union REGS regs;
	regs.h.ah=0x0c;

#if defined(__386__)&&defined(__DOS__)
	int386(0x18,&regs,&regs);
#else
	int86(0x18,&regs,&regs);
#endif
}

void SetPalette(uint8_t p,uint8_t r,uint8_t g,uint8_t b)
{
		outp(0xa8,p); // pal no
		outp(0xaa,g); 
		outp(0xac,r); 
		outp(0xae,b); 
}

void SetBank(int bank)
{
    uint16_t bank0=(bank&0x7);
    uint16_t bank1=(bank+1)&0x7;
#if defined(__386__)&&defined(__DOS__)
    *(uint16_t*)(0xe0004)=bank0;
    *(uint16_t*)(0xe0006)=bank1;
#else
    *(uint16_t __far*)MK_FP(0xe000,0x0004)=bank0;
    *(uint16_t __far*)MK_FP(0xe000,0x0006)=bank1;
#endif
}

uint16_t SetBankAndGetStartOffset(int y)
{
	uint32_t yadr=(uint32_t)(y)*640;
	uint32_t ybank=yadr>>15;
	SetBank(ybank);
	return (yadr&0x7fff);
}

void Pset(int x,int y,unsigned char v)
{
	uint16_t ofsy=SetBankAndGetStartOffset(y);
#if defined(__386__)&&defined(__DOS__)
	*(uint8_t*)(0xa8000+x+ofsy)=v;
#else
	*(uint8_t __far*)(MK_FP(0xa800,x+ofsy))=v;
#endif
}

void GrphOn(unsigned char md)
{
	union REGS regs;

	regs.h.ch=md;
	regs.h.ah=0x40;

#if defined(__386__)&&defined(__DOS__)
	int386(0x18,&regs,&regs);
#else
	int86(0x18,&regs,&regs);
#endif
}

void GrphOff(void)
{
	union REGS regs;
	regs.h.ah=0x41;
#if defined(__386__)&&defined(__DOS__)
	int386(0x18,&regs,&regs);
#else
	int86(0x18,&regs,&regs);
#endif
}

void ClearVRAM(void)
{
	for (int bank=0;bank<8;++bank)
	{
		SetBank(bank);
		for (uint32_t i=0;i<0x10000;++i)
#if defined(__386__)&&defined(__DOS__)
		*(unsigned char*)(0xa8000+i)=0;
#else
		*(unsigned char __far *)MK_FP(0xa800,i)=0;
#endif
	}
}
