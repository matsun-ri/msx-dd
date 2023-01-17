/**************************************************************
	DD_INPUT.c 画面表示（入力ボックス関連）
**************************************************************/
#include <stdio.h>
#include <msxbios.h>		/* locate() */
#include "dd.h"			/* math.hを含む */


/* dd_msg.cにて定義 */
extern VOID vputsgraph( x, y, str );
extern VOID vputs_b( x, y, str );
extern VOID putbox( x, y, w, h );


/**************************************************************
	ファイル名入力用ボックス
	X13, Y10からW14, H5の表示領域を作る
	1行目：X14,   Y11 タイトル	vputs_b( 14, 11, "NEW NAME" );
	2行目：X14+1, Y12 入力行
	3～5行目：X14+1,Y13～15 ガイド
**************************************************************/
VOID inputbox( stitle )
char *stitle;	/* r/o タイトル 太字表示される */
{
	putbox( 13, 10, 16, 7 );			/* 枠を表示 */
	vputs_b( 14, 11, 	stitle );
	locate( (TINY)15, (TINY)13 );
	puts( 				"^^^^^^^^ ^^^" );
	vputs_b( 15, 14, 	"[RET] ok" );
	vputs_b( 15, 15, 	"[ESC] cancel" );
}


/**************************************************************
	ファイル名入力
	return値：	OK:入力確定 ERROR:キャンセル
**************************************************************/
STATUS inputline( str )
char *str;	/* r+w	デフォルト値・確定値 16バイト要する */
			/* 		確定時のみ内容が変更される */
{
	int		ix = 0;			/* カーソル位置 */
	int		ilen = 0;
	STATUS	sret = ERROR;	/* return値 */
	char	ch;				/* 入力されたキー */
	char	sinput[16];
	char	wk1[16], wk2[16], wk3[16];
	char	*dot = NULL;
	BOOL	bloop = TRUE;	/* ループフラグ */
	BOOL	bshaping = FALSE;	/* テキスト整形フラグ */
	
	strcpy( sinput, str );
	
	ix = strlen( sinput );
/*
60 '
70 F$="hoehoe.c"
80 X=LEN(F$)
100 'loop
110 '
120 LOCATE 15,12
130 PRINT F$;"----+----+----+"
*/
	do {
		/* 表示更新 */
		locate( (TINY)15, (TINY)12 );
		puts( "-------- ---" );
		locate( (TINY)15, (TINY)12 );
		puts( sinput ); 
/*
140 LOCATE 15+X
150 A=ASC(INPUT$(1))
*/
		/* キー入力 */
		locate( (TINY)(15+ix), (TINY)12 );
		ch = toupper( getch() );
		
		
		/* 入力に対する処理*/
		switch( ch ) {
			
/* 170 IF A=27 THEN END 'ESC */
			case	KEY_ESC:	/* ESC→終了 */
				bloop = FALSE;
				break;

/* 180 IF A=8 AND 0<X THEN X=X-1:F$=LEFT$(F$,X-1)+MID$(F$,X+1) */
			case	KEY_BS:		/* BS */
				if ( 0 < ix ) {
					strcpy( wk2, (char *)(sinput+ix) );
					ix--;
					strcpy( wk1, sinput );
					wk1[ix] = '\0';
					strcpy( sinput, wk1 );
					strcat( sinput, wk2 );
				}
				break;

/* 190 IF A=127 THEN F$=LEFT$(F$,X)+MID$(F$,X+2) */
			case	KEY_DEL:		/* DEL */
				strcpy( wk1, sinput );
				wk1[ix] = '\0';
				strcpy( wk2, (char *)(sinput+ix+1) );
				strcpy( sinput, wk1 );
				strcat( sinput, wk2 );
				break;
				
/* 195 IF A=13  THEN 400 'seikei */
			case	KEY_RET:		/* RET→整形 */
			
/*
400 'seikei
410 L=LEN(F$)
*/
				bshaping = FALSE;	/* テキスト整形フラグ */
				ilen = strlen( sinput );
				
/*
419 ' .ga nakattara . wo tuika
420 IF 8<L AND 0=INSTR(F$,".") THEN F$=LEFT$(F$,8)+"."+MID$(F$,9)
*/
				/* 9文字以上あるのに.が見つからない場合は.を追加 */
				if( 8 < ilen ) {
					if ( strchr( sinput, '.' ) == NULL ) {
						strcpy( wk1, sinput );
						strcpy( wk2, sinput );
						wk1[8] = '.';
						wk1[9] = '\0';
						strcpy( sinput, wk1 );
						strcat( sinput, (char *)(wk2 + 8) );
						
						bshaping = TRUE;
					}
				}
/*
430 M=INSTR(F$,".")
*/
				dot = strchr( sinput, '.' );
/*
439 ' .ga attara . noato3moji ikouwo kirisute
440 IF 0<M THEN F$=LEFT$(F$,M+3)
*/
				if ( NULL != dot ) {
/*
         1111
1234567890123

0123456789012

abcdefghijkl
abc.def
*/
					/* .がある場合は、.の後ろ4文字目以降を切り捨て */
					ilen = (int)(dot - sinput) + 4;
					if( ilen < strlen(sinput) ) {

						if ( 12 < ilen ) ilen = 12;
						*(char *)(sinput + ilen) = '\0';

						bshaping = TRUE;
					}

/*
450 IF 9<M THEN F$=LEFT$(F$,8)+MID$(F$,M)
*/
					/* .が9文字目以降にある場合は、.の前9文字目以降を切り捨て */
					if ( 8 < (int)(dot - sinput) ) {
						strcpy( wk1, sinput );
						wk1[8] = '\0';
						strcpy( sinput, wk1 );
						strcat( sinput, dot );
						
						bshaping = TRUE;
					}
				}
				
				/* 整形が行われていなければ確定とする */
				if ( FALSE == bshaping ){
					bloop = FALSE;
					sret = OK;
					strcpy( str, sinput );
				}
				
				break;
/*
230 'ｶ-ｿﾙ ｲﾄﾞｳ
240 IF A=&H1D THEN X=X-1
250 IF A=&H1C THEN X=X+1
*/
			case KEY_RIGHT:			/* →キー イン */
				ix++;
				break;
				
			case KEY_LEFT:			/* ←キー イン */
				ix--;
				break;

			case ':':				/* ファイル名として無効な文字 */
			case ';':
			case ',':
			case '=':
			case '+':
			case '\\':
			case '<':
			case '>':
			case '|':
			case '/':
			case '"':
			case '[':
			case ']':
			case 0xff:
				/* 何もしない */
				break;
				
/*dd
210 'ﾌｧｲﾙﾒｲﾆ ﾂｶｴﾅｲ ﾓｼﾞ
220 ':;.,=+\<>|/"[]
200 '0~20H,7fH,FFh
280 'ｶﾝｹｲﾅｲﾓｼﾞｦ ﾊｼﾞｸ
290 IF (A<=&H20) OR (&H5F<A) THEN 110
*/
			default:
				/* コントロールコードと空白は無視 */
				if ( ch <= ' ' ) break;
/*
300 ' ｲﾏ . ｶﾞ ﾌｸﾏﾚﾃﾅｲ ﾊﾞｱｲ ﾉﾐ . ｦ ｷｮｶ
310 IF (A=&H2E) AND (0<INSTR(F$,".")) THEN 110
*/
				/* ファイル名に既に.がある場合は.を無視する */
				if ( ( '.' == ch ) && ( strchr( sinput, '.' ) != NULL ) ) {
					 break;
				}
/*
320 'ｿｳﾆｭｳ
330 F$=LEFT$(F$,X)+CHR$(A)+MID$(F$,X+1)
340 F$=LEFT$(F$,12)
350 X=X+1
*/
				/* 入力した文字を挿入 */
				strcpy( wk1, sinput );
				wk1[ix] = '\0';
				wk2[0]  = ch;
				wk2[1]  = '\0';
				strcpy( wk3, (char *)(sinput+ix) );
				strcpy( sinput, wk1 );
				strcat( sinput, wk2 );
				strcat( sinput, wk3 );
				sinput[12] = '\0';
				ix++;
		}
		
/*
260 IF X<0 THEN X=0
270 IF LEN(F$)<X THEN X=LEN(F$)

360 IF LEN(F$)<X THEN X=LEN(F$)
*/
		ilen = strlen( sinput );
		if( ilen < ix ) ix = ilen;
		if( ix < 0 ) ix = 0;
/*
370 GOTO 110
*/

	} while ( TRUE == bloop );
	
	
	return sret;
}


/**************************************************************
	ドライブ名入力用ボックス
**************************************************************/
VOID drvbox( stitle )
char *stitle;	/* r/o タイトル 太字表示される */
{
	putbox( 12, 10, 18, 6 );			/* 枠を表示 */
	vputs_b( 13, 11, 	stitle );
	locate( (TINY)14, (TINY)12 );
	puts( 				"to:_" );
	vputs_b( 14, 13, 	"[A]-[H] drive" );
	vputs_b( 14, 14, 	"[ESC]   cancel" );
}


