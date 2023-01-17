/**************************************************************
	DD_EXEC.c : 外部プログラム実行
**************************************************************/
/*
★外部プログラム実行の流れ

・D.BAT（仮）		：中身は固定
	DDX.com %1 %2
	%DD_EXEC%	外部プログラム実行時は DDG.bat・本プログラムの終了時は空文字列
						
・BATFILE(DDG.BAT)	：適宜DDX.comが出力
	d:					カレントドライブを変更
	cd d:\src\			カレントディレクトリを変更
	notepad hoe.c		実行内容
	PAUSE				$KがあればPAUSE なければNULL
	C:					カレントドライブを戻す
	C:\MSXC\LIB\		カレントディレクトリを戻す
	DD.BAT D:\SRC\ A:\	DD.BATに、さっき表示していたパスを2タブ分渡す
*/

#include <stdio.h>
/* XREG, FIB */
#include <bdosfunc.h>
/* locate() */
#include <msxbios.h>
/* execlp() */
#include <process.h>
#include "dd.h"

/* parsemacro用定数 */
#define PM_FOUND_C	(TINY)0x01	/* ファイル名置き換えマクロを発見 */
#define PM_FOUND_K	(TINY)0x02	/* 実行後キー入力待ちマクロを発見 */

/* dd_sub.cにて定義 */
extern int bdoscall( c, a, b, de, hl, ix, r );

/* dd_scrn.cにて定義 */
extern VOID msgbox( x, y, stitle, stext );


/**************************************************************
	環境変数から自身のパスを取得
	return値：	TRUE : パス名が解析できた
				FALSE: できなかった
	execbat()→getmypath()
**************************************************************/
BOOL getmypath( shell, mine, mypath )
char *shell;	/* w/o COMMAND2.COMのパス＋ファイル名 */
char *mine;		/* w/o 自分自身のパス＋ファイル名 */
char *mypath;	/* w/o ↑のパス部分のみ */
{
	char	*s;					/* 環境変数取得用*/
	BOOL	bret;
	XREG	reg;


	bret = FALSE;
	
	/* 環境変数 %SHELL% の取得 勝手にmallocするのでfreeすること */
	s = getenv( ENVSHELL );
	strcpy( shell, s );
	free( s );
	
	/* 環境変数 %PROGRAM% の取得 勝手にmallocするのでfreeすること */
	s = getenv( ENVPRGM );
	strcpy( mine, s );	/* mine=c:\hoehoe\dd.com */
	free( s );
	
	strcpy( mypath, mine );
	
	/* パス名の解析  */
	bdoscall( _PARSE,
			0, 0, 
			(unsigned)mypath,
			0, 0,
			&reg			/* 戻り値受取用 */
			);

	if ( reg.hl == reg.de ) {
		/* HL==DEの場合 */
		/* 文字列が「\」文字で終わっていたり、あるいはヌル(ドライブ名は別として) */
		/* とりあえずファイル名を設定 */
		strcpy( mypath, mypath );
	} else {
		/* HL=文字列の最後の項目(「ファイル名」部分)の最初の文字 */
		/* なのでそこをNULLにすれば \ で止まる */
		/* mine=c:\hoehoe\dd.com mypath=c:\hoehoe\ */
		*(char *)(reg.hl) = NULL;

		bret = TRUE;
	}
	
	return bret;
}


/**************************************************************
	拡張子に対するコマンドラインをコンフィグファイルから得る
	return値：	TRUE : 取得成功
				FALSE: 見つからなかった
	execbat()→getmypath()
**************************************************************/
BOOL getexecfile( key, cfgfile, cmdline )
char	*key;		/* r/o 検索対象の拡張子 RET=小文字 SHIFT+RET=大文字 */
char	*cfgfile;	/* r/o .cfgファイル名 */
char	*cmdline;	/* w/o  取得したコマンドライン */
{
	int		i, ilen, ict;
	char 	sline[80]	/* コンフィグファイルの1行がそのまま入る */
	    ,	slinekey[4]	/* コンフィグファイル上の拡張子部分 */
			;
	char	*pline1, *pline2, *pline3;	/* コンフィグファイル解析用 */
	BOOL	bfind = FALSE;	/* コンフィグファイルに該当する拡張子を見つけた */
	FILE	*fp;


	strcpy( slinekey, "" );

	fp = fopen( cfgfile, "r" );
	if ( NULL == fp ) {
		
		/* ファイルが見つからない */
		msgbox( 0, 0, "CONFIG FILE NOT FOUND", cfgfile );
		
	} else {
		
		/* 目的の拡張子が見つかるまで検索 */
		while ( (fgets( sline, 40, fp ) != NULL) && (bfind == FALSE) ) {
			
			if( (';' == sline[0]) || (0x0a == sline[0]) ){

				/* 行頭がいきなりセミコロンまたは改行コード→読み飛ばす */
				/* （一番多いパターンと想定されるので特別扱い） */

			} else {

				/* 読み込んだ行を解析 */

				/*            11111
					012345678901234
				      cpp    vsmall
				      pline1 先頭の空白を無視した最初の文字（拡張子を想定）
				        pline2 その次の最初の空白（デリミタを想定）
				             pline3 その次の最初の文字（コマンドラインを想定）
				*/
				
				ilen = strlen( sline );
				pline1 = sline;
				
				/* 先頭の空白を読み飛ばす */
				while( (pline1 < sline+ilen) && isspace( *pline1 ) ) {
					pline1++;
				}
/*
				if( ';' == *pline1 ) {
					/* 先頭が空白＋セミコロン→読み飛ばす */
				} else {
*/
				if( '.' == *pline1 ) {
					
					pline1++;
					pline2 = pline1;
					
					/* 拡張子を（次の空白まで）読み飛ばす */
					while( (pline2 < sline+ilen) && (isspace(*pline2)==FALSE) ) {
						 pline2++;
					}
					pline3 = pline2;
					
					/* 次の空白（拡張子とコマンドラインのデリミタ）を読み飛ばす */
					while( (pline3 < sline+ilen) && isspace( *pline3 ) ) {
						pline3++;
					}
					
					/* 読み込んだ行から拡張子を抽出 */
					slinekey[0] = '\0';
					slinekey[1] = '\0';
					slinekey[2] = '\0';
					slinekey[3] = '\0';
					ict = pline2 - pline1; /* 拡張子部分 */
					if ( 3 < ict ) ict = 3;
					for( i=0; i<ict; i++ ){ 
						/* slinekey[]←拡張子 1～3文字 */
						/* 大文字・小文字はそのまま */
						slinekey[i] = *(pline1 + i);
					}
/*
sprintf(stmp, "1:%d 2:%d 3:%d slinekey=%s",
	(unsigned)(pline1-sline),
	(unsigned)(pline2-sline),
	(unsigned)(pline3-sline),
	slinekey
	);
msgbox( 0, 0, skey, stmp );
*/
					/* 関数に与えられた拡張子と、長さも含め完全一致していれば */
					if ( strcmp( slinekey, key ) == 0 ){
						/* コンフィグファイルに掛かれているコマンドライン */
						strcpy( cmdline, pline3 );
						bfind = TRUE;
						break;
					}
				}
			}
		}
		fclose( fp );
		fp = NULL;
	}
	
	return bfind;
}



/**************************************************************
	環境変数の登録
	return値：	OK:成功 ERROR:失敗
**************************************************************/
STATUS setenv( skey, svalue )
char	*skey;		/* r/o 環境変数名 */
char	*svalue;	/* r/o 値 */
{
	char swork[MAXPATH];
	
	sprintf( swork, "%s=%s", skey, svalue );
	
	return putenv( swork );
}



/**************************************************************
	マクロの展開
	return値：	OK    マクロを処理した
				ERROR マクロではなかった
	execbat()→parsemacro()
**************************************************************/
STATUS parsemacro( ch, srcpath, srcfile, dstpath, dstfile, sfullcmd, pfound, pmf )
char ch;			/* r/o マクロ文字 大文字で渡される */
char *srcpath;		/* r/o カレントタブのパス */
char *srcfile;		/* r/o カレントタブのファイル名 */
char *dstpath;		/* r/o もう一方のタブのパス */
char *dstfile;		/* r/o もう一方のタブのファイル名 */
char *sfullcmd;		/* w/o コマンドライン生成用 */
char **pfound;		/* r+w 処理位置 */
TINY *pmf;			/* r+w 処理済みフラグ */
{
	STATUS sret = OK;
	
	
	switch( ch ) {
		
		case 'C':	/* $C:ファイル名置き換えマクロ */
			*(*pfound) = '\0';		/* 一旦文字列を閉じる */
			/* ファイル名を追加 */
			strcat( sfullcmd, srcfile );
			*(pfound) += strlen( srcfile );
			*pmf = *pmf | PM_FOUND_C;	/* $C見つけたよ */
/*
			bmc = TRUE;		/* $C見つけたよ */
*/
			break;
		
		case 'D':	/* $D:別のタブのカーソル位置のファイル名 */
			*(*pfound) = '\0';		/* 一旦文字列を閉じる */
			/* ファイル名を追加 */
			strcat( sfullcmd, dstfile );
			*(pfound) += strlen( dstfile );
			break;
		
		case 'K':	/* $K:実行後キー入力待ちマクロ */
			*pmf = *pmf | PM_FOUND_K;	/* $K見つけたよ */
/*
			bmk = TRUE;		/* $K見つけたよ */
*/
			break;

		case 'P':	/* $P:カレントタブのパス */
			*(*pfound) = '\0';		/* 一旦文字列を閉じる */
			/* パスを追加 */
			strcat( sfullcmd, srcpath );
			*(pfound) += strlen( srcpath );
			break;

		case 'Q':	/* $Q:別のタブのパス */
			*(*pfound) = '\0';		/* 一旦文字列を閉じる */
			/* ファイル名を追加 */
			strcat( sfullcmd, dstpath );
			*(pfound) += strlen( dstpath );
			break;
		
		default:	/* ddのマクロ文字ではない */
		
			sret = ERROR;
	}
	
	return sret;
}


/**************************************************************
	外部プログラムの実行
	return値：	OK   : 外部プログラムの実行準備完了
				ERROR: 準備失敗（コンフィグの読込に失敗など）
	operation()→execbat()
**************************************************************/
/*
STATUS execbat( image, srcpath, srcfile, dstpath, dstfile, keymod )
char *image;	/* r/o カレントタブの表示イメージ ※拡張子だけを使う */
char *srcpath;	/* r/o カレントタブのパス */
char *srcfile;	/* r/o カレントタブのファイル名 */
char *dstpath;	/* r/o もう一方のタブのパス */
char *dstfile;	/* r/o もう一方のタブのファイル名 */
TINY  keymod;	/* r/o 修飾キー押下状態 */
*/
STATUS execbat( srctab, dsttab, keymod )
DD_TAB	*srctab;	/* r/o カレントタブページ */
DD_TAB	*dsttab;	/* r/o 非カレントタブページ */
TINY	keymod;		/* r/o 修飾キー押下状態 */
{
	/* コンフィグファイル検索系 */
	int		iret;
	char	shell[MAXPATH];		/* COMMAND2.COMのパス＋ファイル名 */
	char	mine[MAXPATH];		/* 自分自身のパス＋ファイル名 */
	char	smypath[MAXPATH];		/* 自分自身のパスのみ */
	char	cfgfile[MAXPATH];	/* .cfgファイル名 */
	char	batfile[MAXPATH];	/* .batファイル生成用 */
	char	cmdline[MAXPATH];	/* コマンドライン */
	
	/* バッチファイル作成系 */
	TINY	pmf = 0;			/* マクロ発見フラグ */
	int		i = 0;
	int		ilen = 0;
	char	skey[4]				/* 検索する拡張子 */
		,	scmddrv[3]			/* 実行対象ドライブ */
		,	smydrv[3]			/* DDの位置するドライブ */
		,	spause[8]			/* pauseコマンド */
		,	sfullcmd[MAXPATH]	/* コマンドライン生成用 */
		;
	char	*pfound;			/* 処理位置 */
	FILE	*fp;
	STATUS	stret = ERROR;
	

	strcpy( spause, "" );

	/* 環境変数から自身のパスを取得 */
	if( getmypath( shell, mine, smypath ) ) {
		/* パス名が解析できた */
		strcpy( cfgfile, smypath );		/* cfgfile=c:\hoehoe\ */
		strcat( cfgfile, CFGFILE );		/*                  + dd.cfg */
		strcpy( batfile, smypath );		/* batfile=c:\hoehoe\ */
		strcat( batfile, BATFILE );		/*                  + ddg.bat */
	} else {
		/* パス名が解析できなかった*/
		strcpy( batfile, BATFILE );
	}

	/* 拡張子を取得する（すでに大文字） */
	skey[0] = (srctab->files[ srctab->cursor ].image[9]==0x20 ? NULL:srctab->files[ srctab->cursor ].image[9]);
	skey[1] = (srctab->files[ srctab->cursor ].image[10]==0x20 ? NULL:srctab->files[ srctab->cursor ].image[10]);
	skey[2] = (srctab->files[ srctab->cursor ].image[11]==0x20 ? NULL:srctab->files[ srctab->cursor ].image[11]);
	skey[3] = '\0';
/*
locate((TINY)1,(TINY)5);
printf("* <execbat():%d>", (int)keymod );
*/
	/* Shiftキーが押されていない場合は拡張子を小文字化する */
	if ( ( keymod & KM_SHIFT ) == 0 ) {
		/*  大文字のまま */
/*
msgbox( 0, 0, "execbat()", "shift ON" );
*/
	} else {
/*
msgbox( 0, 0, "execbat()", "shift OFF" );
*/
		/*  小文字に変換 */
		strlwr( skey );
	}

	/* 拡張子に対するコマンドラインをコンフィグファイルから得る */
	if( getexecfile( skey, cfgfile, cmdline ) ) {
	
		/* 拡張子に対応するコマンドが見つかった */

		/* 改行を潰す */
		if( 0x0a == cmdline[ strlen(cmdline)-1 ] ) {
			cmdline[ strlen(cmdline)-1 ] = '\0';
		}
		
		/* 1文字ずつ転記しつつマクロ($x)の検索 */
/*
		bmc = FALSE;
		bmk = FALSE;
*/
		pfound = sfullcmd;
		i = 0;

		while ( i <= strlen( cmdline ) ) { /* 最後の\0まで転記させる */

			if( isspace( cmdline[i] ) ) {
				/* スペースと紛らわしい文字はすべてスペースに変換 */
				*pfound = 0x20;
				pfound++;
				
			} else if( '$'==cmdline[i] ) {
				
				/* $を発見したら次の文字をチェック */
				if ( parsemacro( toupper(cmdline[i+1]),
							srctab->path, 
							srctab->files[ srctab->cursor ].name, 
							dsttab->path, 
							dsttab->files[ dsttab->cursor ].name,
							sfullcmd,
							&pfound,
							&pmf
						) == OK ) {
					
					/* 次の文字は読み飛ばす  */
					i++;
					
				} else {
					
					/* 該当するマクロがなかった→そのまま転記する */
					*pfound = cmdline[i];
					pfound++;
					
				}
/*				
				if ( 'C'==toupper(cmdline[i+1]) ) {
					/* $C:ファイル名置き換えマクロ */
					*pfound = '\0';		/* 一旦文字列を閉じる */
					/* ファイル名を追加 */
					strcat( sfullcmd, srctab->files[ srctab->cursor ].name );
					pfound += strlen( srctab->files[ srctab->cursor ].name );
					bmc = TRUE;		/* $C見つけたよ */
					i++; /* 次の文字は読み飛ばす  */
					
				} else if ( 'D'==toupper(cmdline[i+1]) ) {
					/* $D:別のタブのカーソル位置のファイル名 */
					*pfound = '\0';		/* 一旦文字列を閉じる */
					/* ファイル名を追加 */
					strcat( sfullcmd, dsttab->files[ dsttab->cursor ].name );
					pfound += strlen( dsttab->files[ dsttab->cursor ].name );

					i++; /* 次の文字は読み飛ばす  */
					
				} else if( 'K'==toupper(cmdline[i+1]) ) {
					/* $K:実行後キー入力待ちマクロ */
					bmk = TRUE;		/* $K見つけたよ */
					i++; /* 次の文字は読み飛ばす  */

				} else if( 'P'==toupper(cmdline[i+1]) ) {
					/* $P:カレントタブのパス */
					*pfound = '\0';		/* 一旦文字列を閉じる */
					/* パスを追加 */
					strcat( sfullcmd, srctab->path );
					pfound += strlen( srctab->path );

					i++; /* 次の文字は読み飛ばす  */

				} else if( 'Q'==toupper(cmdline[i+1]) ) {
					/* $Q:別のタブのパス */
					*pfound = '\0';		/* 一旦文字列を閉じる */
					/* ファイル名を追加 */
					strcat( sfullcmd, dsttab->path );
					pfound += strlen( dsttab->path );

					i++; /* 次の文字は読み飛ばす  */

				} else {
					/* ddのマクロ文字ではない→そのまま$を転記 */
					*pfound = cmdline[i];
					pfound++;
				}
*/
			} else {
				/* そのまま転記 */
				*pfound = cmdline[i];
				pfound++;
			}
			i++;
		}

		/* $Cがなければターゲットファイル名を後ろに追加 */
		if( 0 == (pmf & PM_FOUND_C) ) {
			strcat( sfullcmd, " " );
			strcat( sfullcmd, srctab->files[ srctab->cursor ].name );
		}
		
/* -------------------------------------- 環境変数＋バッチファイルの場合 */
/*
		/* 環境変数に実行内容を出力 */

		/* カレントドライブ生成 */
		strcpy( sdrv, "*:" );
		strncpy( sdrv, srctab->path, 2);
		
		setenv( "DD_EXEC1", sdrv );			/* カレントドライブを変更 */
		setenv( "DD_EXEC2", srctab->path );		/* カレントディレクトリを変更 */
		setenv( "DD_EXEC3", sfullcmd );		/* コマンドライン */
		if( bmk ) {							/* $Kがあれば実行後キー待ちする */
			setenv( "DD_EXEC4", "pause" );
		} else {
			setenv( "DD_EXEC4", "" );
		}

		/* DDの位置するドライブ生成 */
		strcpy( sdrv, "*:" );
		strncpy( sdrv, mine, 2);
		
		setenv( "DD_EXEC5", sdrv );			/* カレントドライブを戻す */
		setenv( "DD_EXEC6", smypath );		/* カレントディレクトリを戻す */
		
		/* バッチファイル名（プログラム終了時=""） */
		setenv( "DD_EXEC7", BATFILE );		

		/* バッチファイルでDDに戻る */
		stret = OK;
*/
/*
	5～7は c: | cd c:\msxc\lib\ | ddg.bat c:\work でいける
	1～2もたぶんいける
	
	バッチファイルの構造
	(例) 	このプログラムのパス：	C:\MSXC\LIB\DD.COM
			ターゲットファイル：	D:\SRC\HOE.C
	
	dd %1					このプログラム
	%DD_EXEC1%				D:				カレントドライブを変更
	%DD_EXEC2%				D:\SRC\			カレントディレクトリを変更
	%DD_EXEC3%				aped HOE.C		実行内容
	%DD_EXEC4%				PAUSE			$KがあればPAUSE なければNULL
	%DD_EXEC5%				C:				カレントドライブを戻す
	%DD_EXEC6%				C:\MSXC\LIB\	カレントディレクトリを戻す
	%DD_EXEC7%	%DD_EXEC2%	DDG.BAT D:\SRC\	さっき表示していたパスを渡す
							↓
	dd %1					dd D:\SRC\		さっき表示していたパスを表示
	%DD_EXEC1%～%DD_EXEC7%	終了時はすべてNULL→普通にバッチファイルが終わる

*/

/* -------------------------------------- バッチファイルを作成する場合 */

		/* 外部プログラム実行用バッチファイル実行用環境変数を設定 */
		setenv( "DD_EXEC", BATFILE );

		/* バッチファイルの作成 */
		fp = fopen( batfile, "w" );
		if ( NULL == fp ) {
			
			/* ファイルが見つからない */
			msgbox( 0, 0, "CAN'T CREATE BATCH FILE", batfile );
			
		} else {
			/* 実行対象ドライブ */
			strcpy(  scmddrv, "*:" );
			strncpy( scmddrv, srctab->path, 2);
			
			/* $Kがあれば実行後キー待ちする */
			if( pmf & PM_FOUND_K ) {
				strcat( spause, "pause" );
			}
			
			/* DDの位置するドライブ */
			strcpy( smydrv, "*:" );
			strncpy( smydrv, mine, 2);
			
/* デバッグ用 この方法だとメモリに困らない
strcat( sfullcmd, "\nmemory\npause\n" );
*/
			if( fprintf( fp, 
				/*┌カレントドライブを変更
				  ｜  ┌カレントディレクトリを変更
				  ｜  ｜     ┌実行内容
				  ｜  ｜     ｜  ┌pause or ""
				  ｜  ｜     ｜  ｜  ┌カレントドライブを戻す
				  ｜  ｜     ｜  ｜  ｜  ┌カレントディレクトリを戻す
				  ｜  ｜     ｜  ｜  ｜  ｜     ┌バッチファイル名
				  ｜  ｜     ｜  ｜  ｜  ｜     ｜ ┌再起動時にここを表示する
				  ↓  ↓     ↓  ↓  ↓  ↓     ↓ ↓	*/
			 "cls\n%s\ncd %s\n%s\n%s\n%s\ncd %s\n%s %s\n"
				, scmddrv, srctab->path
				, sfullcmd
				, spause
				, smydrv, smypath
				, "D.bat", srctab->path
			) != EOF ) {
				if( fclose( fp ) != EOF ) {
					fp = NULL;
					
					/* 準備完了 */
					stret = OK;

					/* 今作成したバッチファイルの実行 */
/*					execlp( shell, batfile );
*/
/*
	execlp() バッチ		実行後
#1	command2 command2	帰ってくる	4174	3243	2312	×1381
#2	command2 直com		帰ってくる	4174	3243	2312	1381	×450帰ってこない
#3 
*/
				}
			}
		}
	} else {
		
		/* 拡張子に対応するコマンドが見つからなかった */
		msgbox( 0, 0, "COMMAND NOT FOUND", skey );
	}

	return stret;
}


