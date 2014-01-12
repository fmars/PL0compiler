#ifndef _MHEADER
#define _MHEADER

#include<stdio.h> //For <memset>,<fseek>
#include<stdlib.h>//For <atoi>
#include<string.h>//For <strcmp>
#define ISLETTER(c)	((c)>='A'&&(c)<='Z'||(c)>='a'&&(c)<='z')//判断是否是字母
#define ISNUMBER(c)	((c)>='0'&&(c)<='9')	//判断是否是数字
#define ISCHAR(c)	((c)>=33 &&(c)<=126)	//判断是否是字符
#define ISSQMRK(c) 	((c)==39)				//判断是否是单引号
#define ISDQMRK(c) 	((c)==34)				//判断是否是双引号


#define MAX_ASM			1024	//最多目标代码条数
#define MAX_ASMLEN     32      //每条汇编指令长度
#define MAX_ASMHEAD     32       //汇编文件头指令数
#define MAX_ASMHEADLEN  128     //汇编文件头指令长度
#define MAX_ASMDATA     128      //汇编数据区大小
#define MAX_ASMDATALEN  128     //数据区指令长度
#define MAX_ERR			256		//最多显示错误数
#define MAX_FORM		1024	//最大符号表长度
#define MAX_LEVEL		128		//最大符号表层数
#define MAX_SYMLEN		128		//最大符号长度 包括关键字、标示符和数字、字符串
#define MAX_NUMLEN		128		//最大常数长度
#define MAX_NUMBER		65535	//最大常数
#define MAX_VARNUM      128      //一条变量声明语句最大含有变量数
#define MAX_MLCODE      1024*8  //最大四元式数量
#define MAX_CONSTEACHROW 128    //switch语句每行做多常量数
#define MAX_ROW         128     //switch语句做多行数
#define MAX_DAG			128		//代码优化dag最多中间节点数

#define MAX_KEYWORD		20		//关键字数量
#define MAX_OPWORDA		7		//单字运算符数量
#define MAX_OPWORDB		4		//双字运算符数量
#define MAX_ENDWORD		8		//单字界符数量
#define MAX_ERROR		34		//错误类型数量
#define MAX_ASMWORD		8		//汇编指令条数


#define ERR_1			1		//字符长度超过1
#define ERR_2			2		//字符串长度超过限度
#define ERR_3			3		//常量声明错误
#define ERR_4			4		//变量声明错误
#define ERR_5			5		//缺少左括号
#define ERR_6			6		//缺少右括号
#define ERR_7			7		//缺少冒号
#define ERR_8			8		//形式参数错误
#define ERR_9			9		//缺少分号
#define ERR_10			10		//分程序缺少结束
#define ERR_11			11		//未定义标示符
#define ERR_12			12		//缺少逗号
#define ERR_13			13		//缺少IF
#define ERR_14			14		//不能作为因子
#define ERR_15			15		//缺少比较符号
#define ERR_16			16		//缺少THEN
#define ERR_17			17		//缺少FOR
#define ERR_18			18		//需要变量
#define ERR_19			19		//缺少:=
#define ERR_20			20		//缺少to或downto
#define ERR_21			21		//缺少DO
#define ERR_22			22		//缺少CASE
#define ERR_23			23		//缺少OF
#define ERR_24			24		//需要常量
#define ERR_25			25		//情况语句不正常结束
#define ERR_26			26		//缺少END
#define ERR_27			27		//READ语句需要变量
#define ERR_28			28		//错误赋值语句




enum SYM_TYPE{          /*词法分析-符号类型*/
        TYPE_ERROR,	    //错误类型号
		TYPE_KEYWORD,	//关键字类型号
		TYPE_SYMBOL,	//符号类型号
		TYPE_NUMBER,	//常量类型号
		TYPE_CHAR,		//字符类型号
		TYPE_STRING,	//字符串类型号
		TYPE_OPWORD,	//运算符类型号
		TYPE_ENDWORD,	//界符类型号
};//为每个符号

enum DEC_TYPE{          /*声明类型*/
        TYPE_CONST,     //常量
        TYPE_VAR,       //变量
        TYPE_PROCEDURE, //过程
        TYPE_FUNCTION,  //函数
        TYPE_TMPVAR,    //临时变量
        TYPE_LABEL,     //标号
		TYPE_REG,		//寄存器
        TYPE_NULL,      //空
};//为符号表

typedef struct		//符号描述结构体(含错误描述结构)
{
	SYM_TYPE type;		//类型号(0:错误)
	int		id;		//ID号(错误值)     //关键字-关键字号    //数字-数字的值    //字符-字符的值  //运算符-运算符号，包括单目双目运算符  //界符-界符号
	int		line;		//所在行数
	char	name[MAX_SYMLEN+1];	//所取的词
}SYM;

typedef struct		/*符号表*/
{
	DEC_TYPE type;		//类型
	int     kind;       //1:整数 2:字符 3:array[]of int 4:array[]of char //函数的返回值类型
	int		lvl;		//层数
	int		adr;		//变量的相对地址，临时变量的次序,函数局部变量的个数
	char	name[MAX_SYMLEN+1];	//名称
	int    val; //常量的值，数组的大小，函数的参数个数，参数是否是传址引用
                //函数声明作为参数时，表明此参数是不是var
                    //0表示是局部变量
                    //1表示是参数且是var参数
                    //2表示是参数但不是var参数
}TABLE;

enum ML_TYPE{
    ML_PLUS,ML_MINUS,ML_TIMES,ML_SLASH,ML_BEC,ML_BEGIN,ML_END,ML_PUSH,ML_POP,ML_CALL,
    ML_CMP,ML_JWNT,ML_JMP,ML_LABEL,ML_READ,ML_WRITE,ML_NULL,ML_REG
};

typedef struct      /*四元式*/
{
    ML_TYPE type;   //操作类型
    TABLE   x1;
    TABLE   x2;
    TABLE   x3;
}MLCODE;

//代码优化
typedef struct		//DAG的节点
{
	int		p[MAX_DAG];	//父节点,p[0]表示父节点个数
	int		lc,rc;	//左右子节点
	TABLE	t;		//叶子节点--初始值  中间节点--表示值
	ML_TYPE	c;			//运算符号 0-3:+-*/
}NODE1;
typedef struct		//DAG的节点表的表项
{
	TABLE	t;			//变量
	int		p;			//节点号
}NODE2;

#endif
