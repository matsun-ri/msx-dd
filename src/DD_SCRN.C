/**************************************************************
	DD_SCRN.c 画面表示
**************************************************************/
/*
・PCG書き換え
	・VOID setpcg()にて作成
	・static TINY ptn[]←パターン定義
	・罫線（MSXのグラフィックキャラクタ）→ 罫線PCG
	・0xe0..0xfa（「た」～「れ」）→大文字のボールド
	・（未定・暫定で「g」）→カーソル「→」
	・（未定・暫定で「*」）→チェックマーク
・特殊な表示処理
	・vputsgraph( x, y, str )：罫線印字用
		・strの文字を-40hずらして直接VRAMに書き込み
	・vputs_b( x, y, str )：英字印字用
		・strの文字を大文字だけ+A0hずらして直接VRAMに書き込み
・
*/
#include <stdio.h>
#include <glib.h>		/* vpeek, vpoke */
#include <bdosfunc.h>	/* XREG */
/* #include <msxbios.h>		locate() */
#include "dd.h"			/* math.hを含む */

extern VOID vputsgraph( x, y, str );
extern VOID vputs_b( x, y, str );
extern VOID putbox( x, y, w, h );


/* 外字パターン		書き換え対象の文字コード, ビットパターン×8 */
static TINY ptn[]={
					/* ここから縦棒グラフ */
					0x11,	0x84,0x78,0x0,0x0,0xFC,0x78,0x30,0x0,	/*┴Q*/
					0x12,	0x0,0x30,0x78,0xFC,0x0,0x0,0x78,0x84,	/*┬R*/
					0x15,	0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,/*□U*/
					0x1c,	0xB4,0xB4,0xB4,0xB4,0xB4,0xB4,0xB4,0xB4,/*■\*/
					/* ここから横棒グラフ */
					0x0a,	0xFF,0x0,0x0,0x0,0x0,0x0,0xFF,0x0,		/*0J*/
					0x0b,	0xFF,0x0,0x80,0x80,0x80,0x0,0xFF,0x0,	/*1K*/
					0x0c,	0xFF,0x0,0xC0,0xC0,0xC0,0x0,0xFF,0x0,	/*2L*/
					0x0d,	0xFF,0x0,0xE0,0xE0,0xE0,0x0,0xFF,0x0,	/*3M*/
					0x0e,	0xFF,0x0,0xF0,0xF0,0xF0,0x0,0xFF,0x0,	/*4N*/
					0x0f,	0xFF,0x0,0xF8,0xF8,0xF8,0x0,0xFF,0x0,	/*5O*/
					0x10,	0xFF,0x0,0xFC,0xFC,0xFC,0x0,0xFF,0x0,	/*6P*/
					0x13,	0x80,0x40,0x40,0x40,0x40,0x40,0x80,0x0,	/*)S:┤*/
					0x14,	0x4,0x08,0x08,0x08,0x08,0x08,0x4,0x0,	/*(T:├*/
					/* ここから罫線 配置はグラフィックキャラクタの罫線と同じ*/
					0x16, 	0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, /*│V*/
					0x17, 	0x0,0x0,0x0,0xFF,0xFF,0x0,0x0,0x0,		/*─W */
					0x18, 	0x0,0x0,0x0,0x1F,0x3F,0x30,0x30,0x30,	/*┌X*/
					0x19, 	0x0,0x0,0x0,0xE0,0xF0,0x30,0x30,0x30,	/*┐Y*/
					0x1a, 	0x30,0x30,0x30,0x3F,0x1F,0x0,0x0,0x0,	/*└Z*/
					0x1b, 	0x30,0x30,0x30,0xF0,0xE0,0x0,0x0,0x0,	/*┘[*/
					/* ここから記号 */
/*
					'g', 	0x40,0x60,0x70,0x78,0x70,0x60,0x40,0x0,	/*→g */
*/
					'*', 	0x0,0x0,0x4,0xC,0xd8,0x70,0x20,0x0 	/*チェック'*'*/
				};

/* 画面デザイン 実際には文字コードを-40hして書き込む */
/*                                1         2         3         4
                         1234567890123456789012345678901234567890 */
static char scr[7][42] =
					{	"XWWWWWWWWWWWWWWWWY@@@@XWWWWWWWWWWWWWWWWY",
						"V@@@@@@@@@@@@@@@@V@@@@V@@@@@@@@@@@@@@@@V",
						"V@@@@@@@@@@@@@@@@ZWWWWWWWWWWWWWWWWWWWY@V",
						"V@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@UV@V",
						"ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[@V",
						"@@ZWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW[",
						" [Help] [     ] [     ] [     ] [Exit]" };

/**************************************************************
	キャラクタパターン定義
**************************************************************/
VOID setpcg()
{
	int i, j, a;
	
	/* 罫線・カーソル・選択 マーク */
	for ( i = 0; i < 20; i++ ) {
		for ( j = 0; j < 8; j++ ) {
			vpoke(	0x800 + (int)(ptn[ i*9 ]) * 8 + j, ptn[ i*9 + 1+j ] );
		}
	}
	/* 大文字のボールド作成 */
	/* '@'..'Z'→0xe0..0xfa（「た」～「れ」）*/
	for ( i = 0; i < 27*8; i++ ) {
		a = vpeek( 0x800 + ((int)'@')*8 + i );
		vpoke( 0x800 + (0xe0)*8 + i, (TINY)(a | (a>>1)) );
	}
}


/**************************************************************
	画面表示
**************************************************************/
VOID window_l()
{
	int i;

	cls();
	
	/* タブ */
	for ( i=0; i<3; i++ ) {
		vputsgraph( LIST_X - 2, LIST_Y - 3 + i, scr[i] );
	}
	/* リスト部分 */
	for ( i=3; i<3 + LIST_H + 2; i++ ) {
		vputsgraph( LIST_X - 2, LIST_Y - 3 + i, scr[3] );
	}
	/* タブ下部 */
	for ( i=4; i<6; i++ ) {
		vputsgraph( LIST_X - 2, LIST_Y + LIST_H - 4 + i, scr[i] );
	}
	/* 最下行 */
	vputs_b( LIST_X - 2, LIST_Y + LIST_H + 2, scr[6] ); 
/*
	for ( i=0; i<3; i++ ) {
		locate( (TINY)(LIST_X - 2), (TINY)(LIST_Y - 3 + i) );
		puts( scr[i] );
	}
	for ( i=3; i<3 + LIST_H; i++ ) {
		locate( (TINY)(LIST_X - 2), (TINY)(LIST_Y - 3 + i) );
		puts( scr[3] );
	}
	for ( i=4; i<7; i++ ) {
		locate( (TINY)(LIST_X - 2), (TINY)(LIST_Y - 4 + LIST_H + i) );
		puts( scr[i] );
	}
*/
/* GRPH文字 表示テスト */
/*
for ( i=0; i<32; i++ ) {
	vpoke( i, (TINY)i );
}
*/
}



/**************************************************************
	棒グラフの表示
**************************************************************/
/*
	キャラクタコード
		0x0d  0x0f..0x15  0x0e
		   [ 長さ0～6の棒 ]
		   M     O～U     N ←指定文字（-0x40して使われる）
*/
VOID bargraph( sfreebar, ipercent )
char *sfreebar;
int	ipercent;	/* r/o 棒グラフの長さ パーセント */
{
	char sbar [9];
	int i, i36, ichar, imod;

	/*			   123456789 */
	strcpy( sbar, "TJJJJJJS" );

	/* 36分のいくつ？ */
	i36 = 36 * ipercent / 100;
	/* 何文字分？ */
	ichar = i36 / 6;
	/* 余り */
	imod = i36 % 6;
	
	/* 6分率で計算(全部で6文字なので) */
	for ( i = 0; i < ichar; i++ ) {
		sbar[ 1 + i ] = 'P';
	}
	/* 100%未満であれば余りを1文字で表示する */
	if ( ichar < 6 ){
		sbar[ 1 + ichar ] = (TINY)('J' + imod );
	}
	
	strcpy( sfreebar, sbar );

	/* ここに棒グラフを表示する処理 */
/*	vputsgraph( x, y, sbar ); */
/*
locate((TINY)1, (TINY)1);
printf("ipercent:%d iall:%d imod:%d", ipercent, ichar, imod );
*/
}


/**************************************************************
	個別のヘルプの表示
**************************************************************/
VOID putonehelp( ix, iy, ch )
int		ix;		/* r/o 表示位置 X */
int		iy;		/* r/o 表示位置 Y */
char	ch;		/* r/o 表示対象のキー */
{
	char stitle[16];
	char stext[16];
	
	/* 見出し */
	strcpy( stitle, "*" );
	stitle[0] = ch;
	
	/* 内容 */
	strcpy( stext, "" );
	
	/* 説明文 */
	switch( ch ) {
		case 'C':
			strcat( stext, " copy" );
			break;
		case 'D':
			strcat( stitle, "/DEL" );
			strcat( stext, " delete" );
			break;
		case 'H':
			strcat( stext, " this help" );
			break;
		case 'K':
			strcat( stext, " make dir." );
			break;
		case 'L':
			strcat( stext, " change drive" );
			break;
		case 'M':
			strcat( stext, " move" );
			break;
		case 'N':
			strcat( stext, " rename" );
			break;
		case 'Q':
			strcat( stitle, "/ESC" );
			strcat( stext, " exit" );
			break;
		case 'R':
			strcat( stitle, "/0" );
			strcat( stext, " reload" );
			break;
		case 'T':
			strcat( stitle, "/\\" );
			strcat( stext, " root dir." );
			break;
		case 'U':
			strcat( stitle, "/BS" );
			strcat( stext, " upper dir" );
			break;
		default:
			break;
/*
 1-8	drv
*/
	}
	
	/* 見出し */
	vputs_b( ix, iy, stitle );
	/* 内容 */
	vputs_b( ix + strlen( stitle ), iy, stext );
	
}


/**************************************************************
	ヘルプ画面の表示
**************************************************************/
VOID puthelp()
{
	int		i = 0;
	char	ch;
/*
| ABCDEFGH.TXT123,456 00-00-00 00:00H|H|
123456789012345678901234567890
    | A              B             |
      12345678901234 12345678901234
*/

	putbox( 5, 6, 32, 15 );			/* 枠を表示 */
	
	for( i=0; i<13; i++ ) {
		
		/* 内容を表示 */
		putonehelp( 6, 7+i, (char)('A' + i) );
		putonehelp( 22, 7+i, (char)('A' + 13 + i) );
		
	}
	
	ch = getch();
}


