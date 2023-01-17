/**************************************************************
	DD_MSG.c ��ʕ\���i���b�Z�[�W�{�b�N�X�֘A�j
**************************************************************/
#include <stdio.h>
#include <glib.h>		/* vpeek, vpoke */
#include <bdosfunc.h>	/* XREG */
#include "dd.h"			/* math.h���܂� */

/* dd_sub.c�ɂĒ�` */
extern int bdoscall( c, a, b, de, hl, ix, r );



/**************************************************************
	VPOKE�ɂ�镶����\���F�r���󎚗p
	str�̕�����-40h���炵�Ē���VRAM�ɏ�������
**************************************************************/
VOID vputsgraph( x, y, str )
int		x;		/* r/o X���W 1�` */
int		y;		/* r/o Y���W 1�` */
char	*str;	/* r/o �\�����镶���� */
{
	int i;
	int ilen = 0;
	char buf[40];
	
	
	ilen = strlen( str );
	if ( 0 < ilen ) {
		
		if ( 40 < ilen ) ilen = 40;
		
		/* ��ɕϊ����Ă����Č�Ńu���b�N�]�� */
		for ( i=0; i < ilen; i++ ) {
			buf[i] = str[i] - 0x40;
		}

		/*SCREEN0�̕����ʒu�� 0+Y*40+X */
		ldirvm( (y-1) * 40 + (x-1), buf, ilen );
/*
		for ( i=0; i<strlen(str); i++ ) {
			/*SCREEN0�̕����ʒu�� 0+Y*40+X */
			vpoke( (y-1) * 40 + (x-1) + i, str[i] - 0x40 );
		}
*/
	}
}


/**************************************************************
	VPOKE�ɂ�镶����\���F�p���󎚗p
	str�̕�����啶������+A0h���炵�Ē���VRAM�ɏ�������
**************************************************************/
VOID vputs_b( x, y, str )
int		x;		/* r/o X���W 1�` */
int		y;		/* r/o Y���W 1�` */
char	*str;	/* r/o �\�����镶���� */
{
	int i;
	int ilen = 0;
	char ch;
	char buf[40];
	
	
	ilen = strlen( str );
	if ( 0 < ilen ) {

		if ( 40 < ilen ) ilen = 40;
		
		/* ��ɕϊ����Ă����Č�Ńu���b�N�]�� */
		for ( i=0; i < ilen; i++ ) {

			/*�啶���ł����+A0h*/
			ch = str[i];
			if( isupper(ch) ) ch += 0xa0;
			buf[i] = ch;
	/*
			vpoke( (y-1) * 40 + (x-1) + i, ch );
	*/
		}

		/*SCREEN0�̕����ʒu�� 0+Y*40+X */
		ldirvm( (y-1) * 40 + (x-1), buf, ilen );
		
	}
}


/**************************************************************
	���b�Z�[�W�{�b�N�X�̘g����
**************************************************************/
VOID putbox( x, y, w, h )
int x;			/* r/o ����X */
int y;			/* r/o ����Y */
int w;			/* r/o �S�� ������-2 */
int h;			/* r/o �S�� �X�y�[�X�s��-2  */
{
	int  i;
	char skeisen[42], sspace[42];

	strcpy( skeisen, "X" );	/* �� */
	strcpy( sspace,  "V" );	/* �� */
	
	/* ���r������� */
	for ( i=0; i<w-2; i++ ) {
		skeisen[1+i] = 'W';	/* �� */
		sspace[1+i] = '@';	/* �X�y�[�X */
	}
	
	skeisen[w-1] = 'Y';		/* �� */
	skeisen[w] = '\0';
	
	sspace[w-1] = 'V';		/* �� */
	sspace[w] = '\0';
	
	/* ��� */
/*	skeisen[0] = 'X';
	skeisen[w-1] = 'Y';
*/
	vputsgraph( x, y+0, skeisen );

	/* ���i */
	for ( i=0; i<h-2; i++ ) {
		vputsgraph( x,     y+1+i, sspace );
/*
		vputsgraph( x,     y+1+i, "V" );	/* �E */
		vputsgraph( x+w-1, y+1+i, "V" );	/* �� */
		locate( (TINY)(x+1), (TINY)(y+1+i) );
		puts( sspace );
*/
	}
	
	/* ���� */
	skeisen[0] = 'Z';	/* �� */
	skeisen[w-1] = '[';	/* �� */
	vputsgraph( x, y+h-1, skeisen );

}


/**************************************************************
	���b�Z�[�W�{�b�N�X
**************************************************************/
VOID msgbox( x, y, stitle, stext )
int x;			/* r/o ����X 1�` 0�̏ꍇ��title��str�̒���������v�Z */
int y;			/* r/o ����Y */
char *stitle;	/* r/o �^�C�g�� �����\������� */
char *stext;	/* r/o �{�� �����\������Ȃ� */
{
	int iw, ix, i;
	char ch;
	
	/* �����𓾂� */
	iw = max( strlen( stitle ), strlen( stext ) ) + 4;
	if( 40 < iw ) iw = 40;
	
	ix = 21 - iw/2;
	
	/* �g��\�� */
	putbox( ix, 10, iw, 4 );
	
	/* �^�C�g�� */
	vputs_b( ix+1, 11, stitle );

	/* �e�L�X�g�s */
	locate( (TINY)(ix + 2), (TINY)12 );
	puts( stext );

locate((TINY)ix, (TINY)14);printf("* <iw=%d; ix=%d>", iw, ix);

	ch = getch();
	
}


/**************************************************************
	�v���O���X�o�[�p�{�b�N�X
	�R�s�[�E�ړ��E�폜�̐i���󋵕\����z��
	X13, Y10����W14, H3�̕\���̈�����
	1�s�ځFX14,   Y11 �^�C�g��	vputs_b( 14, 11, "COPYING..." );
	2�s�ځFX14+1, Y12 ���e		locate( (TINY)15, (TINY)12 );puts()...
	3�s�ځFX14,   Y13 �v���O���X�o�[
		�i���\���ɂ�X15, Y13����12�������g�p����
**************************************************************/
VOID progressbox( stitle )
char *stitle;	/* r/o �^�C�g�� �����\������� */
{
	putbox( 13, 10, 16, 5 );			/* �g��\�� */
	vputs_b( 14, 11, stitle );
	vputsgraph( 14, 13, "TJJJJJJJJJJJJS" );
/*
            1234567890123456
            |COPYING...    |
            | hoehoeho.txt |
            |[123456789012]|
1234567890123456789012345678901234567890
| ABCDEFGH.TXT xx,xxx 00-00-00 00:00U|U|
*/
}


/**************************************************************
	�v���O���X�o�[�̕\��
	1�s�ځF�������Ȃ�
	2�s�ځF�w�肳�ꂽ�t�@�C������\������
	3�s�ځF�v���O���X�o�[��\������
**************************************************************/
VOID putprogress( stext, idenomi, inum )
char	*stext;		/* r/o �^�C�g�� �����\������� */
int		idenomi;	/* r/o �v���O���X�o�[�̕��� */
int		inum;		/* r/o �v���O���X�o�[�̕��q */
{
	int i=0;
	
	/* �����Ώۃt�@�C���̕\�� */
	locate( (TINY)15, (TINY)12 );
	printf( "%-12s", stext );
	
	/* �v���O���X�o�[��i�߂�(0.5�i�߂Ă���) */
	for ( i=0; i < 12 * (inum*2+1) / (idenomi*2); i++ ){
		vputsgraph( 15+i, 13, "P" );
	}
}


/**************************************************************
	�G���[���b�Z�[�W�{�b�N�X
**************************************************************/
VOID errmsgbox( icode, fname )
int icode;		/* r/o �G���[�R�[�h */
char *fname;	/* r/o ���i�����ʒu�Ȃǁj */
{
	char	ch;
	char	title[40];	/* �^�C�g�� */
	char	msg[72];	/* �G���[���b�Z�[�W�p������ �Œ�64�o�C�g */
	XREG	reg;		/* BDOS�R�[������̖߂�l�擾�p */
	
	bdoscall( _EXPLAIN,		/* �G���[�R�[�h�̐���������𓾂� */
		0, 
		icode,			/* �G���[�R�[�h */
		(unsigned)msg, 
		0, 0,
		&reg			/* �߂�l���p */
		 );
		 
	sprintf( title, "ERROR at %s", fname );
/*
strcpy(msg,
	"12345678901234567890123456789012this is over 32bytes sentence!!!");
*/
/*	 1234567890123456789012345678901234567890123456789012345678901234*/

	if ( strlen( msg ) <= 32 ) {
		
		msgbox( 0, 0, title, msg );
		
	} else {

		/* �G���[���b�Z�[�W�������ꍇ��2�s�ŕ\������ */
		putbox(  3, 10, 36, 5 );
		vputs_b( 4, 11, title );
		
		/* 2�s�� */
		locate(  (TINY)5, (TINY)13 );
		puts( msg+32 );
		
		/* 1�s�� */
		msg[32] = '\0';
		locate(  (TINY)5, (TINY)12 );
		puts( msg );
		
	}
	
	ch = getch();

}

