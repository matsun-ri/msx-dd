/**************************************************************
	DD_EXEC.c : �O���v���O�������s
**************************************************************/
/*
���O���v���O�������s�̗���

�ED.BAT�i���j		�F���g�͌Œ�
	DDX.com %1 %2
	%DD_EXEC%	�O���v���O�������s���� DDG.bat�E�{�v���O�����̏I�����͋󕶎���
						
�EBATFILE(DDG.BAT)	�F�K�XDDX.com���o��
	d:					�J�����g�h���C�u��ύX
	cd d:\src\			�J�����g�f�B���N�g����ύX
	notepad hoe.c		���s���e
	PAUSE				$K�������PAUSE �Ȃ����NULL
	C:					�J�����g�h���C�u��߂�
	C:\MSXC\LIB\		�J�����g�f�B���N�g����߂�
	DD.BAT D:\SRC\ A:\	DD.BAT�ɁA�������\�����Ă����p�X��2�^�u���n��
*/

#include <stdio.h>
/* XREG, FIB */
#include <bdosfunc.h>
/* locate() */
#include <msxbios.h>
/* execlp() */
#include <process.h>
#include "dd.h"

/* parsemacro�p�萔 */
#define PM_FOUND_C	(TINY)0x01	/* �t�@�C�����u�������}�N���𔭌� */
#define PM_FOUND_K	(TINY)0x02	/* ���s��L�[���͑҂��}�N���𔭌� */

/* dd_sub.c�ɂĒ�` */
extern int bdoscall( c, a, b, de, hl, ix, r );

/* dd_scrn.c�ɂĒ�` */
extern VOID msgbox( x, y, stitle, stext );


/**************************************************************
	���ϐ����玩�g�̃p�X���擾
	return�l�F	TRUE : �p�X������͂ł���
				FALSE: �ł��Ȃ�����
	execbat()��getmypath()
**************************************************************/
BOOL getmypath( shell, mine, mypath )
char *shell;	/* w/o COMMAND2.COM�̃p�X�{�t�@�C���� */
char *mine;		/* w/o �������g�̃p�X�{�t�@�C���� */
char *mypath;	/* w/o ���̃p�X�����̂� */
{
	char	*s;					/* ���ϐ��擾�p*/
	BOOL	bret;
	XREG	reg;


	bret = FALSE;
	
	/* ���ϐ� %SHELL% �̎擾 �����malloc����̂�free���邱�� */
	s = getenv( ENVSHELL );
	strcpy( shell, s );
	free( s );
	
	/* ���ϐ� %PROGRAM% �̎擾 �����malloc����̂�free���邱�� */
	s = getenv( ENVPRGM );
	strcpy( mine, s );	/* mine=c:\hoehoe\dd.com */
	free( s );
	
	strcpy( mypath, mine );
	
	/* �p�X���̉��  */
	bdoscall( _PARSE,
			0, 0, 
			(unsigned)mypath,
			0, 0,
			&reg			/* �߂�l���p */
			);

	if ( reg.hl == reg.de ) {
		/* HL==DE�̏ꍇ */
		/* �����񂪁u\�v�����ŏI����Ă�����A���邢�̓k��(�h���C�u���͕ʂƂ���) */
		/* �Ƃ肠�����t�@�C������ݒ� */
		strcpy( mypath, mypath );
	} else {
		/* HL=������̍Ō�̍���(�u�t�@�C�����v����)�̍ŏ��̕��� */
		/* �Ȃ̂ł�����NULL�ɂ���� \ �Ŏ~�܂� */
		/* mine=c:\hoehoe\dd.com mypath=c:\hoehoe\ */
		*(char *)(reg.hl) = NULL;

		bret = TRUE;
	}
	
	return bret;
}


/**************************************************************
	�g���q�ɑ΂���R�}���h���C�����R���t�B�O�t�@�C�����瓾��
	return�l�F	TRUE : �擾����
				FALSE: ������Ȃ�����
	execbat()��getmypath()
**************************************************************/
BOOL getexecfile( key, cfgfile, cmdline )
char	*key;		/* r/o �����Ώۂ̊g���q RET=������ SHIFT+RET=�啶�� */
char	*cfgfile;	/* r/o .cfg�t�@�C���� */
char	*cmdline;	/* w/o  �擾�����R�}���h���C�� */
{
	int		i, ilen, ict;
	char 	sline[80]	/* �R���t�B�O�t�@�C����1�s�����̂܂ܓ��� */
	    ,	slinekey[4]	/* �R���t�B�O�t�@�C����̊g���q���� */
			;
	char	*pline1, *pline2, *pline3;	/* �R���t�B�O�t�@�C����͗p */
	BOOL	bfind = FALSE;	/* �R���t�B�O�t�@�C���ɊY������g���q�������� */
	FILE	*fp;


	strcpy( slinekey, "" );

	fp = fopen( cfgfile, "r" );
	if ( NULL == fp ) {
		
		/* �t�@�C����������Ȃ� */
		msgbox( 0, 0, "CONFIG FILE NOT FOUND", cfgfile );
		
	} else {
		
		/* �ړI�̊g���q��������܂Ō��� */
		while ( (fgets( sline, 40, fp ) != NULL) && (bfind == FALSE) ) {
			
			if( (';' == sline[0]) || (0x0a == sline[0]) ){

				/* �s���������Ȃ�Z�~�R�����܂��͉��s�R�[�h���ǂݔ�΂� */
				/* �i��ԑ����p�^�[���Ƒz�肳���̂œ��ʈ����j */

			} else {

				/* �ǂݍ��񂾍s����� */

				/*            11111
					012345678901234
				      cpp    vsmall
				      pline1 �擪�̋󔒂𖳎������ŏ��̕����i�g���q��z��j
				        pline2 ���̎��̍ŏ��̋󔒁i�f���~�^��z��j
				             pline3 ���̎��̍ŏ��̕����i�R�}���h���C����z��j
				*/
				
				ilen = strlen( sline );
				pline1 = sline;
				
				/* �擪�̋󔒂�ǂݔ�΂� */
				while( (pline1 < sline+ilen) && isspace( *pline1 ) ) {
					pline1++;
				}
/*
				if( ';' == *pline1 ) {
					/* �擪���󔒁{�Z�~�R�������ǂݔ�΂� */
				} else {
*/
				if( '.' == *pline1 ) {
					
					pline1++;
					pline2 = pline1;
					
					/* �g���q���i���̋󔒂܂Łj�ǂݔ�΂� */
					while( (pline2 < sline+ilen) && (isspace(*pline2)==FALSE) ) {
						 pline2++;
					}
					pline3 = pline2;
					
					/* ���̋󔒁i�g���q�ƃR�}���h���C���̃f���~�^�j��ǂݔ�΂� */
					while( (pline3 < sline+ilen) && isspace( *pline3 ) ) {
						pline3++;
					}
					
					/* �ǂݍ��񂾍s����g���q�𒊏o */
					slinekey[0] = '\0';
					slinekey[1] = '\0';
					slinekey[2] = '\0';
					slinekey[3] = '\0';
					ict = pline2 - pline1; /* �g���q���� */
					if ( 3 < ict ) ict = 3;
					for( i=0; i<ict; i++ ){ 
						/* slinekey[]���g���q 1�`3���� */
						/* �啶���E�������͂��̂܂� */
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
					/* �֐��ɗ^����ꂽ�g���q�ƁA�������܂ߊ��S��v���Ă���� */
					if ( strcmp( slinekey, key ) == 0 ){
						/* �R���t�B�O�t�@�C���Ɋ|����Ă���R�}���h���C�� */
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
	���ϐ��̓o�^
	return�l�F	OK:���� ERROR:���s
**************************************************************/
STATUS setenv( skey, svalue )
char	*skey;		/* r/o ���ϐ��� */
char	*svalue;	/* r/o �l */
{
	char swork[MAXPATH];
	
	sprintf( swork, "%s=%s", skey, svalue );
	
	return putenv( swork );
}



/**************************************************************
	�}�N���̓W�J
	return�l�F	OK    �}�N������������
				ERROR �}�N���ł͂Ȃ�����
	execbat()��parsemacro()
**************************************************************/
STATUS parsemacro( ch, srcpath, srcfile, dstpath, dstfile, sfullcmd, pfound, pmf )
char ch;			/* r/o �}�N������ �啶���œn����� */
char *srcpath;		/* r/o �J�����g�^�u�̃p�X */
char *srcfile;		/* r/o �J�����g�^�u�̃t�@�C���� */
char *dstpath;		/* r/o ��������̃^�u�̃p�X */
char *dstfile;		/* r/o ��������̃^�u�̃t�@�C���� */
char *sfullcmd;		/* w/o �R�}���h���C�������p */
char **pfound;		/* r+w �����ʒu */
TINY *pmf;			/* r+w �����ς݃t���O */
{
	STATUS sret = OK;
	
	
	switch( ch ) {
		
		case 'C':	/* $C:�t�@�C�����u�������}�N�� */
			*(*pfound) = '\0';		/* ��U���������� */
			/* �t�@�C������ǉ� */
			strcat( sfullcmd, srcfile );
			*(pfound) += strlen( srcfile );
			*pmf = *pmf | PM_FOUND_C;	/* $C�������� */
/*
			bmc = TRUE;		/* $C�������� */
*/
			break;
		
		case 'D':	/* $D:�ʂ̃^�u�̃J�[�\���ʒu�̃t�@�C���� */
			*(*pfound) = '\0';		/* ��U���������� */
			/* �t�@�C������ǉ� */
			strcat( sfullcmd, dstfile );
			*(pfound) += strlen( dstfile );
			break;
		
		case 'K':	/* $K:���s��L�[���͑҂��}�N�� */
			*pmf = *pmf | PM_FOUND_K;	/* $K�������� */
/*
			bmk = TRUE;		/* $K�������� */
*/
			break;

		case 'P':	/* $P:�J�����g�^�u�̃p�X */
			*(*pfound) = '\0';		/* ��U���������� */
			/* �p�X��ǉ� */
			strcat( sfullcmd, srcpath );
			*(pfound) += strlen( srcpath );
			break;

		case 'Q':	/* $Q:�ʂ̃^�u�̃p�X */
			*(*pfound) = '\0';		/* ��U���������� */
			/* �t�@�C������ǉ� */
			strcat( sfullcmd, dstpath );
			*(pfound) += strlen( dstpath );
			break;
		
		default:	/* dd�̃}�N�������ł͂Ȃ� */
		
			sret = ERROR;
	}
	
	return sret;
}


/**************************************************************
	�O���v���O�����̎��s
	return�l�F	OK   : �O���v���O�����̎��s��������
				ERROR: �������s�i�R���t�B�O�̓Ǎ��Ɏ��s�Ȃǁj
	operation()��execbat()
**************************************************************/
/*
STATUS execbat( image, srcpath, srcfile, dstpath, dstfile, keymod )
char *image;	/* r/o �J�����g�^�u�̕\���C���[�W ���g���q�������g�� */
char *srcpath;	/* r/o �J�����g�^�u�̃p�X */
char *srcfile;	/* r/o �J�����g�^�u�̃t�@�C���� */
char *dstpath;	/* r/o ��������̃^�u�̃p�X */
char *dstfile;	/* r/o ��������̃^�u�̃t�@�C���� */
TINY  keymod;	/* r/o �C���L�[������� */
*/
STATUS execbat( srctab, dsttab, keymod )
DD_TAB	*srctab;	/* r/o �J�����g�^�u�y�[�W */
DD_TAB	*dsttab;	/* r/o ��J�����g�^�u�y�[�W */
TINY	keymod;		/* r/o �C���L�[������� */
{
	/* �R���t�B�O�t�@�C�������n */
	int		iret;
	char	shell[MAXPATH];		/* COMMAND2.COM�̃p�X�{�t�@�C���� */
	char	mine[MAXPATH];		/* �������g�̃p�X�{�t�@�C���� */
	char	smypath[MAXPATH];		/* �������g�̃p�X�̂� */
	char	cfgfile[MAXPATH];	/* .cfg�t�@�C���� */
	char	batfile[MAXPATH];	/* .bat�t�@�C�������p */
	char	cmdline[MAXPATH];	/* �R�}���h���C�� */
	
	/* �o�b�`�t�@�C���쐬�n */
	TINY	pmf = 0;			/* �}�N�������t���O */
	int		i = 0;
	int		ilen = 0;
	char	skey[4]				/* ��������g���q */
		,	scmddrv[3]			/* ���s�Ώۃh���C�u */
		,	smydrv[3]			/* DD�̈ʒu����h���C�u */
		,	spause[8]			/* pause�R�}���h */
		,	sfullcmd[MAXPATH]	/* �R�}���h���C�������p */
		;
	char	*pfound;			/* �����ʒu */
	FILE	*fp;
	STATUS	stret = ERROR;
	

	strcpy( spause, "" );

	/* ���ϐ����玩�g�̃p�X���擾 */
	if( getmypath( shell, mine, smypath ) ) {
		/* �p�X������͂ł��� */
		strcpy( cfgfile, smypath );		/* cfgfile=c:\hoehoe\ */
		strcat( cfgfile, CFGFILE );		/*                  + dd.cfg */
		strcpy( batfile, smypath );		/* batfile=c:\hoehoe\ */
		strcat( batfile, BATFILE );		/*                  + ddg.bat */
	} else {
		/* �p�X������͂ł��Ȃ�����*/
		strcpy( batfile, BATFILE );
	}

	/* �g���q���擾����i���łɑ啶���j */
	skey[0] = (srctab->files[ srctab->cursor ].image[9]==0x20 ? NULL:srctab->files[ srctab->cursor ].image[9]);
	skey[1] = (srctab->files[ srctab->cursor ].image[10]==0x20 ? NULL:srctab->files[ srctab->cursor ].image[10]);
	skey[2] = (srctab->files[ srctab->cursor ].image[11]==0x20 ? NULL:srctab->files[ srctab->cursor ].image[11]);
	skey[3] = '\0';
/*
locate((TINY)1,(TINY)5);
printf("* <execbat():%d>", (int)keymod );
*/
	/* Shift�L�[��������Ă��Ȃ��ꍇ�͊g���q�������������� */
	if ( ( keymod & KM_SHIFT ) == 0 ) {
		/*  �啶���̂܂� */
/*
msgbox( 0, 0, "execbat()", "shift ON" );
*/
	} else {
/*
msgbox( 0, 0, "execbat()", "shift OFF" );
*/
		/*  �������ɕϊ� */
		strlwr( skey );
	}

	/* �g���q�ɑ΂���R�}���h���C�����R���t�B�O�t�@�C�����瓾�� */
	if( getexecfile( skey, cfgfile, cmdline ) ) {
	
		/* �g���q�ɑΉ�����R�}���h���������� */

		/* ���s��ׂ� */
		if( 0x0a == cmdline[ strlen(cmdline)-1 ] ) {
			cmdline[ strlen(cmdline)-1 ] = '\0';
		}
		
		/* 1�������]�L���}�N��($x)�̌��� */
/*
		bmc = FALSE;
		bmk = FALSE;
*/
		pfound = sfullcmd;
		i = 0;

		while ( i <= strlen( cmdline ) ) { /* �Ō��\0�܂œ]�L������ */

			if( isspace( cmdline[i] ) ) {
				/* �X�y�[�X�ƕ���킵�������͂��ׂăX�y�[�X�ɕϊ� */
				*pfound = 0x20;
				pfound++;
				
			} else if( '$'==cmdline[i] ) {
				
				/* $�𔭌������玟�̕������`�F�b�N */
				if ( parsemacro( toupper(cmdline[i+1]),
							srctab->path, 
							srctab->files[ srctab->cursor ].name, 
							dsttab->path, 
							dsttab->files[ dsttab->cursor ].name,
							sfullcmd,
							&pfound,
							&pmf
						) == OK ) {
					
					/* ���̕����͓ǂݔ�΂�  */
					i++;
					
				} else {
					
					/* �Y������}�N�����Ȃ����������̂܂ܓ]�L���� */
					*pfound = cmdline[i];
					pfound++;
					
				}
/*				
				if ( 'C'==toupper(cmdline[i+1]) ) {
					/* $C:�t�@�C�����u�������}�N�� */
					*pfound = '\0';		/* ��U���������� */
					/* �t�@�C������ǉ� */
					strcat( sfullcmd, srctab->files[ srctab->cursor ].name );
					pfound += strlen( srctab->files[ srctab->cursor ].name );
					bmc = TRUE;		/* $C�������� */
					i++; /* ���̕����͓ǂݔ�΂�  */
					
				} else if ( 'D'==toupper(cmdline[i+1]) ) {
					/* $D:�ʂ̃^�u�̃J�[�\���ʒu�̃t�@�C���� */
					*pfound = '\0';		/* ��U���������� */
					/* �t�@�C������ǉ� */
					strcat( sfullcmd, dsttab->files[ dsttab->cursor ].name );
					pfound += strlen( dsttab->files[ dsttab->cursor ].name );

					i++; /* ���̕����͓ǂݔ�΂�  */
					
				} else if( 'K'==toupper(cmdline[i+1]) ) {
					/* $K:���s��L�[���͑҂��}�N�� */
					bmk = TRUE;		/* $K�������� */
					i++; /* ���̕����͓ǂݔ�΂�  */

				} else if( 'P'==toupper(cmdline[i+1]) ) {
					/* $P:�J�����g�^�u�̃p�X */
					*pfound = '\0';		/* ��U���������� */
					/* �p�X��ǉ� */
					strcat( sfullcmd, srctab->path );
					pfound += strlen( srctab->path );

					i++; /* ���̕����͓ǂݔ�΂�  */

				} else if( 'Q'==toupper(cmdline[i+1]) ) {
					/* $Q:�ʂ̃^�u�̃p�X */
					*pfound = '\0';		/* ��U���������� */
					/* �t�@�C������ǉ� */
					strcat( sfullcmd, dsttab->path );
					pfound += strlen( dsttab->path );

					i++; /* ���̕����͓ǂݔ�΂�  */

				} else {
					/* dd�̃}�N�������ł͂Ȃ������̂܂�$��]�L */
					*pfound = cmdline[i];
					pfound++;
				}
*/
			} else {
				/* ���̂܂ܓ]�L */
				*pfound = cmdline[i];
				pfound++;
			}
			i++;
		}

		/* $C���Ȃ���΃^�[�Q�b�g�t�@�C���������ɒǉ� */
		if( 0 == (pmf & PM_FOUND_C) ) {
			strcat( sfullcmd, " " );
			strcat( sfullcmd, srctab->files[ srctab->cursor ].name );
		}
		
/* -------------------------------------- ���ϐ��{�o�b�`�t�@�C���̏ꍇ */
/*
		/* ���ϐ��Ɏ��s���e���o�� */

		/* �J�����g�h���C�u���� */
		strcpy( sdrv, "*:" );
		strncpy( sdrv, srctab->path, 2);
		
		setenv( "DD_EXEC1", sdrv );			/* �J�����g�h���C�u��ύX */
		setenv( "DD_EXEC2", srctab->path );		/* �J�����g�f�B���N�g����ύX */
		setenv( "DD_EXEC3", sfullcmd );		/* �R�}���h���C�� */
		if( bmk ) {							/* $K������Ύ��s��L�[�҂����� */
			setenv( "DD_EXEC4", "pause" );
		} else {
			setenv( "DD_EXEC4", "" );
		}

		/* DD�̈ʒu����h���C�u���� */
		strcpy( sdrv, "*:" );
		strncpy( sdrv, mine, 2);
		
		setenv( "DD_EXEC5", sdrv );			/* �J�����g�h���C�u��߂� */
		setenv( "DD_EXEC6", smypath );		/* �J�����g�f�B���N�g����߂� */
		
		/* �o�b�`�t�@�C�����i�v���O�����I����=""�j */
		setenv( "DD_EXEC7", BATFILE );		

		/* �o�b�`�t�@�C����DD�ɖ߂� */
		stret = OK;
*/
/*
	5�`7�� c: | cd c:\msxc\lib\ | ddg.bat c:\work �ł�����
	1�`2�����Ԃ񂢂���
	
	�o�b�`�t�@�C���̍\��
	(��) 	���̃v���O�����̃p�X�F	C:\MSXC\LIB\DD.COM
			�^�[�Q�b�g�t�@�C���F	D:\SRC\HOE.C
	
	dd %1					���̃v���O����
	%DD_EXEC1%				D:				�J�����g�h���C�u��ύX
	%DD_EXEC2%				D:\SRC\			�J�����g�f�B���N�g����ύX
	%DD_EXEC3%				aped HOE.C		���s���e
	%DD_EXEC4%				PAUSE			$K�������PAUSE �Ȃ����NULL
	%DD_EXEC5%				C:				�J�����g�h���C�u��߂�
	%DD_EXEC6%				C:\MSXC\LIB\	�J�����g�f�B���N�g����߂�
	%DD_EXEC7%	%DD_EXEC2%	DDG.BAT D:\SRC\	�������\�����Ă����p�X��n��
							��
	dd %1					dd D:\SRC\		�������\�����Ă����p�X��\��
	%DD_EXEC1%�`%DD_EXEC7%	�I�����͂��ׂ�NULL�����ʂɃo�b�`�t�@�C�����I���

*/

/* -------------------------------------- �o�b�`�t�@�C�����쐬����ꍇ */

		/* �O���v���O�������s�p�o�b�`�t�@�C�����s�p���ϐ���ݒ� */
		setenv( "DD_EXEC", BATFILE );

		/* �o�b�`�t�@�C���̍쐬 */
		fp = fopen( batfile, "w" );
		if ( NULL == fp ) {
			
			/* �t�@�C����������Ȃ� */
			msgbox( 0, 0, "CAN'T CREATE BATCH FILE", batfile );
			
		} else {
			/* ���s�Ώۃh���C�u */
			strcpy(  scmddrv, "*:" );
			strncpy( scmddrv, srctab->path, 2);
			
			/* $K������Ύ��s��L�[�҂����� */
			if( pmf & PM_FOUND_K ) {
				strcat( spause, "pause" );
			}
			
			/* DD�̈ʒu����h���C�u */
			strcpy( smydrv, "*:" );
			strncpy( smydrv, mine, 2);
			
/* �f�o�b�O�p ���̕��@���ƃ������ɍ���Ȃ�
strcat( sfullcmd, "\nmemory\npause\n" );
*/
			if( fprintf( fp, 
				/*���J�����g�h���C�u��ύX
				  �b  ���J�����g�f�B���N�g����ύX
				  �b  �b     �����s���e
				  �b  �b     �b  ��pause or ""
				  �b  �b     �b  �b  ���J�����g�h���C�u��߂�
				  �b  �b     �b  �b  �b  ���J�����g�f�B���N�g����߂�
				  �b  �b     �b  �b  �b  �b     ���o�b�`�t�@�C����
				  �b  �b     �b  �b  �b  �b     �b ���ċN�����ɂ�����\������
				  ��  ��     ��  ��  ��  ��     �� ��	*/
			 "cls\n%s\ncd %s\n%s\n%s\n%s\ncd %s\n%s %s\n"
				, scmddrv, srctab->path
				, sfullcmd
				, spause
				, smydrv, smypath
				, "D.bat", srctab->path
			) != EOF ) {
				if( fclose( fp ) != EOF ) {
					fp = NULL;
					
					/* �������� */
					stret = OK;

					/* ���쐬�����o�b�`�t�@�C���̎��s */
/*					execlp( shell, batfile );
*/
/*
	execlp() �o�b�`		���s��
#1	command2 command2	�A���Ă���	4174	3243	2312	�~1381
#2	command2 ��com		�A���Ă���	4174	3243	2312	1381	�~450�A���Ă��Ȃ�
#3 
*/
				}
			}
		}
	} else {
		
		/* �g���q�ɑΉ�����R�}���h��������Ȃ����� */
		msgbox( 0, 0, "COMMAND NOT FOUND", skey );
	}

	return stret;
}

