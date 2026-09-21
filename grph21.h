#if (!defined(__GRPH21_H__))
#define __GRPH21_H__
#include <stdint.h>

// 256色モード/パックドピクセルを有効にする
extern void Enable256(void);
// 16色モードに戻す
extern void Disable256(void);
// 9821か調べる
extern int is9821(void);
// 480ラインモードにする
extern void Set480(void);
// 400ラインモードにする
extern void Set400(void);
// テキスト表示をONにする
extern void TextOn(void);
// パレット設定
extern void SetPalette(uint8_t p,uint8_t r,uint8_t g,uint8_t b);
// バンク設定(b0000からが次のバンクになる)
extern void SetBank(int bank);
extern void Pset(int x,int y,unsigned char v);

extern void ClearVRAM(void);

// グラフィック表示ON
extern void GrphOn(int mode);
extern void GrphOff(void);

#endif
