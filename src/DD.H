/* 再帰処理を使用しない */
#pragma nonrec


#define ENVSHELL	"SHELL"		/*  環境変数%SHELL%取得用 */
#define ENVPRGM		"PROGRAM"	/*  環境変数%PROGRAM%取得用 */
#define CFGFILE		"DD.CFG"	/*  環境設定ファイル */
#define BATFILE		"DDG.BAT"	/*  子プロセス起動用バッチファイル */
#define MAXPATH		256			/* パス最大長 */
#define MAXFILES	128			/* 1ディレクトリ内のファイル最大数 */
#define	LIST_X		3			/* スクロール領域の左上 */
#define	LIST_Y		4			/* スクロール領域の左上 */
#define	LIST_H		18			/* スクロール領域の縦幅（行数） */
#define	TAB_MAX		2			/* 最大タブ数 */
#define KEY_BS		(TINY)8		/* BSキー */
#define KEY_TAB		(TINY)9		/* TABキー */
#define KEY_RET		(TINY)13	/* RETURNキー */
#define KEY_SEL		(TINY)24	/* SELECTキー */
#define KEY_ESC		(TINY)27	/* ESCキー */
#define KEY_RIGHT	(TINY)28	/* →キー */
#define KEY_LEFT	(TINY)29	/* ←キー */
#define KEY_UP		(TINY)30	/* ↑キー */
#define KEY_DOWN	(TINY)31	/* ↓キー */
#define KEY_ROOT	(TINY)'\\'	/* \ キー */
#define KEY_DEL		(TINY)127	/* DELキー */

/* Key Matrix: キーマトリクスの状態 */
/* bit7       6       5      4        3       2       1       0   */
/* F8/F3	F7/F2	F6/F1	かな	CAPS	GRAPH	CTRL	SHIFT */
#define KM_SHIFT	(TINY)1


/* Tab Status: タブページ構造体の状態 */
#define TS_NOP		(TINY)0		/* 現在の情報が最新（再読み込み不要） */
#define TS_CHDIR	(TINY)1		/* ディレクトリを移動した（ファイル一覧の取得を要する） */
#define TS_CHDRV	(TINY)2		/* ドライブを変更した（ドライブ情報＋↑の取得を要する） */
#define TS_UNUSED	(TINY)10	/* ◆未使用：まだ一度もファイル一覧を取得したことがない
						   （もう一方のタブページからの丸写しを要する） */

/* Loop Status: メインループの状態 */
#define LS_LOOP		(TINY)0		/* ループを継続 */
#define LS_CHGTAB	(TINY)1		/* タブページを変更した */
#define LS_BORDER	(TINY)10	/* ---これ未満ならループを継続--- */
#define LS_EXEC		(TINY)11	/* 外部コマンドの実行 */
#define LS_END		(TINY)12	/* 単なる終了 */


typedef	unsigned MYLONG[2];

typedef	struct {
	char	name[13];	/* file name as an ASCIZ string		*/
	TINY	attr;		/* file attribute			*/
	unsigned time;		/* time of last modification		*/
	unsigned date;		/* date of last modification		*/
	MYLONG	size;		/* file size in bytes */
						/* ↑SLONGを使うためにmath.hをリンクするとコンパイラがオーバーフローする*/
	char	image[36];	/* ABCDEFGH.TXTkkk,bbb yy-mm-dd hh:mm のイメージ*/
						/* 12345678901234567890123456789012345 */
	BOOL	ismarked;	/* マークフラグ */
	BOOL	isalive;	/* 死活フラグ */
} FILES;

/* タブページ1枚を表現する構造体 */
typedef	struct {
	TINY	status;				/* 再読み込み要求 Tab Statusを参照 */
	int		count;				/* ファイル数 */
	int		cursor;				/* カーソル位置（何個目のファイルを指しているか） */
	int		page;				/* 何ページ目か 最初=0 */
	int		markedct;			/* マークされたファイル数 */
	MYLONG	markedsize;			/* ↑のファイルサイズ */
	char	path[MAXPATH];		/* パス（ドライブ＋パス＋\） */
	char	shortpath[MAXPATH];	/* ↑の短縮形 */
	char	label[12];			/* ボリュームラベル */
	char	capa[16];			/* ファイル容量の表示イメージ */
	char	bar[9];				/* free bargraph image */
	FILES	files[MAXFILES];	/* ファイル一覧 */
} DD_TAB;


