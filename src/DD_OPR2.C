/**************************************************************
	DD_OPR.c : ユーザ入力に対する各種処理
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
/* #include <math.h>			 SLONG */
#include "dd.h"

/* dd_sub.cにて定義 */
extern int bdoscall( c, a, b, de, hl, ix, r );
extern STATUS bdos_trap( c, a, b, de, hl, ix, r, fname );
extern char *makeshortpath( path, shortpath );

/* dd_scrn.cにて定義 */
extern VOID msgbox( x, y, stitle, stext );

/* dd_input.cにて定義 */
extern VOID drvbox( stitle );
extern VOID inputbox( stitle );
extern STATUS inputline( str );

extern STATUS chdrv( ch, tabs, itabpage );



/**************************************************************
	ドライブの変更 UIあり
	return値：	OK:成功 ERROR:失敗
	operation()→tochdrv()
**************************************************************/
STATUS tochdrv( tabs, itabpage )
DD_TAB	tabs[];		/* r+w タブページ1枚を表現する構造体 */
int		itabpage;	/* r/o 操作中のタブページ 0or1 */
{
	char	ch;
	BOOL	bloop = TRUE;
	STATUS	sret = ERROR;
	
	
	/* 入力用ボックス描画 */
	drvbox( "CHANGE DRIVE" );
	
	/* 現在のドライブを表示 */
	locate( (TINY)17, (TINY)12 );
	putchar( tabs[itabpage].path[0] );
	locate( (TINY)17, (TINY)12 );

	do {
		ch = toupper( getch() );
		
		switch( ch ) {
			
			case 'A':	/* ドライブの変更 */
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
				sret = chdrv( (TINY)(ch - '@'), tabs, itabpage );
				bloop = FALSE;
				break;
				
			case KEY_ESC:	/* キャンセル */
				bloop = FALSE;
				break;
			
		}
		
	} while ( bloop );
	
	
	return sret;
}



/**************************************************************
	上位ディレクトリへ移動
	return値： エラーコード
	operation()→chuprdir()
**************************************************************/
int chuprdir( tabs, itabpage )
DD_TAB	tabs[];			/* r+w タブページ1枚を表現する構造体 */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	int		iret = 0;
	XREG	reg;				/* BDOSコールからの戻り値取得用 */
	char	pathwk[MAXPATH];	/* path作成用 */

	/* _PARSEするために最後の\を取る */
	strncpy( pathwk, tabs[itabpage].path, strlen(tabs[itabpage].path) -1 );
	pathwk[ strlen( tabs[itabpage].path ) - 1 ] = NULL;

	/* パス名の解析  */
	iret = bdoscall( _PARSE,
			0, 0, 
			(unsigned)pathwk,
			0, 0,
			&reg			/* 戻り値受取用 */
			);
	if ( reg.hl != reg.de ) {

		/* HL=文字列の最後の項目(「ファイル名」部分)の最初の文字 */
		/* なのでそこをNULLにすれば \ で止まる */
		*(char *)(reg.hl) = NULL;
		strcpy( tabs[itabpage].path, pathwk );
		
		/* パス文字列の短縮形を作成する */
		makeshortpath( tabs[itabpage].path, tabs[itabpage].shortpath );
		
		/* 次回表示時にファイル一覧の取得が必要 */
		tabs[itabpage].status = TS_CHDIR;
/*
	} else {
*/
		/* HL==DEの場合 */
		/* 文字列が「\」文字で終わっていたり、あるいはヌル(ドライブ名は別として) */
		/* 何もしない */
	}
	
	return iret;
}


/**************************************************************
	サブディレクトリの作成
	return値：	OK:成功 ERROR:失敗
	operation()→tomakedir()
**************************************************************/
STATUS tomakedir( tabs, itabpage )
DD_TAB	tabs[];			/* r/o タブページ1枚を表現する構造体 */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	char	pathwk[MAXPATH];	/* path作成用 */
	char	sname[MAXPATH];		/* 作成するディレクトリ名 */
	STATUS	sret = ERROR;		/* return値 */
	
	
	strcpy( sname, "" );
	
	/* 入力用ボックス描画 */
	inputbox( "MAKE DIR" );
	
	/* テキストボックス */
	if( OK == inputline( sname ) ) {
	
		/* 改行潰し */
/*
		if( isspace( sname[ strlen( sname )-1 ] )) {
			sname[ strlen( sname )-1 ] = '\0';
		}
*/		
		/* 1文字以上入力されていれば実行 */
		if( 0 < strlen( sname )) {
			strcpy( pathwk, tabs[ itabpage ].path );
			strcat( pathwk, sname );
			msgbox( 0, 0, "MKDIR", pathwk );
			
			/* サブディレクトリの作成 */
			sret = mkdir( pathwk ); 
			
			if ( OK == sret ) {
				/* 次回表示時にファイル一覧の取得が必要 */
				tabs[ itabpage ].status = TS_CHDIR;
			} else {
				msgbox( 0, 0, "MAKE DIR", "error cuured." );
			}
		}
	}
	
	return sret;
}


/**************************************************************
	ファイルまたはサブディレクトリの名前変更
	return値：	OK:成功 ERROR:失敗
	operation()→torename()
**************************************************************/
STATUS torename( tabs, itabpage )
DD_TAB	tabs[];			/* r/o タブページ1枚を表現する構造体 */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	char	pathwk[MAXPATH];	/* path作成用 */
	char	sname[MAXPATH];		/* 作成するディレクトリ名 */
	STATUS	sret = ERROR;		/* return値 */
	XREG	reg;				/* BDOSコールからの戻り値取得用 */
	
	
	/* デフォルト値 */
	strcpy( sname, tabs[itabpage].files[ tabs[itabpage].cursor ].name );

	/* 入力用ボックス描画 */
	inputbox( "RENAME" );
	
	/* テキストボックス */
	if( OK == inputline( sname ) ) {

		/* 1文字以上入力されていれば実行 */
		if( 0 < strlen( sname )) {
			strcpy( pathwk, tabs[ itabpage ].path );
			strcat( pathwk, tabs[itabpage].files[ tabs[itabpage].cursor ].name );
			
			/* 名前の変更 エラーを拾うためDOSコールで対応 */
			if ( ( sret = bdos_trap(
					_RENAME,
					0, 0,
					(unsigned)pathwk,	/* 現ファイル名 */
					(unsigned)sname,	/* 新ファイル名 */
					0,
					&reg,				/* 戻り値受取用 */
					"chdrv().10"
				) ) == OK ) {
				/* 次回表示時にファイル一覧の取得が必要 */
				tabs[ itabpage ].status = TS_CHDIR;
			}
	/*		sret = rename( pathwk, sname );
			if ( OK == sret ) {
				/* 次回表示時にファイル一覧の取得が必要 */
				tabs[ itabpage ].status = TS_CHDIR;
			} else {
				msgbox( 0, 0, "RENAME", "error cuured." );
			}
	*/
		}
	}
	
	return sret;
}


