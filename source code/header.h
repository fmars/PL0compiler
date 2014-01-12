#ifndef _MHEADER
#define _MHEADER

#include<stdio.h> //For <memset>,<fseek>
#include<stdlib.h>//For <atoi>
#include<string.h>//For <strcmp>
#define ISLETTER(c)	((c)>='A'&&(c)<='Z'||(c)>='a'&&(c)<='z')//�ж��Ƿ�����ĸ
#define ISNUMBER(c)	((c)>='0'&&(c)<='9')	//�ж��Ƿ�������
#define ISCHAR(c)	((c)>=33 &&(c)<=126)	//�ж��Ƿ����ַ�
#define ISSQMRK(c) 	((c)==39)				//�ж��Ƿ��ǵ�����
#define ISDQMRK(c) 	((c)==34)				//�ж��Ƿ���˫����


#define MAX_ASM			1024	//���Ŀ���������
#define MAX_ASMLEN     32      //ÿ�����ָ���
#define MAX_ASMHEAD     32       //����ļ�ͷָ����
#define MAX_ASMHEADLEN  128     //����ļ�ͷָ���
#define MAX_ASMDATA     128      //�����������С
#define MAX_ASMDATALEN  128     //������ָ���
#define MAX_ERR			256		//�����ʾ������
#define MAX_FORM		1024	//�����ű���
#define MAX_LEVEL		128		//�����ű����
#define MAX_SYMLEN		128		//�����ų��� �����ؼ��֡���ʾ�������֡��ַ���
#define MAX_NUMLEN		128		//���������
#define MAX_NUMBER		65535	//�����
#define MAX_VARNUM      128      //һ�����������������б�����
#define MAX_MLCODE      1024*8  //�����Ԫʽ����
#define MAX_CONSTEACHROW 128    //switch���ÿ�����ೣ����
#define MAX_ROW         128     //switch�����������
#define MAX_DAG			128		//�����Ż�dag����м�ڵ���

#define MAX_KEYWORD		20		//�ؼ�������
#define MAX_OPWORDA		7		//�������������
#define MAX_OPWORDB		4		//˫�����������
#define MAX_ENDWORD		8		//���ֽ������
#define MAX_ERROR		34		//������������
#define MAX_ASMWORD		8		//���ָ������


#define ERR_1			1		//�ַ����ȳ���1
#define ERR_2			2		//�ַ������ȳ����޶�
#define ERR_3			3		//������������
#define ERR_4			4		//������������
#define ERR_5			5		//ȱ��������
#define ERR_6			6		//ȱ��������
#define ERR_7			7		//ȱ��ð��
#define ERR_8			8		//��ʽ��������
#define ERR_9			9		//ȱ�ٷֺ�
#define ERR_10			10		//�ֳ���ȱ�ٽ���
#define ERR_11			11		//δ�����ʾ��
#define ERR_12			12		//ȱ�ٶ���
#define ERR_13			13		//ȱ��IF
#define ERR_14			14		//������Ϊ����
#define ERR_15			15		//ȱ�ٱȽϷ���
#define ERR_16			16		//ȱ��THEN
#define ERR_17			17		//ȱ��FOR
#define ERR_18			18		//��Ҫ����
#define ERR_19			19		//ȱ��:=
#define ERR_20			20		//ȱ��to��downto
#define ERR_21			21		//ȱ��DO
#define ERR_22			22		//ȱ��CASE
#define ERR_23			23		//ȱ��OF
#define ERR_24			24		//��Ҫ����
#define ERR_25			25		//�����䲻��������
#define ERR_26			26		//ȱ��END
#define ERR_27			27		//READ�����Ҫ����
#define ERR_28			28		//����ֵ���




enum SYM_TYPE{          /*�ʷ�����-��������*/
        TYPE_ERROR,	    //�������ͺ�
		TYPE_KEYWORD,	//�ؼ������ͺ�
		TYPE_SYMBOL,	//�������ͺ�
		TYPE_NUMBER,	//�������ͺ�
		TYPE_CHAR,		//�ַ����ͺ�
		TYPE_STRING,	//�ַ������ͺ�
		TYPE_OPWORD,	//��������ͺ�
		TYPE_ENDWORD,	//������ͺ�
};//Ϊÿ������

enum DEC_TYPE{          /*��������*/
        TYPE_CONST,     //����
        TYPE_VAR,       //����
        TYPE_PROCEDURE, //����
        TYPE_FUNCTION,  //����
        TYPE_TMPVAR,    //��ʱ����
        TYPE_LABEL,     //���
		TYPE_REG,		//�Ĵ���
        TYPE_NULL,      //��
};//Ϊ���ű�

typedef struct		//���������ṹ��(�����������ṹ)
{
	SYM_TYPE type;		//���ͺ�(0:����)
	int		id;		//ID��(����ֵ)     //�ؼ���-�ؼ��ֺ�    //����-���ֵ�ֵ    //�ַ�-�ַ���ֵ  //�����-������ţ�������Ŀ˫Ŀ�����  //���-�����
	int		line;		//��������
	char	name[MAX_SYMLEN+1];	//��ȡ�Ĵ�
}SYM;

typedef struct		/*���ű�*/
{
	DEC_TYPE type;		//����
	int     kind;       //1:���� 2:�ַ� 3:array[]of int 4:array[]of char //�����ķ���ֵ����
	int		lvl;		//����
	int		adr;		//��������Ե�ַ����ʱ�����Ĵ���,�����ֲ������ĸ���
	char	name[MAX_SYMLEN+1];	//����
	int    val; //������ֵ������Ĵ�С�������Ĳ��������������Ƿ��Ǵ�ַ����
                //����������Ϊ����ʱ�������˲����ǲ���var
                    //0��ʾ�Ǿֲ�����
                    //1��ʾ�ǲ�������var����
                    //2��ʾ�ǲ���������var����
}TABLE;

enum ML_TYPE{
    ML_PLUS,ML_MINUS,ML_TIMES,ML_SLASH,ML_BEC,ML_BEGIN,ML_END,ML_PUSH,ML_POP,ML_CALL,
    ML_CMP,ML_JWNT,ML_JMP,ML_LABEL,ML_READ,ML_WRITE,ML_NULL,ML_REG
};

typedef struct      /*��Ԫʽ*/
{
    ML_TYPE type;   //��������
    TABLE   x1;
    TABLE   x2;
    TABLE   x3;
}MLCODE;

//�����Ż�
typedef struct		//DAG�Ľڵ�
{
	int		p[MAX_DAG];	//���ڵ�,p[0]��ʾ���ڵ����
	int		lc,rc;	//�����ӽڵ�
	TABLE	t;		//Ҷ�ӽڵ�--��ʼֵ  �м�ڵ�--��ʾֵ
	ML_TYPE	c;			//������� 0-3:+-*/
}NODE1;
typedef struct		//DAG�Ľڵ��ı���
{
	TABLE	t;			//����
	int		p;			//�ڵ��
}NODE2;

#endif
