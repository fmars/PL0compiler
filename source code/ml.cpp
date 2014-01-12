#include"header.h"
#include<string.h>
//����һ��TABLE��
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

//������Ԫʽ����
void mlgen(int &mlcode_n,MLCODE *mlcode,ML_TYPE type,TABLE x1,TABLE x2,TABLE x3){
	mlcode_n++;
    mlcode[mlcode_n].type = type;
    table_copy(mlcode[mlcode_n].x1,x1);
    table_copy(mlcode[mlcode_n].x2,x2);
    table_copy(mlcode[mlcode_n].x3,x3);
}
//������ʱ������ӦTABLE
void tabletmpval(int x,TABLE&t){
    char tmp[MAX_SYMLEN];
    memset(&t,0,sizeof(TABLE));
	t.type=TYPE_TMPVAR;
	t.adr=x;
	strcpy(t.name,"!t");
	itoa(x,tmp,10);
	strcat(t.name,tmp);
}
//���ɳ�����Ӧ��TABLE
void tableconst(int x,TABLE&t){
    char tmp[MAX_SYMLEN];
    memset(&t,0,sizeof(TABLE));
    t.type=TYPE_CONST;
    t.val=x;
    itoa(x,tmp,10);
    strcpy(t.name,tmp);
}
//���ɱ�Ŷ�Ӧ��TABLE
void tablelabel(int x,TABLE&t){
    char tmp[MAX_SYMLEN];
    memset(&t,0,sizeof(TABLE));
	t.type=TYPE_LABEL;
	t.adr=x;
	strcpy(t.name,"@label");
	itoa(x,tmp,10);
	strcat(t.name,tmp);
}

/*Ѱַ*/
void getaddress(int &asmcode_n,char asmcode[MAX_ASM][MAX_ASMLEN],
    int lvl_now,//��ǰ���
    int locvar_n,//�ֲ���������
    int para_n,//��������
    int tmpvar_n,//��ʱ��������
    TABLE t,
    char r[4]){
	//�Ĵ���
	if( t.type == TYPE_REG){
		sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,%s",r,t.name);
	}
    //��ʱ����
    else if( t.type == TYPE_TMPVAR ){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,ebp",r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,locvar_n*4+t.adr*4);
    }
    //�ֲ�������������������
    else if( t.type == TYPE_VAR &&(t.val==0 || t.kind==3||t.kind==4 )){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*t.adr);
    }
    //var����
    else if( t.type == TYPE_VAR && t.val==1){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=��ָ��
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
    //��var����
    else if( t.type == TYPE_VAR && t.val==2){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=��ָ��
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
    }
    //����
    else if( t.type == TYPE_FUNCTION){//����
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl));//edx=��ָ��
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,4",r);
    }
    }
/*Ѱֵ*/
void getvalue(int &asmcode_n,char asmcode[MAX_ASM][MAX_ASMLEN],
    int lvl_now,//��ǰ���
    int locvar_n,//�ֲ���������
    int para_n,//��������
    int tmpvar_n,//��ʱ��������
    TABLE t,
    char r[4])
{
	//�Ĵ���
	if( t.type == TYPE_REG){
		sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,t.name);
	}
    //����
    else if( t.type == TYPE_CONST ){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,%d",r,t.val);
    }
    //��ʱ����
    if( t.type == TYPE_TMPVAR ){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp-%d]",r,locvar_n*4+t.adr*4);
    }
    //�ֲ�������������������
    else if( t.type == TYPE_VAR &&(t.val==0 || t.kind==3||t.kind==4 )){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s-%d]",r,r,4*t.adr);
    }
    //var����
    else if( t.type == TYPE_VAR && t.val==1){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=��ָ��
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
    //��var����
    else if( t.type == TYPE_VAR && t.val==2){
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl+1));//edx=��ָ��
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl-1));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tSUB %s,%d",r,4*(t.adr-1));
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
    //����
    else if( t.type == TYPE_FUNCTION){//����
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[ebp+%d]",r,8+4*(lvl_now-t.lvl));//edx=��ָ��
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,%d",r,8+4*(t.lvl));
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,[%s]",r,r);
        sprintf(asmcode[ ++asmcode_n ],"\tADD %s,4",r);
        sprintf(asmcode[ ++asmcode_n ],"\tMOV %s,[%s]",r,r);
    }
}




//��Ԫʽ����������
void asmgen(int mlcode_n,MLCODE *mlcode,int &asmcode_n,char asmcode[MAX_ASM][MAX_ASMLEN],
    int &asmcodehead_n,char asmcodehead[MAX_ASMHEAD][MAX_ASMHEADLEN],
    int &asmcodedata_n,char asmcodedata[MAX_ASMDATA][MAX_ASMDATALEN]){//��Ԫʽ�������
    //������ͷ
    strcpy( asmcodehead[0],"end _main");//���������ļ�ĩβ
    strcpy( asmcodehead[++asmcodehead_n],"	.386");
    strcpy( asmcodehead[++asmcodehead_n],"	.model flat, stdcall  ; 32 bit memory model");
    strcpy( asmcodehead[++asmcodehead_n],"	option casemap :none  ; case sensitive");
    strcpy( asmcodehead[++asmcodehead_n],"	include \\masm32\\include\\io32.inc ");
    //������.data��
    strcpy( asmcodedata[0],".data");
    //������
    //��Ԫʽָ��
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
    int lvl_now;//��ǰ���
    int locvar_n;//�ֲ���������
    int para_n;//��������
    int tmpvar_n;//��ʱ��������

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

                //����ͷ
                sprintf(asmcode[ ++asmcode_n ],"_%s %s",mlcode[i].x2.name,"PROC");
                //������ǰջ��ָ�룬��ָ��ǰջ��
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH ebp");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","MOV ebp,esp");
                //�����Լ���ebp
                sprintf(asmcode[ ++asmcode_n ],"\t%s","MOV [esp+8],ebp");
                //���ֲ���������ʱ�����ڵط�
                sprintf(asmcode[ ++asmcode_n ],"\t%s%d","SUB esp,",(locvar_n+tmpvar_n)*4);
                //����Ĵ�����
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH ebx");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH esi");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH edi");
                break;
            }
            case ML_END:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s",";END");
                //��ԭ�Ĵ�����
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP edi");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP esi");
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP ebx");
                //��ԭ��ջ
                sprintf(asmcode[ ++asmcode_n ],"\t%s%d","ADD esp,",(locvar_n+tmpvar_n)*4);
                //��ԭ��ǰջ��ָ��
                sprintf(asmcode[ ++asmcode_n ],"\t%s","POP ebp");
                //�������
                sprintf(asmcode[ ++asmcode_n ],"\t%s","RET");
                //����β
                sprintf(asmcode[ ++asmcode_n ],"_%s %s",mlcode[i].x2.name,"ENDP");
                break;
            }
            case ML_CALL:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s",";CALL",mlcode[i].x2.name);
                //����display��
                for( j=1;j<=mlcode[i].x2.lvl-1;j++ ){
                    k = 8+4*lvl_now-j*4;
                    sprintf(asmcode[ ++asmcode_n ],"\tMOV eax,[ebp+%d]",k);
                    sprintf(asmcode[ ++asmcode_n ],"\tPUSH eax");
                }
                //Ϊ�Լ���ebp����ռ�
                sprintf(asmcode[ ++asmcode_n ],"\tSUB esp,4");
                //����call���
                sprintf(asmcode[ ++asmcode_n ],"\tCALL _%s",mlcode[i].x2.name);
                //�����߻ָ���ջ
                sprintf(asmcode[ ++asmcode_n ],"\tADD esp,%d",(mlcode[i].x2.lvl)*4);//����display��
                sprintf(asmcode[ ++asmcode_n ],"\tMOV eax,[esp]");//��ʱ[esp]Ϊ��������*4
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
                //����cmpָ���������������Ϊ��ʱ����

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

                //12.16��ʱ�ļӼ��˳������������Ŀֻ��������ʱ����
                //12.17��ʱ����ʹ�������
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
                //null,null,null���뺯������ֵ�Ŀռ�
                if( mlcode[i].x2.type==TYPE_NULL )
                {
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB esp,4");
                }
                //�ǳ���--��������ʱѹ�����������
                else if( mlcode[i].x1.type==TYPE_NULL && mlcode[i].x2.type==TYPE_CONST )
                {
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"eax");
                    sprintf(asmcode[ ++asmcode_n ],"\t%s","PUSH eax");
                }
                //���ú���ʱѹ�Σ����β�Ϊ��var
                else if( mlcode[i].x1.val == 2)
                {
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"eax");
                    sprintf(asmcode[ ++asmcode_n ],"\tPUSH eax");
                }
                //���ú���ʱѹ�Σ����β�Ϊvar
                else if( mlcode[i].x1.val == 1)
                {
					//12.29
					if( mlcode[i].x2.type == TYPE_VAR && mlcode[i].x2.kind>2 ){//��x2Ϊ����
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
                else{//x1��Ϊ����
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                }
                break;
            }
            case ML_POP:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s",";POP",mlcode[i].x2.name);

                //��������ʱ���������-��������ֵ
                if( mlcode[i].x2.type != TYPE_CONST ){
                    sprintf(asmcode[ ++asmcode_n ],"\tPOP eax");
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"edx");
                    sprintf(asmcode[ ++asmcode_n ],"\tMOV [edx],eax");
                }
                //����-�ָ���ջ
                else if( mlcode[i].x2.type == TYPE_CONST ){
                    sprintf(asmcode[ ++asmcode_n ],"\tADD esp,%d",mlcode[i].x2.val);
                }

                break;
            }
            case ML_BEC:
            {
                sprintf(asmcode[ ++asmcode_n ],"\n%s %s %s",";BEC",mlcode[i].x1.name,mlcode[i].x3.name);

                //X3:=X1 ,��x1��x3Ϊ���飬��x2�������±�
                //����eax��¼X1��ֵ
                //����edx��¼x3�ĵ�ַ
                //�������mov [edx],eax

                //1.��eax��¼x1��ֵ
                if( mlcode[i].x1.type == TYPE_VAR && mlcode[i].x1.kind>2 ){//��x1Ϊ����
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"ecx");
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB eax,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tMOV eax,[eax]");
                }
                else{//x1��Ϊ����
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x1,"eax");
                }

                //2.����edx��¼x3�ĵ�ַ
                if( mlcode[i].x3.type == TYPE_VAR && mlcode[i].x3.kind>2 ){//��x3Ϊ����
                    getvalue(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x2,"ecx");
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x3,"edx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                    sprintf(asmcode[ ++asmcode_n ],"\tSUB edx,ecx");
                }
                else{//x3��Ϊ����
                    getaddress(asmcode_n,asmcode,lvl_now,locvar_n,para_n,tmpvar_n,mlcode[i].x3,"edx");
                }

                //3.�������mov [edx],eax
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
    //ͣ��
}





/******************************�����Ż�*********************************/
//*****************************DAGͼ���������ӱ��ʽ********************
//�޸��м����
void mlalt(int mlcode_k,MLCODE *mlcode,ML_TYPE type,TABLE x1,TABLE x2,TABLE x3){
    mlcode[mlcode_k].type = type;
    table_copy(mlcode[mlcode_k].x1,x1);
    table_copy(mlcode[mlcode_k].x2,x2);
    table_copy(mlcode[mlcode_k].x3,x3);
}
//��DAGͼ��Ѱ���м�ڵ�
int findPos1(ML_TYPE c,int x,int y,int &n1,NODE1 list1[],int &n2,NODE2 list2[]){
	int i;
	for(i=1;i<=n1;i++)
		if(list1[i].c==c && 
			(list1[i].lc==list2[x].p && list1[i].rc==list2[y].p ||
			list1[i].lc==list2[y].p && list1[i].rc==list2[x].p))
			return i;
	return -1;
}
//�ڽڵ����Ѱ��t
int findPos2(TABLE t,int &n1,NODE1 list1[],int &n2,NODE2 list2[],int f){
	int i;
	for(i=1;i<=n2;i++)
		if(t.type==list2[i].t.type
			&&t.kind==list2[i].t.kind
			&&t.val==list2[i].t.val
			&&t.adr==list2[i].t.adr)
			return i;
	if(f==0){//��Ҫ��DAGͼ���½��ڵ�
		table_copy(list1[++n1].t,t);
		table_copy(list2[++n2].t,t);
		list2[n2].p=n1;
	}else{//����DAGͼ���½��ڵ�
		table_copy(list2[++n2].t,t);
	}
	return n2;
}
//��ÿ�������鴦��
void opt_dag_each(MLCODE *mlcode,int begin,int end){
	int n1=0;				//�ڵ���
	int n2=0;				//�ڵ������
	NODE1 list1[MAX_DAG];	//DAG�Ľڵ�
	NODE2 list2[MAX_DAG];	//�ڵ��
	int i,j,jj;
	int x,y,z,k;						//x,y,zΪ�ڵ����λ��  kΪDAGͼ��λ��

	memset(list1,0,sizeof(NODE1)*MAX_DAG);
	memset(list2,0,sizeof(NODE2)*MAX_DAG);
	if(mlcode[begin-1].type==ML_POP)begin++;
	
	//�Ƿ�������
	for(j=begin;j<=end;j++){
		if( mlcode[j].x1.type == TYPE_VAR && mlcode[j].x1.kind>2 )
			return;
		if( mlcode[j].x3.type == TYPE_VAR && mlcode[j].x3.kind>2 )
			return;
	}

	if(begin==end)
		return;
	//����DAGͼ
	for(i=begin;i<end;i++){
		if(mlcode[i].type==ML_BEC){		// ��ֵ���
			x=findPos2(mlcode[i].x1,n1,list1,n2,list2,0);
			z=findPos2(mlcode[i].x3,n1,list1,n2,list2,1);
			list2[z].p=list2[x].p;
		}
		else{							//�������
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

	//��DAG��������ʽ�м�������˳��
	int listgen[MAX_DAG];				//�м��������
	int listgenb[MAX_DAG]={0};			//list1�нڵ��Ƿ�������м��������
	int p;
	int f;								//�Ƿ����и��ڵ㶼�Ѿ������м��������
	i=0;
	for(j=1;j<=n1;j++){
		if(list1[j].p[0]==0 && listgenb[j]==0 ){//δ������е�û�и��ڵ�Ľڵ�
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
			if(listgenb[j]==0){	//δ�������
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
	//�����м�������˳�������µ��м����
	int mlcode_i=begin;				//�м�����
	int tmpvar_i=0;					//��ʱ������
	int istmpvar;					//��Ҫ����ʱ������ֵ
	int tmp=0;						//����ֵ���
	int list1_n1,list1_n2,list1_n3;

	int iforpush=0;

	TABLE table_n1,table_n2,table_n3,table_null;
	memset(&table_null,0,sizeof(TABLE));
	table_null.type=TYPE_NULL;

	for(i=n1-1;i>=0;i--){								//����Ҷ�ڵ�(1)ֵ��ת��
		k=listgen[i];
		if(list1[k].lc==0 && list1[k].rc==0){
			//!!!!!!!!Ҷ�ڵ��ֵ������ת��!!!!!!!!!!//
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
			x=findPos2(list1[k].t,n1,list1,n2,list2,1);
			if(list2[x].p!=k){
				tabletmpval(++tmpvar_i,table_n3);
				mlalt(mlcode_i++,mlcode,ML_BEC,list1[k].t,table_null,table_n3);
				table_copy(list1[k].t,table_n3);
			}
			//!!!!!!!!Ҷ�ڵ��ֵ������ת��!!!!!!!!!!//
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
		}
	}
	for(i=n1-1;i>=0;i--){								//����Ҷ�ڵ�(2)��ֵ
		k=listgen[i];
		if(list1[k].lc==0 && list1[k].rc==0){		
			istmpvar=1;
			for(j=1;j<=n2;j++){							//�Ƿ�Ҫ��ֵ����ʱ����
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

	for(i=n1-1;i>=0;i--){			//��ÿ���м�ڵ����������м����--����+��ֵ
		if(list1[listgen[i]].lc==0 && list1[listgen[i]].rc==0)continue;
		tmp=0;					
		list1_n3=listgen[i];
		list1_n1=list1[list1_n3].lc;
		list1_n2=list1[list1_n3].rc;

		table_copy(table_n1,list1[list1_n1].t);
		table_copy(table_n2,list1[list1_n2].t);
		istmpvar=1;
		for(j=1;j<=n2;j++){									//�Ƿ�Ҫ��ֵ����ʱ����
			if( list2[j].p==list1_n3 && list2[j].t.type!=TYPE_TMPVAR )
				istmpvar=0;
		}
		if(istmpvar){										//��ֵ����ʱ����
			tabletmpval(++tmpvar_i,table_n3);
			mlalt(mlcode_i++,mlcode,list1[list1_n3].c,table_n1,table_n2,table_n3);
			table_copy(list1[list1_n3].t,table_n3);
		}
		else{												//��ֵ���еı���
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
	//���¸�mlcode[end]�еĲ�����ֵ
	if(mlcode[end].type==ML_PUSH && mlcode[end].x2.type==TYPE_TMPVAR){	//PUSH ��ʱ����
		y=findPos2(mlcode[end].x2,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x2,list1[y].t);
	}
	else if(mlcode[end].type==ML_WRITE && mlcode[end].x3.type==TYPE_TMPVAR){	//WRITE��ʱ����
		y=findPos2(mlcode[end].x3,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x3,list1[y].t);
	}
	else if(mlcode[end].type==ML_CMP && mlcode[end].x1.type==TYPE_TMPVAR){		//CMP��ʱ����
		y=findPos2(mlcode[end].x1,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x1,list1[y].t);
	}
	else if(mlcode[end].type==ML_CMP && mlcode[end].x2.type==TYPE_TMPVAR){		//CMP��ʱ����
		y=findPos2(mlcode[end].x2,n1,list1,n2,list2,1);
		y=list2[y].p;
		table_copy(mlcode[end].x2,list1[y].t);
	}

	for(;mlcode_i<end;mlcode_i++){							//�����µ��м���븳ML_NULL
		mlalt(mlcode_i,mlcode,ML_NULL,table_null,table_null,table_null);
	}

}

void opt_dag(int mlcode_n,MLCODE *mlcode){
	int n=0;					//��������
	int list[MAX_DAG];		//������
	int i;

	memset(list,0,sizeof(int)*MAX_DAG);

	//���ֻ�����
	for(i=1;i<=mlcode_n;i++){
		if(!( mlcode[i].type==ML_PLUS||mlcode[i].type==ML_MINUS||mlcode[i].type==ML_TIMES||mlcode[i].type==ML_SLASH||mlcode[i].type==ML_BEC ))
			list[n++]=i;
	}
	//����DAG
	for(i=0;i<n-1;i++){
		opt_dag_each(mlcode,list[i]+1,list[i+1]);
	}
}


/**************************************************/
//*****************************�����ϲ�********************
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
	int n=0;					//��������
	int list[MAX_DAG];		//������
	int i;

	memset(list,0,sizeof(int)*MAX_DAG);

	//���ֻ�����
	for(i=1;i<=mlcode_n;i++){
		if(!( mlcode[i].type==ML_PLUS||mlcode[i].type==ML_MINUS||mlcode[i].type==ML_TIMES||mlcode[i].type==ML_SLASH||mlcode[i].type==ML_BEC ))
			list[n++]=i;
	}
	//�����ϲ�
	for(i=0;i<n-1;i++){
		opt_constunion_each(mlcode,list[i]+1,list[i+1]);
	}
}


/**************************************************/
//*****************************ȫ�ּĴ�������EBX********************
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
	//Ϊlist1[j]����EBX
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
	int n=0;					//��������
	int list[MAX_DAG];		//������
	int i,j;

	memset(list,0,sizeof(int)*MAX_DAG);

	//���ֺ�����
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
//*****************************�����Ż�********************
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
	int n=0;					//��������
	int list[MAX_DAG];		//������
	int i;

	memset(list,0,sizeof(int)*MAX_DAG);

	//���ֻ�����
	for(i=1;i<=mlcode_n;i++){
		if(!( mlcode[i].type==ML_PLUS||mlcode[i].type==ML_MINUS||mlcode[i].type==ML_TIMES||mlcode[i].type==ML_SLASH||mlcode[i].type==ML_BEC ))
			list[n++]=i;
	}
	//�����ϲ�
	for(i=0;i<n-1;i++){
		opt_peephole_each(mlcode,list[i]+1,list[i+1]);
	}
}


