/**************************************************************
	ファイル一覧の取得・表示
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>
/* #include <glib.h>		ldirvm */
/* #include <msxbios.h>	 locate */
#include <math.h>	 /* slcpy */
#include "dd.h"


/* dd_scrn.cにて定義 */
extern VOID vputs_b( x, y, str );

extern VOID putbox( x, y, w, h );
extern int bdoscall( c, a, b, de, hl, ix, r );
extern char *sputsize( str, psl );
extern VOID errmsgbox( icode, fname );



/**************************************************************
	指定されたパスのファイル一覧を得る
	return値：ファイル数
**************************************************************/
int getfiles( path, files )
char	*path;		/* r/o パス */
FILES	*files;		/* w/o FILES型構造体へのポインタ */
{
	int			i, ct, ipos, iret, ilen;
	unsigned	uiform, uiyy, uimm, uidd, uihh, uinn	/*ファイル日付時刻用*/
				,uiwk;
	char		msg[72];
	char		sname[13], ssize[8], sstamp[15], swk[13];
	BOOL		bbreak;
	XREG		reg;
	FIB			fib;
	
	
	/* 処理中の表示 */
	putbox( 14, 9, 14, 5 );
	vputs_b( 16, 11, "LOADING..." );

	
	/* 最初のエントリの検索 */
	iret = bdoscall( _FFIRST,
					0, 
					0x16,				/* ディレクトリ+システム+不可視 */
					(unsigned)path,	/* ドライブ・パス・ファイル */
					0,
					(unsigned)&fib,	/* 戻り値受取用 */
					&reg				/* 戻り値受取用 */
				);
	ct = 0;

	/* 追加検索 */
	while( iret == 0 ){
		
		/* 前回の検索結果の保存 */
		strcpy( files[ct].name, fib.name );
		files[ct].attr = fib.attr;
		files[ct].ismarked = FALSE;	/* マークフラグ */
		files[ct].isalive = TRUE;		/* 死活フラグ */

		/* ファイル名を8.3形式に整形 */
					/*   123456789012n */
		strcpy(  sname, "            " );
		ilen = strlen( fib.name );

		/* "."を後ろから探す */
		/* 見つからない= 0 */
		bbreak = FALSE;
		for( ipos = ilen; 0 < ipos; ipos-- ) {
			if ( fib.name[ipos - 1] == '.' ) break;
		}
		if ( ipos == 0 ) {
			/* .がない */
			strncpy( sname, fib.name, ilen );
		} else {
			/* .がある */
			
			/* ファイル名をコピー */
			strncpy( sname, fib.name, ipos-1 );
			sname[8] = '.';
			/* 拡張子をコピー */
			for( i = 0; i < ilen-ipos; i++ ) {
				sname[ i + 9 ] = fib.name[ ipos + i ];
			}
		}

		/* ファイルサイズの取得 */
		slcpy( (SLONG *)(files[ct].size), (SLONG *)(fib.filesize) );

		/* ディレクトリか？ */
		if( fib.attr & 0x10 ) {
			/*              1234567n */
			strcpy( ssize, "< DIR >");

			/* "."と".."は特別扱い */
			if(	(strcmp( ".", fib.name ) == 0) ||
				(strcmp( "..", fib.name ) == 0) ) {
				strcpy(  sname, "            " );
				strncpy( sname, fib.name, ilen );
			}

		} else {
			
			/* ファイルサイズを文字として取得 */
			sputsize( ssize, (SLONG *)(fib.filesize) );
		}

		/* ファイル日付の文字化 */
		if ( fib.fibdate == 0 ) {
			/* 日付がセットされていない*/
						  /* 12345678901234n */
			strcpy( sstamp, "-- no  data --" );

		} else{
			/* 日付がセットされている*/
			uiform = fib.fibtime >> 5;	/* 秒(0-4)は無視 */
			uinn   = uiform & 0x3f;		/* 分(5-10) */
			uihh   = uiform >> 6;		/* 時間(5-8) */

			uiform = fib.fibdate;
			uidd   = uiform & 0x1f;		/* 日(0-4) */
			uiform = uiform >> 5;
			uimm   = uiform & 0x0f;		/* 月(5-8) */
			uiyy   = (uiform >> 4) + 80;	/* 年(9-15=1980～2079) */
			
			sprintf( sstamp,
					"%02d-%02d-%02d %2d:%02d",
					uiyy % 100, uimm, uidd, uihh, uinn );
		}
/*
         1111111111222222222233333333334
1234567890123456789012345678901234567890
ABCDEFGH.TXTkkk,bbb yy-mm-dd hh:mm
*/
		/* ファイル名＋容量＋日付 */
		sprintf( files[ct].image, "%s%s %s", sname, ssize, sstamp );

		/* 次のエントリの検索 */
		iret = bdoscall( _FNEXT,
				0, 0, 0, 0,
				(unsigned)&fib,	/* 戻り値受取用 */
				&reg			/* 戻り値受取用 */
			);
		ct++;
	}
	files[ct].isalive = FALSE; /* 死活フラグ */
	
	/* File not found 以外のエラーの場合はメッセージを表示 */
	if ( iret != 0xd7 ) {	/* .NOFIL(File not found) */
		errmsgbox( iret, "getfiles()" );
	}

	return ct;
/*
		n = expargs( argc, argv, MAXFILES, files );
		if( n == ERROR ){
			puts("Error occured.");
			b_break = TRUE;
		}else{
			for( i=0; i<n; i++ ){
				printf( "%s	", files[i] );
			}
		}
*/
}



