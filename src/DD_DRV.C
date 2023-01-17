/**************************************************************
	DD_DRV.c :	ドライブ情報の取得
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>
#include <msxbios.h>
#include <math.h>
#include "dd.h"


/* dd_sub.cにて定義 */
extern int bdoscall( c, a, b, de, hl, ix, r );
extern STATUS bdos_trap( c, a, b, de, hl, ix, r, fname );
extern VOID bargraph( sfreebar, ipercent );

/* dd_scrn.cにて定義 */
extern VOID errmsgbox( icode, fname );

/* dd_drv.cにて定義 */
extern char *sputsize( str, psl );
extern char *addbackslash( s );
extern char *makeshortpath( path, shortpath );



/**************************************************************
	ドライブ情報の取得
	return値：	OK:成功 ERROR:失敗
	main()→getdriveinfo()
**************************************************************/
STATUS getdriveinfo( path, svollabel, scapa, sfreebar, itabpage )
char	*path;		/* r/o パス */
char	*svollabel;	/* w/o ボリュームラベルを返す */
char	*scapa;		/* w/o ファイル容量の表示イメージを返す */
char	*sfreebar;	/* freearea bargraph image  */
int 	itabpage;	/* r/o 操作中のタブページ 0or1 */
{
	int		id;			/* ドライブ番号 1=a: */
	int		iret = 0;	/* 戻り値取得用 */
	STATUS	sret = OK;	/* return値 */
	SLONG				/* MSX-C Libraryのlong型 */
			sldrvsize	/* ドライブの総容量 */
			,slrvfree	/* ドライブの空き容量 */
			,slwk1, slwk2, slwk3 /* SLONG型計算用ワーク */
			;
	char	sfree[8];
	XREG	reg;			/* BDOSコールからの戻り値取得用 */
	FIB		fib;

	
	id = (int)( *path - 'A' ) + 1;	/* カレントドライブの数値化 */

	bdoscall(_ALLOC,	/* アロケーション情報の獲得 */
		0, 0,
		id,				/* E←ドライブ番号 1=a: */
		0, 0,
		&reg			/* 戻り値受取用 */
		);	/* （戻り値には1クラスタあたりのセクタ数が入っている） */
/*
printf( "* <A:%d>\n", reg.af >> 8 );	/*1クラスタあたりのセクタ数*/
printf( "* <BC:%d>\n", reg.bc );	/*セクタサイズ(常に512)*/
printf( "* <DE:%d>\n", reg.de );	/*ディスク上のクラスタの総数*/
printf( "* <HL:%d>\n", reg.hl );	/*ディスク上の未使用クラスタ数*/
*/
	itosl( &slwk1, reg.bc );			/* slwk1←セクタ/クラスタ */
	itosl( &slwk2, (reg.af >> 8) );		/* slwk2←セクタサイズ */
	slmul( &slwk3, &slwk1, &slwk2 );	/* slwk3←BC*A */
	
	/*ドライブの総容量*/
	itosl( &slwk1, reg.de );				/* slwk1←クラスタ総数 */
	slmul( &sldrvsize, &slwk3, &slwk1 );	/* slwk3←(BC*A)*DE */
	
	/*ドライブの空き容量*/
	itosl( &slwk1, reg.hl );			/* slwk1←未使用クラスタ数 */
	slmul( &slrvfree, &slwk3, &slwk1 );	/* slwk3←(BC*A)*HL */

	/* 総容量 */
	sputsize( scapa, &sldrvsize );
	
	/* 空き容量 */
	sputsize( sfreebar, &slrvfree );

	/*空き容量を％で求める*/
	/*分子*100÷分母だとマイナス値になるので*/
	/*分子÷(分母÷100)する*/

	itosl( &slwk1, 100 );
	sldiv( &slwk2, &sldrvsize, &slwk1 );	/* 分母÷100 */
	sldiv( &slwk3, &slrvfree, &slwk2 );		/* 空き÷(容量/100) */
/*
	itosl( &slwk1, 100 );
	slmul( &slwk2, &slrvfree, &slwk1 );		/* slwk2←slrvfree*100 */
	sldiv( &slwk3, &slwk2, &sldrvsize );	/* slwk3←slrvfree*100/sldrvsize */
*/

/*
	locate( (TINY)(LIST_X + itabpage*22), (TINY)(LIST_Y - 1) );
	printf( "%s%3ld%%free", scapawk, &slwk3 );
*/

	/*使用量棒グラフ 使用量＝全容量-空き容量*/
/* 	bargraph( LIST_X + 7 + itabpage*22, LIST_Y - 1, 100 - *(int *)&slwk3 ); */
	bargraph( sfreebar, 100 - *(int *)&slwk3 );

	/* ボリュームラベルの取得 */
	/*                  12345678901n*/
	strcpy( svollabel, "( no name )" );	/* ボリュームラベル無し */
	iret = bdoscall( _FFIRST,
					0, 
					0x08,			/* ボリュームラベルの取得 */
					(unsigned)path,	/* パス */
					0,
					(unsigned)&fib,	/* 戻り値受取用 */
					&reg			/* 戻り値受取用 */
					);
	if ( 0 == iret ) {
		/* ボリュームラベルあり */
		strcpy( svollabel, fib.name );
	} else if ( 0xd7 == iret ) {	
		/* 0xd7=.NOFIL(File not found) */
		/* ボリュームラベルが見つからなかった */
		iret = 0; /* エラー解除 */
	} else {
		/* File not found 以外のエラーの場合はメッセージを表示 */
		errmsgbox( iret, "getdriveinfo()" );
		sret = ERROR;
	}

	/*ドライブ容量・空き容量 2行表示*/
/*
		locate( (TINY)LIST_X + 2, (TINY)(LIST_Y - 2) );
		printf( "%sBYTES", scapa );
		locate( (TINY)LIST_X + 2, (TINY)(LIST_Y - 1) );
		printf( "%sFREE", sfree );
*/
/*
		printf( "* <Total:[%10ld]->[%s]>\n", &sldrvsize, sarea );
		printf( "* <Free:[%10ld]->[%s]>\n", &slrvfree, sarea );
*/
	
	return sret;
}



/**************************************************************
	ドライブの変更
	return値：	OK:成功 ERROR:失敗
	operation()→chdrv()←tochdrv()
**************************************************************/
STATUS chdrv( idrv, tabs, itabpage )
TINY	idrv;			/* r/o カレントドライブ 1=a: */
DD_TAB	tabs[];		/* r+w タブページ1枚を表現する構造体 */
int		itabpage;	/* r/o 操作中のタブページ 0or1 */
{
	STATUS	sret = ERROR;		/* return値 */
	char	pathwk[MAXPATH];	/* path作成用 */
	XREG	reg;				/* BDOSコールからの戻り値取得用 */


	/* 指定ドライブのカレントディレクトリの獲得  */
	/* （ドライブ名と前後の \ は含まれない） */
	if ( ( sret = bdos_trap(
			_GETCD,
			0, 
			(int)idrv,				/* ドライブ 1=a: */
			(unsigned)pathwk,	/* 戻り値受取用 */
			0, 0,
			&reg,				/* 戻り値受取用 */
			"chdrv().10"
		) ) == OK ) {
		
		/* エラーでなければ */
		
		/* ドライブ名と前後の \ を付加 */
		strcpy( tabs[itabpage].path, "*:\\" );
		tabs[itabpage].path[0] = (char)('@'+idrv);		/* ドライブ名 */
		strcat( tabs[itabpage].path, pathwk );
		/* 最後に\を足す（無条件に足すとルートディレクトリで\が重複する） */
		addbackslash( tabs[itabpage].path );

		/* パス文字列の短縮形を作成する */
		makeshortpath( tabs[itabpage].path, tabs[itabpage].shortpath );
		
		/* 次回表示時にドライブ情報の取得が必要 */
		tabs[itabpage].status = TS_CHDRV;
		
	}
	
	return sret;
}



