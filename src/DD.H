/* �ċA�������g�p���Ȃ� */
#pragma nonrec


#define ENVSHELL	"SHELL"		/*  ���ϐ�%SHELL%�擾�p */
#define ENVPRGM		"PROGRAM"	/*  ���ϐ�%PROGRAM%�擾�p */
#define CFGFILE		"DD.CFG"	/*  ���ݒ�t�@�C�� */
#define BATFILE		"DDG.BAT"	/*  �q�v���Z�X�N���p�o�b�`�t�@�C�� */
#define MAXPATH		256			/* �p�X�ő咷 */
#define MAXFILES	128			/* 1�f�B���N�g�����̃t�@�C���ő吔 */
#define	LIST_X		3			/* �X�N���[���̈�̍��� */
#define	LIST_Y		4			/* �X�N���[���̈�̍��� */
#define	LIST_H		18			/* �X�N���[���̈�̏c���i�s���j */
#define	TAB_MAX		2			/* �ő�^�u�� */
#define KEY_BS		(TINY)8		/* BS�L�[ */
#define KEY_TAB		(TINY)9		/* TAB�L�[ */
#define KEY_RET		(TINY)13	/* RETURN�L�[ */
#define KEY_SEL		(TINY)24	/* SELECT�L�[ */
#define KEY_ESC		(TINY)27	/* ESC�L�[ */
#define KEY_RIGHT	(TINY)28	/* ���L�[ */
#define KEY_LEFT	(TINY)29	/* ���L�[ */
#define KEY_UP		(TINY)30	/* ���L�[ */
#define KEY_DOWN	(TINY)31	/* ���L�[ */
#define KEY_ROOT	(TINY)'\\'	/* \ �L�[ */
#define KEY_DEL		(TINY)127	/* DEL�L�[ */

/* Key Matrix: �L�[�}�g���N�X�̏�� */
/* bit7       6       5      4        3       2       1       0   */
/* F8/F3	F7/F2	F6/F1	����	CAPS	GRAPH	CTRL	SHIFT */
#define KM_SHIFT	(TINY)1


/* Tab Status: �^�u�y�[�W�\���̂̏�� */
#define TS_NOP		(TINY)0		/* ���݂̏�񂪍ŐV�i�ēǂݍ��ݕs�v�j */
#define TS_CHDIR	(TINY)1		/* �f�B���N�g�����ړ������i�t�@�C���ꗗ�̎擾��v����j */
#define TS_CHDRV	(TINY)2		/* �h���C�u��ύX�����i�h���C�u���{���̎擾��v����j */
#define TS_UNUSED	(TINY)10	/* �����g�p�F�܂���x���t�@�C���ꗗ���擾�������Ƃ��Ȃ�
						   �i��������̃^�u�y�[�W����̊ێʂ���v����j */

/* Loop Status: ���C�����[�v�̏�� */
#define LS_LOOP		(TINY)0		/* ���[�v���p�� */
#define LS_CHGTAB	(TINY)1		/* �^�u�y�[�W��ύX���� */
#define LS_BORDER	(TINY)10	/* ---���ꖢ���Ȃ烋�[�v���p��--- */
#define LS_EXEC		(TINY)11	/* �O���R�}���h�̎��s */
#define LS_END		(TINY)12	/* �P�Ȃ�I�� */


typedef	unsigned MYLONG[2];

typedef	struct {
	char	name[13];	/* file name as an ASCIZ string		*/
	TINY	attr;		/* file attribute			*/
	unsigned time;		/* time of last modification		*/
	unsigned date;		/* date of last modification		*/
	MYLONG	size;		/* file size in bytes */
						/* ��SLONG���g�����߂�math.h�������N����ƃR���p�C�����I�[�o�[�t���[����*/
	char	image[36];	/* ABCDEFGH.TXTkkk,bbb yy-mm-dd hh:mm �̃C���[�W*/
						/* 12345678901234567890123456789012345 */
	BOOL	ismarked;	/* �}�[�N�t���O */
	BOOL	isalive;	/* �����t���O */
} FILES;

/* �^�u�y�[�W1����\������\���� */
typedef	struct {
	TINY	status;				/* �ēǂݍ��ݗv�� Tab Status���Q�� */
	int		count;				/* �t�@�C���� */
	int		cursor;				/* �J�[�\���ʒu�i���ڂ̃t�@�C�����w���Ă��邩�j */
	int		page;				/* ���y�[�W�ڂ� �ŏ�=0 */
	int		markedct;			/* �}�[�N���ꂽ�t�@�C���� */
	MYLONG	markedsize;			/* ���̃t�@�C���T�C�Y */
	char	path[MAXPATH];		/* �p�X�i�h���C�u�{�p�X�{\�j */
	char	shortpath[MAXPATH];	/* ���̒Z�k�` */
	char	label[12];			/* �{�����[�����x�� */
	char	capa[16];			/* �t�@�C���e�ʂ̕\���C���[�W */
	char	bar[9];				/* free bargraph image */
	FILES	files[MAXFILES];	/* �t�@�C���ꗗ */
} DD_TAB;


