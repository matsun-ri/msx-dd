/**************************************************************
	DD_COPY.c : ファイルコピー・移動処理
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
#include "dd.h"

/* dd_sub.cにて定義 */
extern int bdoscall( c, a, b, de, hl, ix, r );
extern STATUS bdos_trap( c, a, b, de, hl, ix, r, fname );

/* dd_scrn.cにて定義 */
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );
extern VOID putbox( x, y, w, h );
extern VOID msgbox( x, y, stitle, stext );
extern VOID progressbox( stitle );
extern VOID putprogress( stext, idenomi, inum );

extern int marking( tab, itabpage, index );


/**************************************************************
	ファイルのコピー（実際に行なう）
	return値：	OK:成功 ERROR:失敗
	tocopy()→onecopy()
**************************************************************/
STATUS onecopy( ssrc, sdest )
char	*ssrc;	/* r/o コピー元ファイルパス＋名前 */
char	*sdest;	/* r/o コピー先ファイルパス＋名前 */
{
	char	buffer[1024];		/* コピー用バッファ */
	int		ilen = 0;			/* コピー用サイズ */
	int		iret = 0;			/* return値受取用 */
	unsigned	uattr = 0;		/* ファイル属性 */
	unsigned	udate = 0;		/* ファイル日付 */
	unsigned	utime = 0;		/* ファイル時刻 */
	STATUS	sret = OK;			/* return値 */
	FD		fdsrc, fddest;		/* 低水準入出力のファイルハンドル */
	XREG	reg;				/* BDOSコールからの戻り値取得用 */


	fdsrc = NULL;
	fddest = NULL;
	
	/* コピー元ファイルのオープン 読込専用 */
	fdsrc = open( ssrc, O_RDONLY );
	if ( ERROR == fdsrc ) {
		fdsrc = NULL;
		/* エラー */
		msgbox( 0, 0, "FILE OPEN ERROR", ssrc );
		return ERROR;
	}
	
	/* コピー先ファイルの作成 書込専用 */
	fddest = creat( sdest );	/* 既に存在するファイルは上書きされる */
	if ( ERROR == fddest ) {
		fddest = NULL;
		/* エラー */
		close( fdsrc );			/* コピー元ファイルを閉じておく */
		fdsrc = NULL;
		msgbox( 0, 0, "FILE CREAT ERROR", sdest );
		return ERROR;
	}
	
	/* コピー処理 */
	do {
		/* 読み込み */
		ilen = read( fdsrc, buffer, 1024 );
		
		/* 読めれば（まだEOFでなければ） */
		if ( 0 != ilen ) {
			
			/* 書き込み */
			if( 0 == write( fddest, buffer, ilen ) ) {
				/* 書き込みエラー */
				msgbox( 0, 0, "FILE WRITE ERROR", sdest );
				sret = ERROR;
			}
		}
		
		/*  */
	} while ( (0 != ilen) && ( OK == sret ) );
	
	/* コピー元ファイルのクローズ */
	if( ERROR == close( fdsrc ) ) {
		msgbox( 0, 0, "FILE CLOSE ERROR", ssrc );
		sret = ERROR;
	}
	fdsrc = NULL;

	/* コピー先ファイルのクローズ */
	if( ERROR == close( fddest ) ) {
		msgbox( 0, 0, "FILE CLOSE ERROR", sdest );
		sret = ERROR;
	}
	fddest = NULL;
	
	/* ここまでの処理でエラーがなければ */
	/* タイムスタンプと属性をコピーする */
	if( OK == sret ) {

		/* コピー元ファイルからタイムスタンプを拾う */
		if ( ( sret = bdos_trap(
						 _FTIME, /* ファイルの日付および時刻の獲得・セット */
						0,		/* 日付と時刻の獲得 */
						0,
						(unsigned)ssrc,	/* ドライブ・パス・ファイル */
						0,
						0,
						&reg,	/* 戻り値受取用 */
						"onecopy().10"
			) )  == OK ) {

			/* エラーでなければ */

			/* タイムスタンプを拾う */
			utime = reg.de;
			udate = reg.hl;

			/* コピー先ファイルへ日付・時刻をセット */
			if ( ( sret = bdos_trap( _FTIME, /* ファイルの日付および時刻の獲得・セット */
							1,				/* 日付と時刻のセット */
							0,
							(unsigned)sdest, /* ドライブ・パス・ファイル */
							udate,			/* 日付 */
							utime,			/* 時刻 */
							&reg,			/* 戻り値受取用 */
							"onecopy().20"
				) )  == OK ) {

				/* ここまでの処理でエラーがなければ */
				/* ファイル属性をコピーする */

				/* コピー元ファイルから属性を獲得 */
				if ( ( sret = bdos_trap(
						_ATTR,	/* ファイル属性の獲得・セット */
						0,				/* 属性の獲得 */
						0,
						(unsigned)ssrc,	/* ドライブ・パス・ファイル */
						0,
						0,
						&reg,			/* 戻り値受取用 */
						"onecopy().30"
					) )  == OK ) {
					
					/* エラーでなければ */
					
					/* ファイル属性を拾う */
					uattr = reg.hl & 0xff;	/* L=現在の属性 */
					
					/* コピー先ファイルへ属性をセット */
					sret = bdos_trap(
							_ATTR,
							1,				/* 属性のセット */
							0,
							(unsigned)sdest, /* ドライブ・パス・ファイル */
							uattr,			/* L←新しい属性(1byte) */
							0,
							&reg,			/* 戻り値受取用 */
							"onecopy().40"
						);
				}
			}
		}
	}

	return sret;
}


/**************************************************************
	ファイルのコピー（前処理）
	return値：	OK:成功 ERROR:失敗
	operation()→tocopy()
	前提条件：
		・1つ以上のファイルがマークされていること
		・ディレクトリに対しては何もしない。エラーも出さない
**************************************************************/
STATUS tocopy( tabs, itabpage )
DD_TAB	tabs[];			/* r/o タブページ1枚を表現する構造体 */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	int		i = 0;
	int		ict  = 0;	/* 処理済み件数 */
	int		imax = 0;	/* 処理対象件数 */
	char	ssrc[MAXPATH];		/* 移動元 */
	char	sdest[MAXPATH];		/* 移動先 */
	STATUS	sret = OK;			/* return値 */


	/* コピー元パスとコピー先パスが同じでないか確認 */
	if ( 0 == strcmp( tabs[itabpage].path, tabs[1 - itabpage].path ) ) {
		msgbox( 0, 0, "ERROR", "file can't copy onto itself." );
		return ERROR;
	}
	

	/* プログレス表示の準備 */
	progressbox( "COPY" );
	imax = tabs[itabpage].markedct;

	/* コピー先   の次回表示時にファイル一覧の取得が必要 */
	tabs[ 1 - itabpage ].status = TS_CHDIR;

	for ( i=0; i<tabs[itabpage].count; i++ ){

		/* マークされているファイルを探す */
		if ( TRUE == tabs[itabpage].files[i].ismarked ) {
			
			/* コピー元パス */
			strcpy( ssrc, tabs[itabpage].path );
			strcat( ssrc, tabs[itabpage].files[i].name );
			
			/* コピー先パス */
			strcpy( sdest, tabs[1 - itabpage].path );
			strcat( sdest, tabs[itabpage].files[i].name );
			
			/* プログレス表示 */
			putprogress( tabs[itabpage].files[i].name, imax, ict );

			/* ディレクトリでなければコピー */
			if( tabs[itabpage].files[i].attr & 0x10 ) {

				/* ディレクトリはスキップ */

			} else {

				/* ファイルのコピー */
				sret = onecopy( ssrc, sdest );

				if ( OK == sret ) {

					/* 処理したファイルのマークを解除する */
					marking( &tabs[itabpage], itabpage, i );

				} else {
					/* エラーの場合はメッセージを表示 */
					msgbox( 0, 0, "tocopy", "error cuured." );
					break;
				}
			}
			
			ict++;
			
			/* マークされているファイルをすべて処理し終わった */
			if ( imax == ict ) break;
			
		}
	}
	
	return sret;
}


/**************************************************************
	ファイルまたはサブディレクトリの移動
	・同じドライブならファイルもディレクトリも移動
	・違うドライブならファイルのみコピー＋削除。ディレクトリは処理しない
	return値：	OK:成功 ERROR:失敗
	operation()→tomove()
**************************************************************/
STATUS tomove( tabs, itabpage )
DD_TAB	tabs[];			/* r/o タブページ1枚を表現する構造体 */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	int		i = 0;
	int		ict  = 0;			/* 処理済み件数 */
	int		imax = 0;			/* 処理対象件数 */
	char	ssrc[MAXPATH];		/* 移動元 */
	char	sdest[MAXPATH];		/* 移動先 */
	STATUS	sret = OK	;		/* return値 */
	XREG	reg;				/* BDOSコールからの戻り値取得用 */
	char ch1, ch2;


	/* ドライブレターを取り出す */
	ch1 = tabs[itabpage].path[0];
	ch2 = tabs[1 - itabpage].path[0];

	/* プログレス表示の準備 */
	if( ch1 == ch2 ) {
		progressbox( "MOVE on same" );
	} else {
		progressbox( "MOVE to other" );
	}
	imax = tabs[itabpage].markedct;
	
	/* 移動元の次回表示時にファイル一覧の取得が必要 */
	tabs[ itabpage ].status = TS_CHDIR;
	/* 移動先の次回表示時にファイル一覧の取得が必要 */
	tabs[ 1 - itabpage ].status = TS_CHDIR;

	/* 移動先パス（ドライブ名込み） */
	strcpy( sdest, tabs[1 - itabpage].path );

	for ( i=0; i<tabs[itabpage].count; i++ ){

		/* マークされているファイルを探す */
		if ( TRUE == tabs[itabpage].files[i].ismarked ) {
			
			/* 移動元パス */
			strcpy( ssrc, tabs[itabpage].path );
			strcat( ssrc, tabs[itabpage].files[i].name );
			
			
			/* プログレス表示 */
			putprogress( tabs[itabpage].files[i].name, imax, ict );

			/* 移動元と移動先のドライブレターは同じか？ */
			if( ch1 == ch2 ) {
				
				/* ファイルあるいはサブディレクトリの移動 */
				if ( ( sret = bdos_trap(
						_MOVE,
						0,
						0,
						(unsigned)ssrc,			/* ドライブ・パス・ファイル */
						(unsigned)sdest + 2,	/* 新しいパス ドライブと：を飛ばす */
						0,
						&reg,				/* 戻り値受取用 */
						"tomove().10"
					) )  == OK ) {

					/* エラーでなければ */
					
				} else {
					
					break;
					
				}
				
			} else {

				/* 違うドライブならコピー＋削除 */
				
				/* ファイル？ ディレクトリ？ */
				if(	tabs[itabpage].files[i].attr & 0x10 ) {

					/* ディレクトリには何もしない */

				} else {

					/* ファイルなら処理対象 */

					/* コピー先パス */
					strcpy( sdest, tabs[1 - itabpage].path );
					strcat( sdest, tabs[itabpage].files[i].name );

					/* ファイルのコピー */
					sret = onecopy( ssrc, sdest );
					if ( OK == sret ) {
					
						/* ファイルの削除 */
						sret = unlink( ssrc );
						if ( OK == sret ) {

						} else {
							/* エラーの場合はメッセージを表示 */
							msgbox( 0, 0, "MOVE DELETE PHASE", "error cuured." );
							break;
						}
					
					} else {
						/* エラーの場合はメッセージを表示 */
						msgbox( 0, 0, "MOVE COPY PHASE", "error cuured." );
						break;
					}
				}
			}

			/* 処理したファイルのマークを解除する */
			marking( &tabs[itabpage], itabpage, i );
			
			ict++;
			
			/* マークされているファイルをすべて処理し終わった */
			if ( imax == ict ) break;
		}
	}
	
	return sret;
}


/**************************************************************
	ファイルまたはサブディレクトリの削除
	return値：	OK:成功 ERROR:失敗
	operation()→todelete()
**************************************************************/
STATUS todelete( tabs, itabpage )
DD_TAB	tabs[];			/* r/o タブページ1枚を表現する構造体 */
int		itabpage;		/* r/o 操作中のタブページ 0or1 */
{
	int		i = 0;
	int		ict  = 0;	/* 処理済み件数 */
	int		imax = 0;	/* 処理対象件数 */
	char	pathwk[MAXPATH];	/* path作成用 */
	STATUS	sret = OK;			/* return値 */
	XREG	reg;				/* BDOSコールからの戻り値取得用 */


	/* プログレス表示の準備 */
	progressbox( "DELETE" );
	imax = tabs[itabpage].markedct;
	
	for ( i=0; i<tabs[itabpage].count; i++ ){

		/* マークされているファイルを探す */
		if ( TRUE == tabs[itabpage].files[i].ismarked ) {

			/* プログレス表示 */
			putprogress( tabs[itabpage].files[i].name, imax, ict );

			strcpy( pathwk, tabs[itabpage].path );
			strcat( pathwk, tabs[itabpage].files[i].name );
			
			/* ファイル・サブディレクトリの削除 */
			if ( ( sret = bdos_trap(
					_DELETE,
					0,
					0,
					(unsigned)pathwk,	/* ドライブ・パス・ファイル */
					0,
					0,
					&reg,				/* 戻り値受取用 */
					"todelete().10"
				) )  == OK ) {
				
				/* エラーでなければ */
				
			} else {
				
				break;
				
			}
			
			/* 処理したファイルのマークを解除する */
			marking( &tabs[itabpage], itabpage, i );
			
			ict++;
			
			/* マークされているファイルをすべて処理し終わった */
			if ( imax == ict ) break;
		}
	}
	
	/* 次回表示時にファイル一覧の取得が必要 */
	tabs[ itabpage ].status = TS_CHDIR;
	
	return sret;
}


