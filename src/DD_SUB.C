#include <stdio.h>
#include <bdosfunc.h>
#include <math.h>
#include "dd.h"

/* 再帰処理を使用しない */
#pragma nonrec


/* dd_drv.cにて定義 */
extern char *sputsize( str, psl );

/* dd_msg.cにて定義 */
extern VOID vputs_b( x, y, str );
extern VOID putbox( x, y, w, h );
extern VOID errmsgbox( icode, fname );



/**************************************************************
	最後に \ がなければ \ を追加
	return値：	*s
**************************************************************/
char *addbackslash( s )
char *s; /* r+w 処理対象 */
{
	int ilen;
	
	ilen = strlen( s );
	
	if ( 0 < ilen ){
		/* 最後がNULLでも\でもない→\を追加 */
		if ( (s[ ilen - 1 ] != NULL) && (s[ ilen - 1 ] != '\\' )) {
			strcat( s, "\\" );
		}
	}
	return s;
}


/**************************************************************
	パス文字列の短縮形を作成する
	return値：shortpath
**************************************************************/
char *makeshortpath( path, shortpath )
char	*path;			/* r/o パス文字列 */
char	*shortpath;		/* w/o 短縮したパス文字列を返す */
{
	int ilen;
	char swork[MAXPATH];
	
	ilen = strlen( path );
	
	if ( ilen < 36 ) {
		/* そのまま */
		strcpy( shortpath, path );
	} else {
		/* 縮める */
		strcpy( swork, path );
		swork[3] = '\0';	/* d:\の次 */
		strcat( swork, "..." );
		strcat( swork, (char *)(path + ilen - 31) );
		
		strcpy( shortpath, swork );
	}
	
	return shortpath;
}


/**************************************************************
	BDOSコール（エラー処理なし）
	return値：	BDOSから戻されたAレジスタの値
**************************************************************/
int bdoscall( c, a, b, de, hl, ix, r )
TINY c;						/* r/o ファンクション番号 */
unsigned a, b, de, hl, ix;	/* r/o 各レジスタへの設定値 */
XREG *r;					/* w/o 各レジスタへの戻り値 */
{
	r->bc =	c;	/* c←ファンクション番号 */
	r->bc += b << 8;
	r->af =	a << 8;
	r->de =	de;
	r->hl =	hl;
	r->ix =	ix;

	callxx( BDOS, r );	/* BDOSコール */

	return r->af >> 8;	/* A:エラーコード */
}


/**************************************************************
	BDOSコール（エラーメッセージ表示あり）
	return値：	OK:成功(Aレジスタ=0) ERROR:失敗(Aレジスタ!=0)
**************************************************************/
STATUS bdos_trap( c, a, b, de, hl, ix, r, fname )
TINY c;						/* r/o ファンクション番号 */
unsigned a, b, de, hl, ix;	/* r/o 各レジスタへの設定値 */
XREG *r;					/* w/o 各レジスタへの戻り値 */
char *fname;				/* 呼び出し元関数名 デバッグ用 */
{
	int iret = 0;	/* return値 */
	STATUS sret = ERROR;
	
	/* 丸投げ */
	iret= bdoscall( c, a, b, de, hl, ix, r );
	
	if ( iret == 0 ) {
		/* エラーでなければ */
		sret = OK;
	} else {
		/* エラーの場合はメッセージを表示 */
		errmsgbox( iret, fname );
	}
	
	return sret;
}


/**************************************************************
	ファイル属性ビットの確認
**************************************************************/
VOID attr( a )
TINY a;		/* r/o ファイル属性ビット */
{
/*
ファイル属性
+デバイス属性			排他
|+常に0
||+アーカイブ
|||+ディレクトリ		と通常のファイル
00ad vshr
     +ボリューム名		排他
      +システムファイル
       +不可視ファイル
        +R/O
*/
	if ( a & 0x01 ) puts("/r");
	if ( a & 0x02 ) puts("/h");
	if ( a & 0x04 ) puts("/s");
	if ( a & 0x08 ) puts("/v");
	if ( a & 0x10 ) puts("/d");
	if ( a & 0x20 ) puts("/a");
	if ( a & 0x40 ) puts("/?");
	if ( a & 0x80 ) puts("/!");
}


/**************************************************************
	SLONG値の整形
	return値：*str
**************************************************************/
/*12345678
・nnn,nnn		<1,000,000
・nn,nnnK		<  100,000k = 102,400,000
・nn,nnnM		>  100,000k ※ulong=4,294,967,295
*/
char *sputsize( str, psl )
char	*str;	/* w/o ↓を文字列で返す \0込みで8バイト確保すること */
SLONG	*psl;	/* r/o 文字列型に変換する数値 */
{
	int		iret;
	char	swk[12];			/* テキスト整形用ワーク */
	SLONG	slv1, slv2, slans, slkilo;	/*SLONG型計算用ワーク*/
/*
	printf( "* <putsize()1:%ld>\n", psl );	←OK
	printf( "* <putsize()2:%ld>\n", &psl );	←NG
*/
	strcpy(  str, "ABCDEFG" );

	/* 1,000,000未満か確認 */
	itosl( &slv1, 1000 );
	if( slcmp( psl, &slv1 ) == -1 ) {	/* psl < 1,000 ? */

		/* 1,000未満ならカンマを省く */
					/* 1234 567n*/
		sprintf( str, "    %3ld", psl );

	} else {
		
/*		itosl( &slv1, 1000 ); */
		slmul( &slans, &slv1, &slv1 );	/* 1,000,000 */
		if( slcmp( psl, &slans ) == -1 ) {	/* psl < 1,000,000 ? */
			
			/* 1,000,000未満 */
			sprintf( swk, "%11ld", psl );
			str[0] = swk[ 5];	/* 上3桁 */
			str[1] = swk[ 6];
			str[2] = swk[ 7];
			str[3] = ',';
			str[4] = swk[ 8];	/* 下3桁 */
			str[5] = swk[ 9];
			str[6] = swk[10];
			
		} else {
			
			slcpy( &slans, psl );		/* 加工用に複製 */
			/* kB単位に変換 (÷1024) */
			slsra( &slkilo, &slans, (TINY)10 );
			/* 100,000k未満か確認(102,400,000) */
/*			itosl( &slv1, 1000 ); */
			itosl( &slv2,  100 );
			slmul( &slans, &slv1, &slv2 );	/* 100,000 */
			
			if( slcmp( &slkilo, &slans ) == -1 ) { /* slkilo < 100,000k */
				
				/* 100,000k未満 */
				sprintf( swk, "%11ld", &slkilo );
				str[0] = swk[ 6];	/* 上2桁 */
				str[1] = swk[ 7];
				str[2] = ',';
				str[3] = swk[ 8];	/* 下3桁 */
				str[4] = swk[ 9];
				str[5] = swk[10];
				str[6] = 'k';		/* キロ */
				
			} else {
				
				/* MB単位に変換 (k÷1024) */
				/* SLONGの範囲内では桁不足にはならない */
				slsra( &slans, &slkilo, (TINY)10 );
				sprintf( swk, "%11ld", &slans );
				str[0] = swk[ 6];	/* 上2桁 */
				str[1] = swk[ 7];
				str[2] = ',';
				str[3] = swk[ 8];	/* 下3桁 */
				str[4] = swk[ 9];
				str[5] = swk[10];
				str[6] = 'M';		/* キロ */
			}
		}
	}
	
	return str;
}


