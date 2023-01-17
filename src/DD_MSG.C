/**************************************************************
	DD_MSG.c 画面表示（メッセージボックス関連）
**************************************************************/
#include <stdio.h>
#include <glib.h>		/* vpeek, vpoke */
#include <bdosfunc.h>	/* XREG */
#include "dd.h"			/* math.hを含む */

/* dd_sub.cにて定義 */
extern int bdoscall( c, a, b, de, hl, ix, r );



/**************************************************************
	VPOKEによる文字列表示：罫線印字用
	strの文字を-40hずらして直接VRAMに書き込み
**************************************************************/
VOID vputsgraph( x, y, str )
int		x;		/* r/o X座標 1～ */
int		y;		/* r/o Y座標 1～ */
char	*str;	/* r/o 表示する文字列 */
{
	int i;
	int ilen = 0;
	char buf[40];
	
	
	ilen = strlen( str );
	if ( 0 < ilen ) {
		
		if ( 40 < ilen ) ilen = 40;
		
		/* 先に変換しておいて後でブロック転送 */
		for ( i=0; i < ilen; i++ ) {
			buf[i] = str[i] - 0x40;
		}

		/*SCREEN0の文字位置は 0+Y*40+X */
		ldirvm( (y-1) * 40 + (x-1), buf, ilen );
/*
		for ( i=0; i<strlen(str); i++ ) {
			/*SCREEN0の文字位置は 0+Y*40+X */
			vpoke( (y-1) * 40 + (x-1) + i, str[i] - 0x40 );
		}
*/
	}
}


/**************************************************************
	VPOKEによる文字列表示：英字印字用
	strの文字を大文字だけ+A0hずらして直接VRAMに書き込み
**************************************************************/
VOID vputs_b( x, y, str )
int		x;		/* r/o X座標 1～ */
int		y;		/* r/o Y座標 1～ */
char	*str;	/* r/o 表示する文字列 */
{
	int i;
	int ilen = 0;
	char ch;
	char buf[40];
	
	
	ilen = strlen( str );
	if ( 0 < ilen ) {

		if ( 40 < ilen ) ilen = 40;
		
		/* 先に変換しておいて後でブロック転送 */
		for ( i=0; i < ilen; i++ ) {

			/*大文字であれば+A0h*/
			ch = str[i];
			if( isupper(ch) ) ch += 0xa0;
			buf[i] = ch;
	/*
			vpoke( (y-1) * 40 + (x-1) + i, ch );
	*/
		}

		/*SCREEN0の文字位置は 0+Y*40+X */
		ldirvm( (y-1) * 40 + (x-1), buf, ilen );
		
	}
}


/**************************************************************
	メッセージボックスの枠を印字
**************************************************************/
VOID putbox( x, y, w, h )
int x;			/* r/o 左上X */
int y;			/* r/o 左上Y */
int w;			/* r/o 全幅 内幅は-2 */
int h;			/* r/o 全高 スペース行は-2  */
{
	int  i;
	char skeisen[42], sspace[42];

	strcpy( skeisen, "X" );	/* ┌ */
	strcpy( sspace,  "V" );	/* │ */
	
	/* 横罫線を作る */
	for ( i=0; i<w-2; i++ ) {
		skeisen[1+i] = 'W';	/* ─ */
		sspace[1+i] = '@';	/* スペース */
	}
	
	skeisen[w-1] = 'Y';		/* ┐ */
	skeisen[w] = '\0';
	
	sspace[w-1] = 'V';		/* │ */
	sspace[w] = '\0';
	
	/* 上辺 */
/*	skeisen[0] = 'X';
	skeisen[w-1] = 'Y';
*/
	vputsgraph( x, y+0, skeisen );

	/* 中段 */
	for ( i=0; i<h-2; i++ ) {
		vputsgraph( x,     y+1+i, sspace );
/*
		vputsgraph( x,     y+1+i, "V" );	/* 右 */
		vputsgraph( x+w-1, y+1+i, "V" );	/* 左 */
		locate( (TINY)(x+1), (TINY)(y+1+i) );
		puts( sspace );
*/
	}
	
	/* 下辺 */
	skeisen[0] = 'Z';	/* └ */
	skeisen[w-1] = '[';	/* ┘ */
	vputsgraph( x, y+h-1, skeisen );

}


/**************************************************************
	メッセージボックス
**************************************************************/
VOID msgbox( x, y, stitle, stext )
int x;			/* r/o 左上X 1～ 0の場合はtitleとstrの長い方から計算 */
int y;			/* r/o 左上Y */
char *stitle;	/* r/o タイトル 太字表示される */
char *stext;	/* r/o 本文 太字表示されない */
{
	int iw, ix, i;
	char ch;
	
	/* 横幅を得る */
	iw = max( strlen( stitle ), strlen( stext ) ) + 4;
	if( 40 < iw ) iw = 40;
	
	ix = 21 - iw/2;
	
	/* 枠を表示 */
	putbox( ix, 10, iw, 4 );
	
	/* タイトル */
	vputs_b( ix+1, 11, stitle );

	/* テキスト行 */
	locate( (TINY)(ix + 2), (TINY)12 );
	puts( stext );

locate((TINY)ix, (TINY)14);printf("* <iw=%d; ix=%d>", iw, ix);

	ch = getch();
	
}


/**************************************************************
	プログレスバー用ボックス
	コピー・移動・削除の進捗状況表示を想定
	X13, Y10からW14, H3の表示領域を作る
	1行目：X14,   Y11 タイトル	vputs_b( 14, 11, "COPYING..." );
	2行目：X14+1, Y12 内容		locate( (TINY)15, (TINY)12 );puts()...
	3行目：X14,   Y13 プログレスバー
		進捗表示にはX15, Y13から12文字を使用する
**************************************************************/
VOID progressbox( stitle )
char *stitle;	/* r/o タイトル 太字表示される */
{
	putbox( 13, 10, 16, 5 );			/* 枠を表示 */
	vputs_b( 14, 11, stitle );
	vputsgraph( 14, 13, "TJJJJJJJJJJJJS" );
/*
            1234567890123456
            |COPYING...    |
            | hoehoeho.txt |
            |[123456789012]|
1234567890123456789012345678901234567890
| ABCDEFGH.TXT xx,xxx 00-00-00 00:00U|U|
*/
}


/**************************************************************
	プログレスバーの表示
	1行目：何もしない
	2行目：指定されたファイル名を表示する
	3行目：プログレスバーを表示する
**************************************************************/
VOID putprogress( stext, idenomi, inum )
char	*stext;		/* r/o タイトル 太字表示される */
int		idenomi;	/* r/o プログレスバーの分母 */
int		inum;		/* r/o プログレスバーの分子 */
{
	int i=0;
	
	/* 処理対象ファイルの表示 */
	locate( (TINY)15, (TINY)12 );
	printf( "%-12s", stext );
	
	/* プログレスバーを進める(0.5進めておく) */
	for ( i=0; i < 12 * (inum*2+1) / (idenomi*2); i++ ){
		vputsgraph( 15+i, 13, "P" );
	}
}


/**************************************************************
	エラーメッセージボックス
**************************************************************/
VOID errmsgbox( icode, fname )
int icode;		/* r/o エラーコード */
char *fname;	/* r/o 情報（発生位置など） */
{
	char	ch;
	char	title[40];	/* タイトル */
	char	msg[72];	/* エラーメッセージ用文字列 最低64バイト */
	XREG	reg;		/* BDOSコールからの戻り値取得用 */
	
	bdoscall( _EXPLAIN,		/* エラーコードの説明文字列を得る */
		0, 
		icode,			/* エラーコード */
		(unsigned)msg, 
		0, 0,
		&reg			/* 戻り値受取用 */
		 );
		 
	sprintf( title, "ERROR at %s", fname );
/*
strcpy(msg,
	"12345678901234567890123456789012this is over 32bytes sentence!!!");
*/
/*	 1234567890123456789012345678901234567890123456789012345678901234*/

	if ( strlen( msg ) <= 32 ) {
		
		msgbox( 0, 0, title, msg );
		
	} else {

		/* エラーメッセージが長い場合は2行で表示する */
		putbox(  3, 10, 36, 5 );
		vputs_b( 4, 11, title );
		
		/* 2行目 */
		locate(  (TINY)5, (TINY)13 );
		puts( msg+32 );
		
		/* 1行目 */
		msg[32] = '\0';
		locate(  (TINY)5, (TINY)12 );
		puts( msg );
		
	}
	
	ch = getch();

}


