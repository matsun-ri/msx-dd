/**************************************************************
	DD_COPY.c : �t�@�C���R�s�[�E�ړ�����
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>		/* XREG, FIB, bios() */
#include <msxbios.h>		/* locate() */
#include "dd.h"

/* dd_sub.c�ɂĒ�` */
extern int bdoscall( c, a, b, de, hl, ix, r );
extern STATUS bdos_trap( c, a, b, de, hl, ix, r, fname );

/* dd_scrn.c�ɂĒ�` */
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );
extern VOID putbox( x, y, w, h );
extern VOID msgbox( x, y, stitle, stext );
extern VOID progressbox( stitle );
extern VOID putprogress( stext, idenomi, inum );

extern int marking( tab, itabpage, index );


/**************************************************************
	�t�@�C���̃R�s�[�i���ۂɍs�Ȃ��j
	return�l�F	OK:���� ERROR:���s
	tocopy()��onecopy()
**************************************************************/
STATUS onecopy( ssrc, sdest )
char	*ssrc;	/* r/o �R�s�[���t�@�C���p�X�{���O */
char	*sdest;	/* r/o �R�s�[��t�@�C���p�X�{���O */
{
	char	buffer[1024];		/* �R�s�[�p�o�b�t�@ */
	int		ilen = 0;			/* �R�s�[�p�T�C�Y */
	int		iret = 0;			/* return�l���p */
	unsigned	uattr = 0;		/* �t�@�C������ */
	unsigned	udate = 0;		/* �t�@�C�����t */
	unsigned	utime = 0;		/* �t�@�C������ */
	STATUS	sret = OK;			/* return�l */
	FD		fdsrc, fddest;		/* �ᐅ�����o�͂̃t�@�C���n���h�� */
	XREG	reg;				/* BDOS�R�[������̖߂�l�擾�p */


	fdsrc = NULL;
	fddest = NULL;
	
	/* �R�s�[���t�@�C���̃I�[�v�� �Ǎ���p */
	fdsrc = open( ssrc, O_RDONLY );
	if ( ERROR == fdsrc ) {
		fdsrc = NULL;
		/* �G���[ */
		msgbox( 0, 0, "FILE OPEN ERROR", ssrc );
		return ERROR;
	}
	
	/* �R�s�[��t�@�C���̍쐬 ������p */
	fddest = creat( sdest );	/* ���ɑ��݂���t�@�C���͏㏑������� */
	if ( ERROR == fddest ) {
		fddest = NULL;
		/* �G���[ */
		close( fdsrc );			/* �R�s�[���t�@�C������Ă��� */
		fdsrc = NULL;
		msgbox( 0, 0, "FILE CREAT ERROR", sdest );
		return ERROR;
	}
	
	/* �R�s�[���� */
	do {
		/* �ǂݍ��� */
		ilen = read( fdsrc, buffer, 1024 );
		
		/* �ǂ߂�΁i�܂�EOF�łȂ���΁j */
		if ( 0 != ilen ) {
			
			/* �������� */
			if( 0 == write( fddest, buffer, ilen ) ) {
				/* �������݃G���[ */
				msgbox( 0, 0, "FILE WRITE ERROR", sdest );
				sret = ERROR;
			}
		}
		
		/*  */
	} while ( (0 != ilen) && ( OK == sret ) );
	
	/* �R�s�[���t�@�C���̃N���[�Y */
	if( ERROR == close( fdsrc ) ) {
		msgbox( 0, 0, "FILE CLOSE ERROR", ssrc );
		sret = ERROR;
	}
	fdsrc = NULL;

	/* �R�s�[��t�@�C���̃N���[�Y */
	if( ERROR == close( fddest ) ) {
		msgbox( 0, 0, "FILE CLOSE ERROR", sdest );
		sret = ERROR;
	}
	fddest = NULL;
	
	/* �����܂ł̏����ŃG���[���Ȃ���� */
	/* �^�C���X�^���v�Ƒ������R�s�[���� */
	if( OK == sret ) {

		/* �R�s�[���t�@�C������^�C���X�^���v���E�� */
		if ( ( sret = bdos_trap(
						 _FTIME, /* �t�@�C���̓��t����ю����̊l���E�Z�b�g */
						0,		/* ���t�Ǝ����̊l�� */
						0,
						(unsigned)ssrc,	/* �h���C�u�E�p�X�E�t�@�C�� */
						0,
						0,
						&reg,	/* �߂�l���p */
						"onecopy().10"
			) )  == OK ) {

			/* �G���[�łȂ���� */

			/* �^�C���X�^���v���E�� */
			utime = reg.de;
			udate = reg.hl;

			/* �R�s�[��t�@�C���֓��t�E�������Z�b�g */
			if ( ( sret = bdos_trap( _FTIME, /* �t�@�C���̓��t����ю����̊l���E�Z�b�g */
							1,				/* ���t�Ǝ����̃Z�b�g */
							0,
							(unsigned)sdest, /* �h���C�u�E�p�X�E�t�@�C�� */
							udate,			/* ���t */
							utime,			/* ���� */
							&reg,			/* �߂�l���p */
							"onecopy().20"
				) )  == OK ) {

				/* �����܂ł̏����ŃG���[���Ȃ���� */
				/* �t�@�C���������R�s�[���� */

				/* �R�s�[���t�@�C�����瑮�����l�� */
				if ( ( sret = bdos_trap(
						_ATTR,	/* �t�@�C�������̊l���E�Z�b�g */
						0,				/* �����̊l�� */
						0,
						(unsigned)ssrc,	/* �h���C�u�E�p�X�E�t�@�C�� */
						0,
						0,
						&reg,			/* �߂�l���p */
						"onecopy().30"
					) )  == OK ) {
					
					/* �G���[�łȂ���� */
					
					/* �t�@�C���������E�� */
					uattr = reg.hl & 0xff;	/* L=���݂̑��� */
					
					/* �R�s�[��t�@�C���֑������Z�b�g */
					sret = bdos_trap(
							_ATTR,
							1,				/* �����̃Z�b�g */
							0,
							(unsigned)sdest, /* �h���C�u�E�p�X�E�t�@�C�� */
							uattr,			/* L���V��������(1byte) */
							0,
							&reg,			/* �߂�l���p */
							"onecopy().40"
						);
				}
			}
		}
	}

	return sret;
}


/**************************************************************
	�t�@�C���̃R�s�[�i�O�����j
	return�l�F	OK:���� ERROR:���s
	operation()��tocopy()
	�O������F
		�E1�ȏ�̃t�@�C�����}�[�N����Ă��邱��
		�E�f�B���N�g���ɑ΂��Ă͉������Ȃ��B�G���[���o���Ȃ�
**************************************************************/
STATUS tocopy( tabs, itabpage )
DD_TAB	tabs[];			/* r/o �^�u�y�[�W1����\������\���� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	int		i = 0;
	int		ict  = 0;	/* �����ς݌��� */
	int		imax = 0;	/* �����Ώی��� */
	char	ssrc[MAXPATH];		/* �ړ��� */
	char	sdest[MAXPATH];		/* �ړ��� */
	STATUS	sret = OK;			/* return�l */


	/* �R�s�[���p�X�ƃR�s�[��p�X�������łȂ����m�F */
	if ( 0 == strcmp( tabs[itabpage].path, tabs[1 - itabpage].path ) ) {
		msgbox( 0, 0, "ERROR", "file can't copy onto itself." );
		return ERROR;
	}
	

	/* �v���O���X�\���̏��� */
	progressbox( "COPY" );
	imax = tabs[itabpage].markedct;

	/* �R�s�[��   �̎���\�����Ƀt�@�C���ꗗ�̎擾���K�v */
	tabs[ 1 - itabpage ].status = TS_CHDIR;

	for ( i=0; i<tabs[itabpage].count; i++ ){

		/* �}�[�N����Ă���t�@�C����T�� */
		if ( TRUE == tabs[itabpage].files[i].ismarked ) {
			
			/* �R�s�[���p�X */
			strcpy( ssrc, tabs[itabpage].path );
			strcat( ssrc, tabs[itabpage].files[i].name );
			
			/* �R�s�[��p�X */
			strcpy( sdest, tabs[1 - itabpage].path );
			strcat( sdest, tabs[itabpage].files[i].name );
			
			/* �v���O���X�\�� */
			putprogress( tabs[itabpage].files[i].name, imax, ict );

			/* �f�B���N�g���łȂ���΃R�s�[ */
			if( tabs[itabpage].files[i].attr & 0x10 ) {

				/* �f�B���N�g���̓X�L�b�v */

			} else {

				/* �t�@�C���̃R�s�[ */
				sret = onecopy( ssrc, sdest );

				if ( OK == sret ) {

					/* ���������t�@�C���̃}�[�N���������� */
					marking( &tabs[itabpage], itabpage, i );

				} else {
					/* �G���[�̏ꍇ�̓��b�Z�[�W��\�� */
					msgbox( 0, 0, "tocopy", "error cuured." );
					break;
				}
			}
			
			ict++;
			
			/* �}�[�N����Ă���t�@�C�������ׂď������I����� */
			if ( imax == ict ) break;
			
		}
	}
	
	return sret;
}


/**************************************************************
	�t�@�C���܂��̓T�u�f�B���N�g���̈ړ�
	�E�����h���C�u�Ȃ�t�@�C�����f�B���N�g�����ړ�
	�E�Ⴄ�h���C�u�Ȃ�t�@�C���̂݃R�s�[�{�폜�B�f�B���N�g���͏������Ȃ�
	return�l�F	OK:���� ERROR:���s
	operation()��tomove()
**************************************************************/
STATUS tomove( tabs, itabpage )
DD_TAB	tabs[];			/* r/o �^�u�y�[�W1����\������\���� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	int		i = 0;
	int		ict  = 0;			/* �����ς݌��� */
	int		imax = 0;			/* �����Ώی��� */
	char	ssrc[MAXPATH];		/* �ړ��� */
	char	sdest[MAXPATH];		/* �ړ��� */
	STATUS	sret = OK	;		/* return�l */
	XREG	reg;				/* BDOS�R�[������̖߂�l�擾�p */
	char ch1, ch2;


	/* �h���C�u���^�[�����o�� */
	ch1 = tabs[itabpage].path[0];
	ch2 = tabs[1 - itabpage].path[0];

	/* �v���O���X�\���̏��� */
	if( ch1 == ch2 ) {
		progressbox( "MOVE on same" );
	} else {
		progressbox( "MOVE to other" );
	}
	imax = tabs[itabpage].markedct;
	
	/* �ړ����̎���\�����Ƀt�@�C���ꗗ�̎擾���K�v */
	tabs[ itabpage ].status = TS_CHDIR;
	/* �ړ���̎���\�����Ƀt�@�C���ꗗ�̎擾���K�v */
	tabs[ 1 - itabpage ].status = TS_CHDIR;

	/* �ړ���p�X�i�h���C�u�����݁j */
	strcpy( sdest, tabs[1 - itabpage].path );

	for ( i=0; i<tabs[itabpage].count; i++ ){

		/* �}�[�N����Ă���t�@�C����T�� */
		if ( TRUE == tabs[itabpage].files[i].ismarked ) {
			
			/* �ړ����p�X */
			strcpy( ssrc, tabs[itabpage].path );
			strcat( ssrc, tabs[itabpage].files[i].name );
			
			
			/* �v���O���X�\�� */
			putprogress( tabs[itabpage].files[i].name, imax, ict );

			/* �ړ����ƈړ���̃h���C�u���^�[�͓������H */
			if( ch1 == ch2 ) {
				
				/* �t�@�C�����邢�̓T�u�f�B���N�g���̈ړ� */
				if ( ( sret = bdos_trap(
						_MOVE,
						0,
						0,
						(unsigned)ssrc,			/* �h���C�u�E�p�X�E�t�@�C�� */
						(unsigned)sdest + 2,	/* �V�����p�X �h���C�u�ƁF���΂� */
						0,
						&reg,				/* �߂�l���p */
						"tomove().10"
					) )  == OK ) {

					/* �G���[�łȂ���� */
					
				} else {
					
					break;
					
				}
				
			} else {

				/* �Ⴄ�h���C�u�Ȃ�R�s�[�{�폜 */
				
				/* �t�@�C���H �f�B���N�g���H */
				if(	tabs[itabpage].files[i].attr & 0x10 ) {

					/* �f�B���N�g���ɂ͉������Ȃ� */

				} else {

					/* �t�@�C���Ȃ珈���Ώ� */

					/* �R�s�[��p�X */
					strcpy( sdest, tabs[1 - itabpage].path );
					strcat( sdest, tabs[itabpage].files[i].name );

					/* �t�@�C���̃R�s�[ */
					sret = onecopy( ssrc, sdest );
					if ( OK == sret ) {
					
						/* �t�@�C���̍폜 */
						sret = unlink( ssrc );
						if ( OK == sret ) {

						} else {
							/* �G���[�̏ꍇ�̓��b�Z�[�W��\�� */
							msgbox( 0, 0, "MOVE DELETE PHASE", "error cuured." );
							break;
						}
					
					} else {
						/* �G���[�̏ꍇ�̓��b�Z�[�W��\�� */
						msgbox( 0, 0, "MOVE COPY PHASE", "error cuured." );
						break;
					}
				}
			}

			/* ���������t�@�C���̃}�[�N���������� */
			marking( &tabs[itabpage], itabpage, i );
			
			ict++;
			
			/* �}�[�N����Ă���t�@�C�������ׂď������I����� */
			if ( imax == ict ) break;
		}
	}
	
	return sret;
}


/**************************************************************
	�t�@�C���܂��̓T�u�f�B���N�g���̍폜
	return�l�F	OK:���� ERROR:���s
	operation()��todelete()
**************************************************************/
STATUS todelete( tabs, itabpage )
DD_TAB	tabs[];			/* r/o �^�u�y�[�W1����\������\���� */
int		itabpage;		/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	int		i = 0;
	int		ict  = 0;	/* �����ς݌��� */
	int		imax = 0;	/* �����Ώی��� */
	char	pathwk[MAXPATH];	/* path�쐬�p */
	STATUS	sret = OK;			/* return�l */
	XREG	reg;				/* BDOS�R�[������̖߂�l�擾�p */


	/* �v���O���X�\���̏��� */
	progressbox( "DELETE" );
	imax = tabs[itabpage].markedct;
	
	for ( i=0; i<tabs[itabpage].count; i++ ){

		/* �}�[�N����Ă���t�@�C����T�� */
		if ( TRUE == tabs[itabpage].files[i].ismarked ) {

			/* �v���O���X�\�� */
			putprogress( tabs[itabpage].files[i].name, imax, ict );

			strcpy( pathwk, tabs[itabpage].path );
			strcat( pathwk, tabs[itabpage].files[i].name );
			
			/* �t�@�C���E�T�u�f�B���N�g���̍폜 */
			if ( ( sret = bdos_trap(
					_DELETE,
					0,
					0,
					(unsigned)pathwk,	/* �h���C�u�E�p�X�E�t�@�C�� */
					0,
					0,
					&reg,				/* �߂�l���p */
					"todelete().10"
				) )  == OK ) {
				
				/* �G���[�łȂ���� */
				
			} else {
				
				break;
				
			}
			
			/* ���������t�@�C���̃}�[�N���������� */
			marking( &tabs[itabpage], itabpage, i );
			
			ict++;
			
			/* �}�[�N����Ă���t�@�C�������ׂď������I����� */
			if ( imax == ict ) break;
		}
	}
	
	/* ����\�����Ƀt�@�C���ꗗ�̎擾���K�v */
	tabs[ itabpage ].status = TS_CHDIR;
	
	return sret;
}

