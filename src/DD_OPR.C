/**************************************************************
	DD_OPR.c : ユーザ入力に対する各種処理
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
#include <math.h>			/* SLONG */
#include "dd.h"

/* dd_sub.cにて定義 */
extern char *makeshortpath( path, shortpath );

/* DD_EXEC.cにて定義 */
extern BOOL execbat( filename, image, path, bshift );
extern STATUS setenv( skey, svalue );

/* DD_COPY.cにて定義 */
extern STATUS tocopy( tabs, itabpage );
extern STATUS tomove( tabs, itabpage );
extern STATUS todelete( tabs, itabpage );

/* dd_scrn.cにて定義 */
extern VOID msgbox( x, y, stitle, stext );

/* DD_OPR2.cにて定義 */
extern STATUS chdrv( ch, tabs, itabpage );
extern STATUS tochdrv( tabs, itabpage );
extern int chuprdir( tabs, itabpage );
extern STATUS tomakedir( tabs, itabpage );
extern STATUS torename( tabs, itabpage );

/* DD_KEY.cにて定義 */
extern int marking( tab, itabpage );

extern VOID puthelp();



/**************************************************************
	リターンキー対応処理
	return値： メインループの状態を指示
	operation()→returnkey()
**************************************************************/
int returnkey( tabs, itabpage, keymod )
DD_TAB	tabs[];			/* r+w タブページ1枚を表現する構造体 */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
TINY	keymod;			/* r/o 修飾キー押下状態 */
{
	int		lsret = LS_LOOP;	/* return値 */
/*
if(keymod){
msgbox(0,0,"returnkey()","shift=ON");
}else{
msgbox(0,0,"returnkey()","shift=OFF");
}
*/
	/* ディレクトリであればその中に入る */
	if( tabs[itabpage].files[ tabs[itabpage].cursor ].attr & 0x10 ){
		
		/* ディレクトリが . である場合は無視 */
		if( strcmp( ".", tabs[itabpage].files[ tabs[itabpage].cursor ].name ) != 0 ) {
			puts("* <CHDIR>\n");
			strcat( tabs[itabpage].path, tabs[itabpage].files[ tabs[itabpage].cursor ].name );
			strcat( tabs[itabpage].path, "\\" );
			
			/* パス文字列の短縮形を作成する */
			makeshortpath( tabs[itabpage].path, tabs[itabpage].shortpath );
			
			/* 次回表示時にファイル一覧の取得が必要 */
			tabs[itabpage].status = TS_CHDIR;
		}

	} else {

		/* 拡張子連動実行 */
		if( execbat( &tabs[itabpage], &tabs[1-itabpage], keymod ) == OK ) {
/*
		if( execbat(
						tabs[itabpage].files[ tabs[itabpage].cursor ].image,
						tabs[itabpage].path,
						tabs[itabpage].files[ tabs[itabpage].cursor ].name,
						tabs[1-itabpage].path,
						tabs[1-itabpage].files[ tabs[1-itabpage].cursor ].name,
						keymod )
			 == OK ) {
*/			/* 外部コマンドの実行 後はバッチファイルに丸投げ */
			lsret = LS_EXEC;
		}
	}
	
	return lsret;
}


/**************************************************************
	マークされたファイルがない場合のみ
	カーソル位置のファイルをマークするよう試みる
	operation()→markatcursor()
	return値：	マークされたファイルの数
**************************************************************/
int markatcursor( tab, itabpage )
DD_TAB	*tab;			/* r+w 対象のタブページ */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	/* マークされたファイルがない場合 */
	if ( 0 == tab->markedct ) {
		/* カーソル位置のファイルをマークするよう試みる */
		marking( tab, itabpage, tab->cursor );
	}
	
	return tab->markedct;
}


/**************************************************************
	ユーザの指示に応じた処理を行う
	return値： メインループの状態を指示
	main()→operation()
**************************************************************/
int operation( ch, keymod, tabs, itabpage )
char	ch;				/* r/o ユーザからの入力 英文字は大文字で渡される */
TINY	keymod;			/* r/o 修飾キー押下状態 */
DD_TAB	tabs[];			/* r+w タブページ1枚を表現する構造体 */
int		*itabpage;		/* r+w 操作中のタブページ 0or1 */
{
	int		iret = 0;
	XREG	reg;				/* BDOSコールからの戻り値取得用 */
	int		lsret = LS_LOOP;	/* return値 */
	char	sdrv[4];			/* ルートディレクトリ */


	/* ファイル「..」の選択はBSキーに読み替える */
	if(	( KEY_RET == ch ) &&
		( tabs[*itabpage].files[ tabs[*itabpage].cursor ].attr & 0x10 ) && 
		( strcmp( "..", tabs[*itabpage].files[ tabs[*itabpage].cursor ].name ) == 0 ) ) {
		ch =(char)KEY_BS;
	}

	switch( ch ) {
			
		/* 終了 */
		case KEY_ESC:
		case 'Q':
			lsret = LS_END;		/* 単なる終了 */
			break;
		
		/* タブページの切り替え */
		case KEY_RIGHT:			/* →キー イン */
		case KEY_LEFT:			/* ←キー イン */
			*itabpage = 1 - *itabpage;
			lsret = LS_CHGTAB;
			break;

		/* 1-8→ドライブ変更 */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			chdrv( (TINY)(ch-'0'), tabs, *itabpage ); /* UIなし */
			break;
		
		/* ファイルのコピー */
		case 'C':
			if( 0 < markatcursor( &tabs[ *itabpage ], *itabpage ) ) {
				tocopy( tabs, *itabpage );
			}
			break;
			
		/* ファイルまたはサブディレクトリの削除 */
		case 'D':
		case KEY_DEL:
			if( 0 < markatcursor( &tabs[ *itabpage ], *itabpage ) ) {
				todelete( tabs, *itabpage );
			}
			break;
			
		/* ヘルプの表示 */
		case 'H':
			puthelp();
			break;

		/* サブディレクトリの作成 */
		case 'K':
			tomakedir( tabs, *itabpage );
			break;
			
		/* ドライブ変更 */
		case 'L':
			tochdrv( tabs, *itabpage ); /* UIあり */
			break;
			
		/* ファイルまたはサブディレクトリの移動 */
		case 'M':
			if( 0 < markatcursor( &tabs[ *itabpage ], *itabpage ) ) {
				tomove( tabs, *itabpage );
			}
			break;
			
		/* ファイルまたはサブディレクトリの名前変更 */
		case 'N':
			torename( tabs, *itabpage );
			break;
		
		/* 0→現在のドライブ・パスの再読込 */
		case 'R':
		case '0':
			/* 次回表示時にドライブの取得が必要 */
			tabs[*itabpage].status = TS_CHDRV;
			break;

		/* ルートディレクトリへ移動 */
		case 'T':
		case KEY_ROOT:
			/* d:\ の次をNULLにする */
			*(tabs[*itabpage].path + 3) = NULL;
			
			/* パス文字列の短縮形を作成する */
			makeshortpath( tabs[*itabpage].path, tabs[*itabpage].shortpath );
			
			/* 次回表示時にファイル一覧の取得が必要 */
			tabs[*itabpage].status = TS_CHDIR;
			
			break;
		
		/* 上のディレクトリへ移動 */
		case 'U':
		case KEY_BS:
			chuprdir( tabs, *itabpage );
			break;
			
		/* リターンキー対応処理 */
		case KEY_RET:
/*
if(b_shift){
msgbox(0,0,"operation()","shift=ON");
}else{
msgbox(0,0,"operation()","shift=OFF");
}
*/
			lsret = returnkey( tabs, *itabpage, keymod );
			break;
		
		default:
			break;
	}
	
	return lsret;
}

