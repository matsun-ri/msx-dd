/**************************************************************
	�t�@�C���ꗗ�̎擾�E�\��
**************************************************************/
#include <stdio.h>
#include <bdosfunc.h>
/* #include <glib.h>		ldirvm */
/* #include <msxbios.h>	 locate */
#include <math.h>	 /* slcpy */
#include "dd.h"


/* dd_scrn.c�ɂĒ�` */
extern VOID vputs_b( x, y, str );

extern VOID putbox( x, y, w, h );
extern int bdoscall( c, a, b, de, hl, ix, r );
extern char *sputsize( str, psl );
extern VOID errmsgbox( icode, fname );



/**************************************************************
	�w�肳�ꂽ�p�X�̃t�@�C���ꗗ�𓾂�
	return�l�F�t�@�C����
**************************************************************/
int getfiles( path, files )
char	*path;		/* r/o �p�X */
FILES	*files;		/* w/o FILES�^�\���̂ւ̃|�C���^ */
{
	int			i, ct, ipos, iret, ilen;
	unsigned	uiform, uiyy, uimm, uidd, uihh, uinn	/*�t�@�C�����t�����p*/
				,uiwk;
	char		msg[72];
	char		sname[13], ssize[8], sstamp[15], swk[13];
	BOOL		bbreak;
	XREG		reg;
	FIB			fib;
	
	
	/* �������̕\�� */
	putbox( 14, 9, 14, 5 );
	vputs_b( 16, 11, "LOADING..." );

	
	/* �ŏ��̃G���g���̌��� */
	iret = bdoscall( _FFIRST,
					0, 
					0x16,				/* �f�B���N�g��+�V�X�e��+�s�� */
					(unsigned)path,	/* �h���C�u�E�p�X�E�t�@�C�� */
					0,
					(unsigned)&fib,	/* �߂�l���p */
					&reg				/* �߂�l���p */
				);
	ct = 0;

	/* �ǉ����� */
	while( iret == 0 ){
		
		/* �O��̌������ʂ̕ۑ� */
		strcpy( files[ct].name, fib.name );
		files[ct].attr = fib.attr;
		files[ct].ismarked = FALSE;	/* �}�[�N�t���O */
		files[ct].isalive = TRUE;		/* �����t���O */

		/* �t�@�C������8.3�`���ɐ��` */
					/*   123456789012n */
		strcpy(  sname, "            " );
		ilen = strlen( fib.name );

		/* "."����납��T�� */
		/* ������Ȃ�= 0 */
		bbreak = FALSE;
		for( ipos = ilen; 0 < ipos; ipos-- ) {
			if ( fib.name[ipos - 1] == '.' ) break;
		}
		if ( ipos == 0 ) {
			/* .���Ȃ� */
			strncpy( sname, fib.name, ilen );
		} else {
			/* .������ */
			
			/* �t�@�C�������R�s�[ */
			strncpy( sname, fib.name, ipos-1 );
			sname[8] = '.';
			/* �g���q���R�s�[ */
			for( i = 0; i < ilen-ipos; i++ ) {
				sname[ i + 9 ] = fib.name[ ipos + i ];
			}
		}

		/* �t�@�C���T�C�Y�̎擾 */
		slcpy( (SLONG *)(files[ct].size), (SLONG *)(fib.filesize) );

		/* �f�B���N�g�����H */
		if( fib.attr & 0x10 ) {
			/*              1234567n */
			strcpy( ssize, "< DIR >");

			/* "."��".."�͓��ʈ��� */
			if(	(strcmp( ".", fib.name ) == 0) ||
				(strcmp( "..", fib.name ) == 0) ) {
				strcpy(  sname, "            " );
				strncpy( sname, fib.name, ilen );
			}

		} else {
			
			/* �t�@�C���T�C�Y�𕶎��Ƃ��Ď擾 */
			sputsize( ssize, (SLONG *)(fib.filesize) );
		}

		/* �t�@�C�����t�̕����� */
		if ( fib.fibdate == 0 ) {
			/* ���t���Z�b�g����Ă��Ȃ�*/
						  /* 12345678901234n */
			strcpy( sstamp, "-- no  data --" );

		} else{
			/* ���t���Z�b�g����Ă���*/
			uiform = fib.fibtime >> 5;	/* �b(0-4)�͖��� */
			uinn   = uiform & 0x3f;		/* ��(5-10) */
			uihh   = uiform >> 6;		/* ����(5-8) */

			uiform = fib.fibdate;
			uidd   = uiform & 0x1f;		/* ��(0-4) */
			uiform = uiform >> 5;
			uimm   = uiform & 0x0f;		/* ��(5-8) */
			uiyy   = (uiform >> 4) + 80;	/* �N(9-15=1980�`2079) */
			
			sprintf( sstamp,
					"%02d-%02d-%02d %2d:%02d",
					uiyy % 100, uimm, uidd, uihh, uinn );
		}
/*
         1111111111222222222233333333334
1234567890123456789012345678901234567890
ABCDEFGH.TXTkkk,bbb yy-mm-dd hh:mm
*/
		/* �t�@�C�����{�e�ʁ{���t */
		sprintf( files[ct].image, "%s%s %s", sname, ssize, sstamp );

		/* ���̃G���g���̌��� */
		iret = bdoscall( _FNEXT,
				0, 0, 0, 0,
				(unsigned)&fib,	/* �߂�l���p */
				&reg			/* �߂�l���p */
			);
		ct++;
	}
	files[ct].isalive = FALSE; /* �����t���O */
	
	/* File not found �ȊO�̃G���[�̏ꍇ�̓��b�Z�[�W��\�� */
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


