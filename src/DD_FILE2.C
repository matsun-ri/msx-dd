/**************************************************************
	�t�@�C���ꗗ�̎擾�E�\��
**************************************************************/
#include <stdio.h>
/* #include <bdosfunc.h> */
#include <glib.h>		/* ldirvm */
/* #include <msxbios.h>	 locate */
/* #include <math.h>	 slcpy */
#include "dd.h"


/* dd_scrn.c�ɂĒ�` */
extern VOID vputs_b( x, y, str );
extern VOID vputsgraph( x, y, str );




/**************************************************************
	�t�@�C���ꗗ��\��
	main()��filelist()
**************************************************************/
VOID filelist( files, ifiles, ilistcur, ilistpage, itabpage )
FILES	*files;		/* r/o FILES�^�\���̂ւ̃|�C���^ */
int		ifiles;		/* r/o �t�@�C���� */
int		ilistcur;	/* r/o �J�[�\�������ڂ̃t�@�C�����w���Ă��邩 */
int		ilistpage;	/* r/o �y�[�W �ŏ�=0 */
int		itabpage;	/* r/o ���쒆�̃^�u�y�[�W 0or1 */
{
	int		i, ipos,  iup, idown;


	/* �c�_�O���t�\�� */

	iup = 0;

	if( 0 == ifiles ) { 

		/* �t�@�C�����Ȃ��ꍇ�́��Ŗ��߂� */
		iup = idown = LIST_H - 2;

	} else {

		/* �܂��󔒂Ŗ��߂� */
		for ( i=0; i < LIST_H - 2; i++ ) {
			vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "U" );
		}

		/* �\�����̃y�[�W�̍ŏ�ƍŉ��̃t�@�C���ɓ����镔�������߂� */
		/* �y�[�W���ň�ԏ�̃t�@�C�� ����=LIST_H-2 */
		iup = ilistpage * (LIST_H-2) * (LIST_H-2) / ifiles;
		/* �y�[�W���ň�ԉ��̃t�@�C�� ����=LIST_H-2 */
		ipos = (ilistpage + 1) * (LIST_H-2) - 1;
		/* �ŏI�y�[�W�͏�]�������\������Ă��Ȃ��̂ŏC�� */
		if ( ifiles <= ipos ) ipos = ifiles - 1;
		idown = (LIST_H-2) * ipos / (ifiles - 1);
		if ( idown <= iup ) idown = iup + 1; /* �Œ�ł�1���[�v������ */
		
	}

	/* ������Ŗ��߂� */
	for( i=0; i<iup; i++ ) {
		vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "U" );
	}
	/* �Ԃ����Ŗ��߂� */
	for( i=iup; i<idown; i++ ) {
		vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "\\" );
	}
	/* �������Ŗ��߂� */
	for( i=idown; i<LIST_H-2; i++ ) {
		vputsgraph( LIST_X + 34 + itabpage*2, LIST_Y+1 + i, "U" );
	}

	/* �t�@�C�����̕\�� */
	for ( i = 0; i < LIST_H; i++ ) {
		
		ipos = LIST_H * ilistpage + i;

		/* �܂��Ō�̃t�@�C���ł͂Ȃ��H */
		if( ipos < ifiles ) {
			
			/* �t�@�C����\�� */
/*
			puts( files[ ipos ].image );
*/
			ldirvm(  (LIST_Y + i            - 1) * 40 
					+ LIST_X + itabpage * 2 - 1,
					files[ ipos ].image,	
					34 );
					
			/* �}�[�N�̕\��/��\�� */
			locate( (TINY)(LIST_X + itabpage*2 - 1), (TINY)(LIST_Y + i) );
			if( files[ ipos ].ismarked ) {
				putchar( '*' ); 
			} else {
				putchar( ' ' ); 
/*				putchar( '-' ); */
			}

		} else {
			
			/* �t�@�C���ł͂Ȃ��ꏊ */
			filvrm(  (LIST_Y + i            - 1 ) *40 
					+ LIST_X + itabpage * 2 - 2,
					35, ' ' );
/*					35, '/' );
*/
/*
			/* �}�[�N�̏��� */
			putchar( ' ' ); 
			/*  �X�y�[�X��\��*/
			/*     1234567890123456789012345678901234567890*/
			puts( "----------------------------------" );
*/
		}
	}
/*		
	/* �J�[�\���\�� */
	putcursor( ilistcur + itabpage*2 , 'g', ' ' );
*/
	
	/* �J�[�\���ʒu�̃t�@�C�����𑾎��\������ */
	vputs_b( LIST_X + itabpage*2, LIST_Y + ilistcur % LIST_H, files[ ilistcur ].image );

}



