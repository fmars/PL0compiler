#include"header.h"
#include<string.h>
//复制一个TABLE项
void table_copy(TABLE&t2,TABLE t1){
    //12.17 if(t1.type==TYPE_NULL)return;
    t2.type=t1.type;
	t2.kind=t1.kind;
	t2.lvl=t1.lvl;
    t2.adr=t1.adr;
    if(t1.type!= TYPE_NULL)
        strcpy(t2.name,t1.name);
	else 
		strcpy(t2.name,"");
	t2.val=t1.val;
}
int table_equal(TABLE t1,TABLE t2){
	if(t1.type==t2.type && t1.kind==t2.kind && t1.adr==t2.adr && t1.lvl==t2.lvl )return 1;
	else return 0;
}

//生成四元式代码
void mlgen(int &mlcode_n,MLCODE *mlcode,ML_TYPE type,TABLE x1,TABLE x2,TABLE x3){
	mlcode_n++;
    mlcode[mlcode_n].type = type;
    table_copy(mlcode[mlcode_n].x1,x1);
    table_copy(mlcode[mlcode_n].x2,x2);
    table_copy(mlcode[mlcode_n].x3,x3);
}
//生成临时变量对应TABLE
void tabletmpval(int x,TABLE&t){
    char tmp[MAX_SYMLEN];
    memset(&t,0,sizeof(TABLE));
	t.type=TYPE_TMPVAR;
	t.adr=x;
	strcpy(t.name,"!t");
	itoa(x,tmp,10);
	strcat(t.name,tmp);
}
//生成常量对应的TABLE
void tableconst(int x,TABLE&t){
    char tmp[MAX_SYMLEN];
    memset(&t,0,sizeof(TABLE));
    t.type=TYPE_CONST;
    t.val=x;
    itoa(x,tmp,10);
    strcpy(t.name,tmp);
}
//生成标号对应的TABLE
void tablelabel(int x,TABLE&t){
    char tmp[MAX_SYMLEN];
    memset(&t,0,sizeof(TABLE));
	t.type=TYPE_LABEL;
	t.adr=x;
	strcpy(t.name,"@label");
	itoa(x,tmp,10);
	strcat(t.name,tmp);
}

/*寻址*/
void getaddress(int &asmcode_n,char asmcode[MAX_ASM][MAX_ASMLEN],
    int lvl_now,//当前层次
    int locvar_n,//局部变量个数
    int para_n,//参数个数
    int tmpvar_n,//临时变量个数
    TABLE t,
    char r[4]){
	//寄存器
	if( t.type == TYPE_REG){
		sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,%s",r,t.name);
	}
    //临时变量
    else if( t.type == TYPE_TMPVAR ){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,ebp",r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,locvar_n*4+t.adr*4);
    }
    //局部变量，非数组与数组
    else if( t.type == TYPE_VAR &&(t.val==0 || t.kind==3||t.kind==4 )){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*t.adr);
    }
    //var参数
    else if( t.type == TYPE_VAR && t.val==1){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=基指针
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
    //非var参数
    else if( t.type == TYPE_VAR && t.val==2){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=基指针
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
    }
    //函数
    else if( t.type == TYPE_FUNCTION){//函数
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl));//edx=基指针
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,4",r);
    }
    }
/*寻值*/
void getvalue(int &asmcode_n,char asmcode[MAX_ASM][MAX_ASMLEN],
    int lvl_now,//当前层次
    int locvar_n,//局部变量个数
    int para_n,//参数个数
    int tmpvar_n,//临时变量个数
    TABLE t,
    char r[4])
{
	//寄存器
	if( t.type == TYPE_REG){
		sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,t.name);
	}
    //常量
    else if( t.type == TYPE_CONST ){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,%d",r,t.val);
    }
    //临时变量
    if( t.type == TYPE_TMPVAR ){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp-%d]",r,locvar_n*4+t.adr*4);
    }
    //局部变量，非数组与数组
    else if( t.type == TYPE_VAR &&(t.val==0 || t.kind==3||t.kind==4 )){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s-%d]",r,r,4*t.adr);
    }
    //var参数
    else if( t.type == TYPE_VAR && t.val==1){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=基指针
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
    //非var参数
    else if( t.type == TYPE_VAR && t.val==2){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=基指针
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
    //函数
    else if( t.type == TYPE_FUNCTION){//函数
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl));//edx=基指针
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,4",r);
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
}




//四元式产生汇编代码
void asmgen(int mlcode_n,MLCODE *mlcode,int &asmcode_n,char asmcode[MAX_ASM][MAX_ASMLEN],
    int &asmcodehead_n,char asmcodehead[MAX_ASMHEAD][MAX_ASMHEADLEN],
    int &asmcodedata_n,char asmcodedata[MAX_ASMDATA][MAX_ASMDATALEN]){//四元式产生汇编
    //汇编程序头
    strcpy( asmcodehead[0],"end _main");//输出到汇编文件末尾
    strcpy( asmcodehead[++asmcodehead_n],"	.386");
    strcpy( asmcodehead[++asmcodehead_n],"	.model flat, stdcall  ; 32 bit memory model");
    strcpy( asmcodehead[++asmcodehead_n],"	option casemap :none  ; case sensitive");
    strcpy( asmcodehead[++asmcodehead_n],"	include \\masm32\\include\\io32.inc ");
    //汇编程序.data段
    strcpy( asmcodedata[0],".data");
    //汇编程序
    //四元式指令
    //ML_BEGIN
    //ML_END
    //ML_CALL
    //ML_JMP
    //ML_LABEL
    //ML_JWNT
    //ML_PLUS
    //ML_MINUS
    //ML_TIMES
    //ML_SLASH
    //ML_PUSH
    //ML_POP
    //ML_CMP
    //ML_BEC
	//ML_READ
	//ML_WRITE
    int i,j,k;
    int lvl_now;//当前层次
    int locvar_n;//局部变量个数
    int para_n;//参数个数
    int tmpvar_n;//临时变量个数

    strcpy( asmcode[ ++asmcode_n ],".code");
    for( i=1;i<=mlcode_n;i++ ){
        switch( mlcode[i].type ){
            case ML_BEGIN:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s",";BEGIN");

                lvl_now = mlcode[i].x2.lvl;
                locvar_n = mlcode[i].x2.adr;
                para_n = mlcode[i].x2.val;
                tmpvar_n = mlcode[i].x3.val;

                //函数头
                sprintf(asmcode[ ++asmcode_n ],"_%s %s",mlcode[i].x2.name,"PROC");
                //保存先前栈底指针，并指向当前栈底
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH ebp");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","MOV ebp,esp");
                //返填自己的ebp
                sprintf(asmcode[ ++asmcode_n ],"\t%s","MOV [esp+8],ebp");
                //给局部变量、临时变量腾地方
                sprintf(asmcode[ ++asmcode_n ],"\t%s%d","SUB esp,",(locvar_n+tmpvar_n)*4);
                //保存寄存器堆
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH ebx");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH esi");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH edi");
                break;
            }
            case ML_END:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s",";END");
                //还原寄存器堆
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP edi");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP esi");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP ebx");
                //还原堆栈
                sprintf(asmcode[ ++asmcode_n ],"\t%s%d","ADD esp,",(locvar_n+tmpvar_n)*4);
                //还原先前栈底指针
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP ebp");
                //返回语句
                sprintf(asmcode[ ++asmcode_n ],"\t%s","RET");
                //函数尾
                sprintf(asmcode[ ++asmcode_n ],"_%s %s",mlcode[i].x2.name,"ENDP");
                break;
            }
            case ML_CALL:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s",";CALL",mlcode[i].x2.name);
                //复制display区
                for( j=1;j<=mlcode[i].x2.lvl-1;j++ ){
                    k = 8+4*lvl_now-j*4;
                    sprintf(asmcode[ ++asmcode_n ],"\tMOV eax,[ebp+%d]",k);
                    sprintf(asmcode[ ++asmcode_n ],"\tPUSH eax");
                }
                //为自己的ebp申请空间
                sprintf(asmcode[ ++asmcode_n ],"\tSUB esp,4");
                //产生call语句
                sprintf(asmcode[ ++asmcode_n ],"\tCALL _%s",mlcode[i].x2.name);
                //调用者恢复堆栈
                sprintf(asmcode[ ++asmcode_n ],"\tADD esp,%d",(mlcode[i].x2.lvl)*4);//弹出display区
                sprintf(asmcode[ ++asmcode_n ],"\tMOV eax,[esp]");//此时[esp]为参数个数*4
                sprintf(asmcode[ ++asmcode_n ],"\tADD eax,4");
                sprintf(asmcode[ ++asmcode_n ],"\tADD esp,eax");
                break;
            }
            case ML_JMP:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s",";JMP");
                sprintf(asmcode[ ++asmcode_n ],"\t%s %s","JMP ",mlcode[i].x2.name);
                break;
            }
            case ML_LABEL:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s",";LABEL");
                sprintf(asmcode[ ++asmcode_n ],"%s:",mlcode[i].x2.name);
                break;
            }
            case ML_CMP:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s %s",";CMP",mlcode[i].x1.name,mlcode[i].x2.name);
                //生成cmp指令，她的两个参数均为临时变量

                getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"edx");
                sprintf(asmcode[ ++asmcode_n ],"\tCMP eax,edx");

                break;
            }
            case ML_JWNT:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s",";JWNT ",mlcode[i].x1.name);
                char name[8];
                strcpy(name,mlcode[i].x1.name);
                if( strcmp(name,">")==0 )
                    sprintf(asmcode[ ++asmcode_n ],"\t%s %s","JBE",mlcode[i].x2.name);
                else if( strcmp(name,">=")==0 )
                    sprintf(asmcode[ ++asmcode_n ],"\t%s %s","JB",mlcode[i].x2.name);
                else if( strcmp(name,"<")==0 )
                    sprintf(asmcode[ ++asmcode_n ],"\t%s %s","JAE",mlcode[i].x2.name);
                else if( strcmp(name,"<=")==0 )
                    sprintf(asmcode[ ++asmcode_n ],"\t%s %s","JA",mlcode[i].x2.name);
                else if( strcmp(name,"=")==0 )
                    sprintf(asmcode[ ++asmcode_n ],"\t%s %s","JNE",mlcode[i].x2.name);
                else if( strcmp(name,"<>")==0 )
                    sprintf(asmcode[ ++asmcode_n ],"\t%s %s","JE",mlcode[i].x2.name);
                break;
            }
            case ML_PLUS:
            case ML_MINUS:
            case ML_TIMES:
            case ML_SLASH:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %d",";CALC ",mlcode[i].type);

                //12.16此时的加减乘除运算的运算项目只可能是临时变量
                //12.17此时可以使任意变量
                getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"edx");
                if( mlcode[i].type == ML_PLUS )
                    sprintf(asmcode[ ++asmcode_n ],"\tADD eax,edx");
                else if( mlcode[i].type == ML_MINUS )
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,edx");
                else if( mlcode[i].type == ML_TIMES )
                    sprintf(asmcode[ ++asmcode_n ],"\tIMUL edx");
                else if( mlcode[i].type == ML_SLASH ){
                    //12.17
                    sprintf(asmcode[ ++asmcode_n ],"\tMOV ecx,edx");
                    sprintf(asmcode[ ++asmcode_n ],"\tIMUL ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tIDIV ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tIDIV ecx");
                }
                getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x3,"edx");
                sprintf(asmcode[ ++asmcode_n ],"\tMOV [edx],eax");
                break;
            }
            case ML_PUSH:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s",";PUSH",mlcode[i].x2.name);
                //null,null,null申请函数返回值的空间
                if( mlcode[i].x2.type==TYPE_NULL )
                {
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB esp,4");
                }
                //是常数--函数调用时压入参数的总数
                else if( mlcode[i].x1.type==TYPE_NULL && mlcode[i].x2.type==TYPE_CONST )
                {
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"eax");
                    sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH eax");
                }
                //调用函数时压参，且形参为非var
                else if( mlcode[i].x1.val == 2)
                {
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"eax");
                    sprintf(asmcode[ ++asmcode_n ],"\tPUSH eax");
                }
                //调用函数时压参，且形参为var
                else if( mlcode[i].x1.val == 1)
                {
					//12.29
					if( mlcode[i].x2.type == TYPE_VAR && mlcode[i].x2.kind>2 ){//若x2为数组
						getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x3,"ecx");
						getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"eax");
						sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
						sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
						sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
						sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
					}
					else 
						getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"eax");
                    sprintf(asmcode[ ++asmcode_n ],"\tPUSH eax");
                }
                else{//x1不为数组
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                }
                break;
            }
            case ML_POP:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s",";POP",mlcode[i].x2.name);

                //变量或临时变量或参数-函数返回值
                if( mlcode[i].x2.type != TYPE_CONST ){
                    sprintf(asmcode[ ++asmcode_n ],"\tPOP eax");
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"edx");
                    sprintf(asmcode[ ++asmcode_n ],"\tMOV [edx],eax");
                }
                //常量-恢复堆栈
                else if( mlcode[i].x2.type == TYPE_CONST ){
                    sprintf(asmcode[ ++asmcode_n ],"\tADD esp,%d",mlcode[i].x2.val);
                }

                break;
            }
            case ML_BEC:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s %s",";BEC",mlcode[i].x1.name,mlcode[i].x3.name);

                //X3:=X1 ,若x1或x3为数组，则x2是数组下标
                //先用eax记录X1的值
                //再用edx记录x3的地址
                //最后生成mov [edx],eax

                //1.用eax记录x1的值
                if( mlcode[i].x1.type == TYPE_VAR && mlcode[i].x1.kind>2 ){//若x1为数组
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"ecx");
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tMOV eax,[eax]");
                }
                else{//x1不为数组
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                }

                //2.再用edx记录x3的地址
                if( mlcode[i].x3.type == TYPE_VAR && mlcode[i].x3.kind>2 ){//若x3为数组
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"ecx");
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x3,"edx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                }
                else{//x3不为数组
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x3,"edx");
                }

                //3.最后生成mov [edx],eax
                sprintf(asmcode[ ++asmcode_n ],"\tMOV [edx],eax");
                break;
            }
            case ML_READ:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s",";READ",mlcode[i].x2.name);
				
                getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"edx");
                //sprintf(asmcode[ ++asmcode_n ],"\tReadSDecDword [edx]");
				//12.29
				if(mlcode[i].x2.kind==1)
						sprintf(asmcode[ ++asmcode_n ],"\tReadSDecDword [edx]");
				else if(mlcode[i].x2.kind==2)
						sprintf(asmcode[ ++asmcode_n ],"\tReadChar [edx]");
                break;
            }
            case ML_WRITE:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s",";WRITE");

                if(mlcode[i].x2.type!=TYPE_NULL){
                    asmcodedata_n++;
                    sprintf(asmcodedata[ asmcodedata_n ],"\t_mstr%d byte \"%s\",0",asmcodedata_n,mlcode[i].x2.name);
                    sprintf(asmcode[ ++asmcode_n ],"\tWriteString _mstr%d",asmcodedata_n);
                }
                if(mlcode[i].x3.type!=TYPE_NULL){
					if(mlcode[i].x3.type==TYPE_CONST){
						asmcodedata_n++;
						sprintf(asmcodedata[ asmcodedata_n ],"\t_mstr%d byte \"%s\",0",asmcodedata_n,mlcode[i].x3.name);
						sprintf(asmcode[ ++asmcode_n ],"\tWriteString _mstr%d",asmcodedata_n);
					}else {
						getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x3,"edx");
						if(mlcode[i].x1.val==0)
							sprintf(asmcode[ ++asmcode_n ],"\tWriteChar [edx]");
						else
							sprintf(asmcode[ ++asmcode_n ],"\tWriteSDecDword [edx]");
					}
                }
                break;
            }
			case ML_NULL:
			{
				memset(&mlcode[i].x1,0,sizeof(TABLE));
				memset(&mlcode[i].x2,0,sizeof(TABLE));
				memset(&mlcode[i].x3,0,sizeof(TABLE));
				break;
			}
			case ML_REG:
			{
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s %s",";REG",mlcode[i].x1.name,mlcode[i].x3.name);
				getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,mlcode[i].x3.name);
				break;
			}
        }
    }
    //停留
}





/******************************代码优化*********************************/
//*****************************DAG图消除公共子表达式********************
//修改中间代码
void mlalt(int mlcode_k,MLCODE *mlcode,ML_TYPE type,TABLE x1,TABLE x2,TABLE x3){
    mlcode[mlcode_k].type = type;
    table_copy(mlcode[mlcode_k].x1,x1);
    table_copy(mlcode[mlcode_k].x2,x2);
    table_copy(mlcode[mlcode_k].x3,x3);
}
//在DAG图中寻找中间节点
int findPos1(ML_TYPE c,int x,int y,int &n1,NODE1 list1[],int &n2,NODE2 list2[]){
	int i;
	for(i=1;i<=n1;i++)
		if(list1[i].c==c && 
			(list1[i].lc==list2[x].p && list1[i].rc==list2[y].p ||
			list1[i].lc==list2[y].p && list1[i].rc==list2[x].p))
			return i;
	return -1;
}
//在节点表中寻找t
int findPos2(TABLE t,int &n1,NODE1 list1[],int &n2,NODE2 list2[],int f){
	int i;
	for(i=1;i<=n2;i++)
		if(t.type==list2[i].t.type
			&&t.kind==list2[i].t.kind
			&&t.val==list2[i].t.val
			&&t.adr==list2[i].t.adr)
			return i;
	if(f==0){//需要在DAG图中新建节点
		table_copy(list1[++n1].t,t);
		table_copy(list2[++n2].t,t);
		list2[n2].p=n1;
	}else{//不在DAG图中新建节点
		table_copy(list2[++n2].t,t);
	}
	return n2;
}
//对每个基本块处理
void opt_dag_each(MLCODE *mlcode,int begin,int end){
	int n1=0;				//节点数
	int n2=0;				//节点表项数
	NODE1 list1[MAX_DAG];	//DAG的节点
	NODE2 list2[MAX_DAG];	//节点表
	int i,j,jj;
	int x,y,z,k;						//x,y,z为节点表中位置  k为DAG图中位置

	memset(list1,0,sizeof(NODE1)*MAX_DAG);
	memset(list2,0,sizeof(NODE2)*MAX_DAG);
	if(mlcode[begin-1].type==ML_POP)begin++;
	
	//是否有数组
	for(j=begin;j<=end;j++){
		if( mlcode[j].x1.type == TYPE_VAR && mlcode[j].x1.kind>2 )
			return;
		if( mlcode[j].x3.type == TYPE_VAR && mlcode[j].x3.kind>2 )
			return;
	}

	if(begin==end)
		return;
	//生成DAG图
	for(i=begin;i<end;i++){
		if(mlcode[i].type==ML_BEC){		// 赋值语句
			x=findPos2(mlcode[i].x1,n1,list1,n2,list2,0);
			z=findPos2(mlcode[i].x3,n1,list1,n2,list2,1);
			list2[z].p=list2[x].p;
		}
		else{							//运算语句
			x=findPos2(mlcode[i].x1,n1,list1,n2,list2,0);
			y=findPos2(mlcode[i].x2,n1,list1,n2,list2,0);
			k=findPos1(mlcode[i].type,x,y,n1,list1,n2,list2);
			if(k==-1){
				n1++;
				k=n1;
				list1[n1].c=mlcode[i].type;
				list1[k].lc=list2[x].p;
				list1[k].rc=list2[y].p;
				list1[ list2[x].p ].p[0]++;
				list1[ list2[x].p ].p[ list1[ list2[x].p ].p[0] ]=k;
				list1[ list2[y].p ].p[0]++;
				list1[ list2[y].p ].p[ list1[ list2[y].p ].p[0] ]=k;
			}
			z=findPos2(mlcode[i].x3,n1,list1,n2,list2,1);
			list2[z].p=k;
		}
	}

	//从DAG导出启发式中间代码计算顺序
	int listgen[MAX_DAG];				//中间代码序列
	int listgenb[MAX_DAG]={0};			//list1中节点是否加入了中间代码序列
	int p;
	int f;								//是否所有父节点都已经加入中间代码序列
	i=0;
	for(j=1;j<=n1;j++){
		if(list1[j].p[0]==0 && listgenb[j]==0 ){//未加入队列的没有父节点的节点
			listgenb[j]=1;
			listgen[i++]=j;
			p=list1[j].lc;
			while(p!=0){
					f=1;
					for(jj=1;jj<=list1[p].p[0];jj++)
						if(listgenb[ list1[p].p[jj] ]==0){
							f=0;
							break;
						}
					if(f==0)break;
					listgenb[p]=1;
					listgen[i++]=p;
					p=list1[p].lc;
			}
		}
	}
	while(i<n1){
		for(j=1;j<=n1;j++)
			if(listgenb[j]==0){	//未加入队列
				f=1;
				for(jj=1;jj<=list1[j].p[0];jj++)
					if(listgenb[ list1[j].p[jj] ]==0){
						f=0;
						break;
					}
				if(f==0)continue;
				listgenb[j]=1;
				listgen[i++]=j;
				p=list1[j].lc;
				while(p!=0){
					f=1;
					for(jj=1;jj<=list1[p].p[0];jj++)
						if(listgenb[ list1[p].p[jj] ]==0){
							f=0;
							break;
						}
					if(f==0)break;
					listgenb[p]=1;
					listgen[i++]=p;
					p=list1[p].lc;
				}
			}
	}
	//根据中间代码计算顺序生成新的中间代码
	int mlcode_i=begin;				//中间代码号
	int tmpvar_i=0;					//临时变量号
	int istmpvar;					//需要给临时变量赋值
	int tmp=0;						//处理赋值语句
	int list1_n1,list1_n2,list1_n3;

	int iforpush=0;

	TABLE table_n1,table_n2,table_n3,table_null;
	memset(&table_null,0,sizeof(TABLE));
	table_null.type=TYPE_NULL;

	for(i=n1-1;i>=0;i--){								//处理叶节点(1)值的转移
		k=listgen[i];
		if(list1[k].lc==0 && list1[k].rc==0){
			//!!!!!!!!叶节点的值发生的转移!!!!!!!!!!//
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
			x=findPos2(list1[k].t,n1,list1,n2,list2,1);
			if(list2[x].p!=k){
				tabletmpval(++tmpvar_i,table_n3);
				mlalt(mlcode_i++,mlcode,ML_BEC,list1[k].t,table_null,table_n3);
				table_copy(list1[k].t,table_n3);
			}
			//!!!!!!!!叶节点的值发生的转移!!!!!!!!!!//
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
		}
	}
	for(i=n1-1;i>=0;i--){								//处理叶节点(2)赋值
		k=listgen[i];
		if(list1[k].lc==0 && list1[k].rc==0){		
			istmpvar=1;
			for(j=1;j<=n2;j++){							//是否要赋值给临时变量
				if( list2[j].p==k && list2[j].t.type!=TYPE_TMPVAR )
					istmpvar=0;
			}
			if(istmpvar){
				table_copy(table_n1,list1[k].t);
				tabletmpval(++tmpvar_i,table_n3);
				mlalt(mlcode_i++,mlcode,ML_BEC,table_n1,table_null,table_n3);
			}
			else{
				for(j=1;j<=n2;j++){
					if( list2[j].p==k && list2[j].t.type!=TYPE_TMPVAR && list2[j].t.type!=TYPE_CONST){
						table_copy(table_n1,list1[k].t);
						table_copy(table_n3,list2[j].t);
						mlalt(mlcode_i++,mlcode,ML_BEC,table_n1,table_null,table_n3);
					}
				}
			}
		}
	}

	for(i=n1-1;i>=0;i--){			//给每个中间节点重新生成中间代码--运算+赋值
		if(list1[listgen[i]].lc==0 && list1[listgen[i]].rc==0)continue;
		tmp=0;					
		list1_n3=listgen[i];
		list1_n1=list1[list1_n3].lc;
		list1_n2=list1[list1_n3].rc;

		table_copy(table_n1,list1[list1_n1].t);
		table_copy(table_n2,list1[list1_n2].t);
		istmpvar=1;
		for(j=1;j<=n2;j++){									//是否要赋值给临时变量
			if( list2[j].p==list1_n3 && list2[j].t.type!=TYPE_TMPVAR )
				istmpvar=0;
		}
		if(istmpvar){										//赋值给临时变量
			tabletmpval(++tmpvar_i,table_n3);
			mlalt(mlcode_i++,mlcode,list1[list1_n3].c,table_n1,table_n2,table_n3);
			table_copy(list1[list1_n3].t,table_n3);
		}
		else{												//赋值所有的变量
			for(j=1;j<=n2;j++){
				if( list2[j].p==list1_n3 && list2[j].t.type!=TYPE_TMPVAR){
					if(tmp==0){
						table_copy(table_n3,list2[j].t);
						mlalt(mlcode_i++,mlcode,list1[list1_n3].c,table_n1,table_n2,table_n3);
						table_copy(list1[list1_n3].t,table_n3);
						tmp=j;
					}else{
						table_copy(table_n1,list2[tmp].t);
						table_copy(table_n2,table_null);
						table_copy(table_n3,list2[j].t);
						mlalt(mlcode_i++,mlcode,ML_BEC,table_n1,table_n2,table_n3);
					}
				}
			}
		}
		
	}
	//1.1
	//重新给mlcode[end]中的参数赋值
	if(mlcode[end].type==ML_PUSH && mlcode[end].x2.type==TYPE_TMPVAR){	//PUSH 临时变量
		y=findPos2(mlcode[end].x2,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x2,list1[y].t);
	}
	else if(mlcode[end].type==ML_WRITE && mlcode[end].x3.type==TYPE_TMPVAR){	//WRITE临时变量
		y=findPos2(mlcode[end].x3,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x3,list1[y].t);
	}
	else if(mlcode[end].type==ML_CMP && mlcode[end].x1.type==TYPE_TMPVAR){		//CMP临时变量
		y=findPos2(mlcode[end].x1,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x1,list1[y].t);
	}
	else if(mlcode[end].type==ML_CMP && mlcode[end].x2.type==TYPE_TMPVAR){		//CMP临时变量
		y=findPos2(mlcode[end].x2,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x2,list1[y].t);
	}

	for(;mlcode_i<end;mlcode_i++){							//给空下的中间代码赋ML_NULL
		mlalt(mlcode_i,mlcode,ML_NULL,table_null,table_null,table_null);
	}

}

void opt_dag(int mlcode_n,MLCODE *mlcode){
	int n=0;					//基本块数
	int list[MAX_DAG];		//基本块
	int i;

	memset(list,0,sizeof(int)*MAX_DAG);

	//划分基本块
	for(i=1;i<=mlcode_n;i++){
		if(!( mlcode[i].type==ML_PLUS||mlcode[i].type==ML_MINUS||mlcode[i].type==ML_TIMES||mlcode[i].type==ML_SLASH||mlcode[i].type==ML_BEC ))
			list[n++]=i;
	}
	//构建DAG
	for(i=0;i<n-1;i++){
		opt_dag_each(mlcode,list[i]+1,list[i+1]);
	}
}


/**************************************************/
//*****************************常数合并********************
void opt_constunion_each(MLCODE *mlcode,int begin,int end){
	int i,j;
	int x,y;
	TABLE table_tmp1,table_tmp2,table_null;
	memset(&table_null,0,sizeof(TABLE));
	table_null.type=TYPE_NULL;
	for(i=begin;i<end;i++){
		if(mlcode[i].type<4 && mlcode[i].x1.type==TYPE_CONST && mlcode[i].x2.type==TYPE_CONST && mlcode[i].x3.type==TYPE_TMPVAR){
			x=mlcode[i].x1.val;
			y=mlcode[i].x2.val;
			if(mlcode[i].type==ML_PLUS)
				tableconst(x+y,table_tmp1);
			else if(mlcode[i].type==ML_MINUS)
				tableconst(x-y,table_tmp1);
			else if(mlcode[i].type==ML_TIMES)
				tableconst(x*y,table_tmp1);
			else if(mlcode[i].type==ML_SLASH)
				tableconst(x/y,table_tmp1);
			table_copy(table_tmp2,mlcode[i].x3);
			mlalt(i,mlcode,ML_NULL,table_null,table_null,table_null);
			for(j=i+1;j<end;j++){
				if(table_equal(table_tmp2,mlcode[j].x3))break;
				if(table_equal(table_tmp2,mlcode[j].x1))
					table_copy(mlcode[j].x1,table_tmp1);
				if(table_equal(table_tmp2,mlcode[j].x2))
					table_copy(mlcode[j].x2,table_tmp1);
			}
		}
	}
}
void opt_constunion(int mlcode_n,MLCODE *mlcode){
	int n=0;					//基本块数
	int list[MAX_DAG];		//基本块
	int i;

	memset(list,0,sizeof(int)*MAX_DAG);

	//划分基本块
	for(i=1;i<=mlcode_n;i++){
		if(!( mlcode[i].type==ML_PLUS||mlcode[i].type==ML_MINUS||mlcode[i].type==ML_TIMES||mlcode[i].type==ML_SLASH||mlcode[i].type==ML_BEC ))
			list[n++]=i;
	}
	//常数合并
	for(i=0;i<n-1;i++){
		opt_constunion_each(mlcode,list[i]+1,list[i+1]);
	}
}


/**************************************************/
//*****************************全局寄存器分配EBX********************
void opt_regisiter_each(int &mlcode_n,MLCODE *mlcode,int begin ,int end){
	TABLE	list1[128];
	TABLE	table_reg;
	int		list2[128]={0};
	int		n=0;
	int		i,j;
	int		max=0;
	for(i=begin+1;i<end;i++){
		if(mlcode[i].type==ML_PLUS || mlcode[i].type==ML_MINUS ||mlcode[i].type==ML_TIMES
				||mlcode[i].type==ML_SLASH ||mlcode[i].type==ML_BEC&&mlcode[i].x1.kind<3
				&&mlcode[i].x3.kind<3)
		{
			if(mlcode[i].x1.kind==TYPE_VAR){
				for(j=0;j<n;j++)
					if( table_equal(list1[j],mlcode[i].x1 )){
						list2[j]++;
						break;
					}
				if(j==n){
					table_copy(list1[n],mlcode[i].x1);
					list2[n]=1;
					n++;
				}
			}
			if(mlcode[i].x2.kind==TYPE_VAR){
				for(j=0;j<n;j++)
					if( table_equal(list1[j],mlcode[i].x2 )){
						list2[j]++;
						break;
					}
				if(j==n){
					table_copy(list1[n],mlcode[i].x2);
					list2[n]=1;
					n++;
				}
			}
			if(mlcode[i].x3.kind==TYPE_VAR){
				for(j=0;j<n;j++)
					if( table_equal(list1[j],mlcode[i].x3 )){
						list2[j]++;
						break;
					}
				if(j==n){
					table_copy(list1[n],mlcode[i].x3);
					list2[n]=1;
					n++;
				}
			}
		}
	}
	for(i=0;i<n;i++)
		if(list2[i]>max){
			max=list2[i];
			j=i;
		}
	//为list1[j]分配EBX
	mlcode_n++;
	for(i=mlcode_n;i>begin+1;i--){
		mlcode[i].type=mlcode[i-1].type;
		table_copy(mlcode[i].x1,mlcode[i-1].x1);
		table_copy(mlcode[i].x2,mlcode[i-1].x2);
		table_copy(mlcode[i].x3,mlcode[i-1].x3);
	}
	end++;
	table_reg.type=TYPE_REG;
	strcpy(table_reg.name,"EBX");
	mlcode[begin+1].type=ML_REG;
	table_copy(mlcode[begin+1].x1,list1[j]);
	table_copy(mlcode[begin+1].x3,table_reg);

	for(i=begin+2;i<end;i++){
		if( table_equal(list1[j],mlcode[i].x1 ) )
			table_copy(mlcode[i].x1,list1[j]);
		if( table_equal(list1[j],mlcode[i].x2 ) )
			table_copy(mlcode[i].x2,list1[j]);
		if( table_equal(list1[j],mlcode[i].x3 ) )
			table_copy(mlcode[i].x3,list1[j]);
	}

}
void opt_regisiter(int &mlcode_n,MLCODE *mlcode){
	int n=0;					//函数块数
	int list[MAX_DAG];		//函数块
	int i,j;

	memset(list,0,sizeof(int)*MAX_DAG);

	//划分函数块
	for(i=1;i<=mlcode_n;i++){
		if( mlcode[i].type==ML_BEGIN || mlcode[i].type==ML_END )
			list[n++]=i;
	}
	for(i=0;i<n;i=i+2){
		opt_regisiter_each(mlcode_n,mlcode,list[i],list[i+1]);
		for(j=i+2;j<n;j++)
			list[j]++;
	}
}
/**************************************************/
//*****************************窥孔优化********************
void opt_peephole_each(MLCODE *mlcode,int begin,int end){
	int i;
	TABLE table_null;
	memset(&table_null,0,sizeof(TABLE));
	table_null.type=TYPE_NULL;
	for(i=begin+1;i<end-1;i++){
		if(mlcode[i].type==ML_BEC &&
			mlcode[i+1].type==ML_BEC &&
			table_equal(mlcode[i].x1,mlcode[i+1].x3) &&
			table_equal(mlcode[i].x3,mlcode[i+1].x1) )
			mlalt(i+1,mlcode,ML_NULL,table_null,table_null,table_null);
		else if(mlcode[i].type==ML_JMP &&
			mlcode[i+1].type==ML_LABEL &&
			strcmp(mlcode[i].x2.name,mlcode[i+1].x2.name)==0 )
			mlalt(i,mlcode,ML_NULL,table_null,table_null,table_null);
	}
}
void opt_peephole(int &mlcode_n,MLCODE *mlcode){
	int n=0;					//基本块数
	int list[MAX_DAG];		//基本块
	int i;

	memset(list,0,sizeof(int)*MAX_DAG);

	//划分基本块
	for(i=1;i<=mlcode_n;i++){
		if(!( mlcode[i].type==ML_PLUS||mlcode[i].type==ML_MINUS||mlcode[i].type==ML_TIMES||mlcode[i].type==ML_SLASH||mlcode[i].type==ML_BEC ))
			list[n++]=i;
	}
	//常数合并
	for(i=0;i<n-1;i++){
		opt_peephole_each(mlcode,list[i]+1,list[i+1]);
	}
}


