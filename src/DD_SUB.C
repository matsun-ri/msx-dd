#include <stdio.h>
#include <bdosfunc.h>
#include <math.h>
#include "dd.h"

/* �ċA�������g�p���Ȃ� */
#pragma nonrec


/* dd_drv.c�ɂĒ�` */
extern char *sputsize( str, psl );

/* dd_msg.c�ɂĒ�` */
extern VOID vputs_b( x, y, str );
extern VOID putbox( x, y, w, h );
extern VOID errmsgbox( icode, fname );



/**************************************************************
	�Ō�� \ ���Ȃ���� \ ��ǉ�
	return�l�F	*s
**************************************************************/
char *addbackslash( s )
char *s; /* r+w �����Ώ� */
{
	int ilen;
	
	ilen = strlen( s );
	
	if ( 0 < ilen ){
		/* �ŌオNULL�ł�\�ł��Ȃ���\��ǉ� */
		if ( (s[ ilen - 1 ] != NULL) && (s[ ilen - 1 ] != '\\' )) {
			strcat( s, "\\" );
		}
	}
	return s;
}


/**************************************************************
	�p�X������̒Z�k�`���쐬����
	return�l�Fshortpath
**************************************************************/
char *makeshortpath( path, shortpath )
char	*path;			/* r/o �p�X������ */
char	*shortpath;		/* w/o �Z�k�����p�X�������Ԃ� */
{
	int ilen;
	char swork[MAXPATH];
	
	ilen = strlen( path );
	
	if ( ilen < 36 ) {
		/* ���̂܂� */
		strcpy( shortpath, path );
	} else {
		/* �k�߂� */
		strcpy( swork, path );
		swork[3] = '\0';	/* d:\�̎� */
		strcat( swork, "..." );
		strcat( swork, (char *)(path + ilen - 31) );
		
		strcpy( shortpath, swork );
	}
	
	return shortpath;
}


/**************************************************************
	BDOS�R�[���i�G���[�����Ȃ��j
	return�l�F	BDOS����߂��ꂽA���W�X�^�̒l
**************************************************************/
int bdoscall( c, a, b, de, hl, ix, r )
TINY c;						/* r/o �t�@���N�V�����ԍ� */
unsigned a, b, de, hl, ix;	/* r/o �e���W�X�^�ւ̐ݒ�l */
XREG *r;					/* w/o �e���W�X�^�ւ̖߂�l */
{
	r->bc =	c;	/* c���t�@���N�V�����ԍ� */
	r->bc += b << 8;
	r->af =	a << 8;
	r->de =	de;
	r->hl =	hl;
	r->ix =	ix;

	callxx( BDOS, r );	/* BDOS�R�[�� */

	return r->af >> 8;	/* A:�G���[�R�[�h */
}


/**************************************************************
	BDOS�R�[���i�G���[���b�Z�[�W�\������j
	return�l�F	OK:����(A���W�X�^=0) ERROR:���s(A���W�X�^!=0)
**************************************************************/
STATUS bdos_trap( c, a, b, de, hl, ix, r, fname )
TINY c;						/* r/o �t�@���N�V�����ԍ� */
unsigned a, b, de, hl, ix;	/* r/o �e���W�X�^�ւ̐ݒ�l */
XREG *r;					/* w/o �e���W�X�^�ւ̖߂�l */
char *fname;				/* �Ăяo�����֐��� �f�o�b�O�p */
{
	int iret = 0;	/* return�l */
	STATUS sret = ERROR;
	
	/* �ۓ��� */
	iret= bdoscall( c, a, b, de, hl, ix, r );
	
	if ( iret == 0 ) {
		/* �G���[�łȂ���� */
		sret = OK;
	} else {
		/* �G���[�̏ꍇ�̓��b�Z�[�W��\�� */
		errmsgbox( iret, fname );
	}
	
	return sret;
}


/**************************************************************
	�t�@�C�������r�b�g�̊m�F
**************************************************************/
VOID attr( a )
TINY a;		/* r/o �t�@�C�������r�b�g */
{
/*
�t�@�C������
+�f�o�C�X����			�r��
|+���0
||+�A�[�J�C�u
|||+�f�B���N�g��		�ƒʏ�̃t�@�C��
00ad vshr
     +�{�����[����		�r��
      +�V�X�e���t�@�C��
       +�s���t�@�C��
        +R/O
*/
	if ( a & 0x01 ) puts("/r");
	if ( a & 0x02 ) puts("/h");
	if ( a & 0x04 ) puts("/s");
	if ( a & 0x08 ) puts("/v");
	if ( a & 0x10 ) puts("/d");
	if ( a & 0x20 ) puts("/a");
	if ( a & 0x40 ) puts("/?");
	if ( a & 0x80 ) puts("/!");
}


/**************************************************************
	SLONG�l�̐��`
	return�l�F*str
**************************************************************/
/*12345678
�Ennn,nnn		<1,000,000
�Enn,nnnK		<  100,000k = 102,400,000
�Enn,nnnM		>  100,000k ��ulong=4,294,967,295
*/
char *sputsize( str, psl )
char	*str;	/* w/o ���𕶎���ŕԂ� \0���݂�8�o�C�g�m�ۂ��邱�� */
SLONG	*psl;	/* r/o ������^�ɕϊ����鐔�l */
{
	int		iret;
	char	swk[12];			/* �e�L�X�g���`�p���[�N */
	SLONG	slv1, slv2, slans, slkilo;	/*SLONG�^�v�Z�p���[�N*/
/*
	printf( "* <putsize()1:%ld>\n", psl );	��OK
	printf( "* <putsize()2:%ld>\n", &psl );	��NG
*/
	strcpy(  str, "ABCDEFG" );

	/* 1,000,000�������m�F */
	itosl( &slv1, 1000 );
	if( slcmp( psl, &slv1 ) == -1 ) {	/* psl < 1,000 ? */

		/* 1,000�����Ȃ�J���}���Ȃ� */
					/* 1234 567n*/
		sprintf( str, "    %3ld", psl );

	} else {
		
/*		itosl( &slv1, 1000 ); */
		slmul( &slans, &slv1, &slv1 );	/* 1,000,000 */
		if( slcmp( psl, &slans ) == -1 ) {	/* psl < 1,000,000 ? */
			
			/* 1,000,000���� */
			sprintf( swk, "%11ld", psl );
			str[0] = swk[ 5];	/* ��3�� */
			str[1] = swk[ 6];
			str[2] = swk[ 7];
			str[3] = ',';
			str[4] = swk[ 8];	/* ��3�� */
			str[5] = swk[ 9];
			str[6] = swk[10];
			
		} else {
			
			slcpy( &slans, psl );		/* ���H�p�ɕ��� */
			/* kB�P�ʂɕϊ� (��1024) */
			slsra( &slkilo, &slans, (TINY)10 );
			/* 100,000k�������m�F(102,400,000) */
/*			itosl( &slv1, 1000 ); */
			itosl( &slv2,  100 );
			slmul( &slans, &slv1, &slv2 );	/* 100,000 */
			
			if( slcmp( &slkilo, &slans ) == -1 ) { /* slkilo < 100,000k */
				
				/* 100,000k���� */
				sprintf( swk, "%11ld", &slkilo );
				str[0] = swk[ 6];	/* ��2�� */
				str[1] = swk[ 7];
				str[2] = ',';
				str[3] = swk[ 8];	/* ��3�� */
				str[4] = swk[ 9];
				str[5] = swk[10];
				str[6] = 'k';		/* �L�� */
				
			} else {
				
				/* MB�P�ʂɕϊ� (k��1024) */
				/* SLONG�͈͓̔��ł͌��s���ɂ͂Ȃ�Ȃ� */
				slsra( &slans, &slkilo, (TINY)10 );
				sprintf( swk, "%11ld", &slans );
				str[0] = swk[ 6];	/* ��2�� */
				str[1] = swk[ 7];
				str[2] = ',';
				str[3] = swk[ 8];	/* ��3�� */
				str[4] = swk[ 9];
				str[5] = swk[10];
				str[6] = 'M';		/* �L�� */
			}
		}
	}
	
	return str;
}

