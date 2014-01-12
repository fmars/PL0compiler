#include"header.h"

const char* const keytxt[MAX_KEYWORD]=		//key word
{
	"const","var","integer","char","array","of","procedure","function",
	"begin","end","if","then","else","case","for","downto","to","do","read","write"
};
enum                                        //enum key word
{
	KW_CONST,KW_VAR,KW_INTEGER,KW_CHAR,KW_ARRAY,KW_OF,KW_PROCEDURE,KW_FUNCTION,
	KW_BEGIN,KW_END,KW_IF,KW_THEN,KW_ELSE,KW_CASE,KW_FOR,KW_DOWNTO,KW_TO,KW_DO,
	KW_READ,KW_WRITE
};
const char opatxt[MAX_OPWORDA]=				//unary operator
{
	'+','-','*','/','<','>','='
};
const char* opbtxt[MAX_OPWORDB]=		    //binary operator
{
	"<=",">=",":=","<>"
};
enum                                        //enum operator
{
	OW_PLUS,OW_MINUS,OW_TIMES,OW_SLASH,	//'+','-','*','/'
    OW_LT,OW_LG,OW_EQL,OW_LTE,OW_LGE,OW_BEC,OW_UEQ//'<','>','=','#',"<=",">=",":=","<>"
};
const char eoptxt[MAX_ENDWORD]=				//bound
{
	',',';',':','(',')','.','[',']'
};
enum                                        //enum bound
{EW_COMMA,EW_SEM,EW_COLON,EW_LP,EW_RP,EW_END,EW_LMP,EW_RMP};

/*******************gobal variable*******************/
FILE*       file;                   //source file  //
FILE*       fileout;            //asm output//
SYM         sym;                    //current symbol//
SYM			sym_id;			//lasted read symbol//
int         presym;           //preprocessing main//
int         haveint;    
TABLE       table[MAX_FORM];		//symbol table//
    int     table_lvl;                        //
    int     table_adr;                        //table_adr
    int     table_n;                          //
    int     table_n_now;                      //
MLCODE      mlcode[MAX_MLCODE];     //ML//
    int     mlcode_n;               //ML factor//
    int     tmpvar_n;               //temporary varible
    int     label_n;                //temporary varible pointer
    TABLE   table_1;				  //
    TABLE   table_2;	              //
    TABLE   table_3;				  //
    TABLE   table_tmp;				  //
    TABLE   table_null;               //
char        asmcode[MAX_ASM][MAX_ASMLEN];//asm instruction
char        asmcodehead[MAX_ASMHEAD][MAX_ASMHEADLEN];
char        asmcodedata[MAX_ASMDATA][MAX_ASMDATALEN];
    int     asmcode_n;
    int     asmcodehead_n;
    int     asmcodedata_n;


/***********************************************/

/****************extern func********************/
extern void mlgen(int &mlcode_n,MLCODE *mlcode,ML_TYPE type,TABLE x1,TABLE x2,TABLE x3);
extern void tabletmpval(int x,TABLE&t);
extern void tableconst(int x,TABLE&t);
extern void tablelabel(int x,TABLE&t);
extern void table_copy(TABLE&t2,TABLE t1);
extern void asmgen(int mlcode_n,MLCODE *mlcode,int &asmcode_n,char asmcode[MAX_ASM][MAX_ASMLEN],
            int &asmcodehead_n,char asmcodehead[MAX_ASMHEAD][MAX_ASMHEADLEN],
            int &asmcodedata_n,char asmcodedata[MAX_ASMDATA][MAX_ASMDATALEN]);
extern void opt_dag(int mlcode_n,MLCODE *mlcode);							  
extern void opt_constunion(int mlcode_n,MLCODE *mlcode);					 
extern void opt_regisiter(int &mlcode_n,MLCODE *mlcode);					
extern void opt_peephole(int &mlcode_n,MLCODE *mlcode);					

/***********************************************/

/************interative call********************/
void block();
void callprocedure();
void statement();
void expression();
int getsym();
void printsymtable();
void printmlcode();
void printasmcode();
/************************************************/

/*handle fault*/
void error(int e){
    printf("Line:%d ERR:%d SYM:%s\n",sym.line,e,sym.name);
    getsym();
}

/*read one char*/
char getch(){
    char c;
	c = fgetc(file);
    if(c==EOF){
        printf("program incomplete ");
    }
    return c;
}
/*morphology analyser*/
int getsym(){
    int i,n=0,e=0;	
    char ch;
    if( presym < 5 ){
        if(presym==0){
            sym.type=TYPE_KEYWORD;
            sym.id=KW_PROCEDURE;
            strcpy(sym.name,"procedure");
        }else if(presym==1){
            sym.type=TYPE_SYMBOL;
            strcpy(sym.name,"main");
            strcpy(sym_id.name,"main");
        }else if(presym==2){
            sym.type=TYPE_ENDWORD;
            sym.id=EW_LP;
            strcpy(sym.name,"(");
        }else if(presym==3){
            sym.type=TYPE_ENDWORD;
            sym.id=EW_RP;
            strcpy(sym.name,")");
        }else if(presym==4){
            sym.type=TYPE_ENDWORD;
            sym.id=EW_SEM;
            strcpy(sym.name,";");
        }

        presym++;
        return n;
    }
	i=sym.line;
	memset(&sym,0,sizeof(SYM));
	sym.line=i;
	ch = getch();
	while(!ISCHAR(ch))			
	{
		if(ch=='\n') sym.line++;
		if(ch==EOF) return 0;
		ch = getch();
	}
	if(ISLETTER(ch))				//start with a char
	{
		while(ISLETTER(ch)||ISNUMBER(ch))
		{
			if(n<=MAX_SYMLEN) sym.name[n]=ch;
			n++;
			ch = getch();
		}
		fseek(file,-1,SEEK_CUR);
		sym.name[n]=0;//fmars
		sym.name[MAX_SYMLEN]=0;		
		if(n>MAX_SYMLEN)		
		{/*
			sym.type=TYPE_ERROR;
			sym.id=ERR_OVERSYMLEN;
			adderr(sym.name,ERR_OVERSYMLEN);
		*/
		}
		else
		{
			for(i=0;i<MAX_KEYWORD;i++)
				if(strcmp(sym.name,keytxt[i])==0) break;
            if(i<MAX_KEYWORD)		//belong to key word
				{
					sym.type=TYPE_KEYWORD;
					sym.id=i;
				}
            else					//not 
				{
					sym.type=TYPE_SYMBOL;	//symbol
					sym.id=0;
					sym_id.type = sym.type;
					sym_id.id = 0;
					strcpy(sym_id.name,sym.name);
				}
		}
		return n;
	}
	if(ISNUMBER(ch))				//start with a number
	{
		e=0;
		while(ISNUMBER(ch)||ISLETTER(ch))
		{
			if(ISLETTER(ch)) e=1;
			if(n<=MAX_SYMLEN) sym.name[n]=ch;
			n++;
			ch = getch();
		}
		fseek(file,-1,SEEK_CUR);
		sym.name[MAX_SYMLEN]=0;	
		sym.type=TYPE_NUMBER;
		sym.id=atoi(sym.name);
		/*
		if(e)			;	
			adderr(sym.name,ERR_NUMBER);
		else
			if(n>MAX_NUMLEN||sym.id>MAX_NUMBER)
				adderr(sym.name,ERR_OVERNUMLEN);
			return n;
		*/
		return n;//fmars
	}
	if(ISSQMRK(ch)) 				//start with '
	{
		ch = getch();
		while(!ISSQMRK(ch))
		{
			if(n>1) e=1;
			if(n<=1) sym.name[n]=ch;
			n++;
			ch = getch();
		}
		sym.name[MAX_SYMLEN]=0;
		sym.type=TYPE_CHAR;
		sym.id=sym.name[0];
		if(e)
			error(ERR_1);
		return n;//fmars
	}
	if(ISDQMRK(ch))				//start with ''	
	{
		ch = getch();
		while(!ISDQMRK(ch))
		{
			if(n<=MAX_SYMLEN) sym.name[n]=ch;
			n++;
			ch = getch();
		}
		sym.name[n]=0;
		sym.name[MAX_SYMLEN]=0;
		sym.type=TYPE_STRING;
		sym.id=0;
		if(n>MAX_NUMLEN)error(ERR_2);
		return n;//fmars
	}
	{				
		char strop[3];
		if(ch=='.'){
		    strop[0]=ch;
		    strop[1]='\0';
		    sym.type=TYPE_ENDWORD;
		    sym.id=EW_END;
		    return 1;
		}
		strop[0] = ch;
		strop[1] = getch();
		strop[2] = '\0';
		for(i=0;i<MAX_OPWORDB;i++)	
			if(strcmp(strop,opbtxt[i])==0)break;
		if(i<MAX_OPWORDB)
		{
			sym.type=TYPE_OPWORD;
			sym.id=MAX_OPWORDA+i;
			strcpy(sym.name,strop);
			return 2;
		}
		fseek(file,-1,SEEK_CUR);
		sym.name[0]=ch;
		sym.name[1]=0;
		for(i=0;i<MAX_OPWORDA;i++)
			if(ch==opatxt[i]) break;
		if(i<MAX_OPWORDA)
		{
			sym.type=TYPE_OPWORD;
			sym.id=i;
			return 1;
		}
		for(i=0;i<MAX_ENDWORD;i++)
			if(ch==eoptxt[i]) break;
		if(i<MAX_ENDWORD)
		{
			sym.type=TYPE_ENDWORD;
			sym.id=i;
			return 1;
		}
		sym.type=TYPE_ERROR;
		//adderr(sym.name,ERR_NOTDEF);
		return 1;
	}
}

/*set up symbol talbe*/
int entry(DEC_TYPE type,int para1,int para2=0){
	table_n++;
    switch( type )
    {

        case TYPE_CONST :
        {
            memset(&table[table_n],sizeof(TABLE),0);
            table[table_n].type=type;
            table[table_n].kind=para1;
            table[table_n].lvl=table_lvl;
			if(para1 == 1)
                table[table_n].val = atoi(sym.name);
            else
                table[table_n].val = (char)sym.name[0];
            strcpy(table[table_n].name,sym_id.name);
            break;
        }
        case TYPE_VAR:
        {
            memset(&table[table_n],sizeof(TABLE),0);
            table[table_n].type=type;
            table[table_n].kind=para1;
            table[table_n].lvl=table_lvl;
            table[table_n].adr=++table_adr;
            if(para1==3||para1==4)
                table_adr+=para2-1 ;
            table[table_n].val = para2;
                                      
			strcpy(table[table_n].name,sym_id.name);
            break;
        }
        case TYPE_PROCEDURE:
        {
            memset(&table[table_n],sizeof(TABLE),0);
            table[table_n].type=type;
            table[table_n].lvl=table_lvl;
            strcpy(table[table_n].name,sym_id.name);
            break;
        }
        case TYPE_FUNCTION:
        {
            memset(&table[table_n],sizeof(TABLE),0);
            table[table_n].type=type;
            table[table_n].lvl=table_lvl;
            strcpy(table[table_n].name,sym_id.name);
            break;
        }
        case TYPE_NULL:
        case TYPE_TMPVAR:
        case TYPE_LABEL:;
    }
    return 0;
}
/*look up symbol in symbol table*/
int position(SYM pos_sym){
    int i,j;
    strcpy(table[0].name,pos_sym.name);
    i=table_n;
    j=table_lvl;
    while(!(strcmp(table[i].name,pos_sym.name)==0 && table[i].lvl<=j))
    {
        i--;
        if(table[i].lvl<j)j=table[i].lvl;
    }
    //0 denotes not found
    return i;
}
/*look up the level of current function in symbol table*/
int position(int n){
    int i = n;
    while(table[i].lvl!=table_lvl-1)
        i--;
    return i;
}


/*constant declaration*/
void constdeclaration(){
    if( sym.type == TYPE_SYMBOL ){
        getsym();
        if( sym.type == TYPE_OPWORD && sym.id == OW_EQL ){
            getsym();
            if( sym.type == TYPE_CHAR )
                entry(TYPE_CONST,2);
            else if( sym.type == TYPE_OPWORD && (sym.id == OW_PLUS)||(sym.id == OW_MINUS) ){
                char ch = sym.name[0];
                getsym();
                if( sym.type == TYPE_NUMBER ){
                    sym.name[strlen(sym.name)+1] = 0;
                    for( int i=strlen(sym.name);i>=0;i--)sym.name[i+1]=sym.name[i];
                    sym.name[0]=ch;
                    entry(TYPE_CONST,1);
                }else error(ERR_3);      
            }
            else if( sym.type == TYPE_NUMBER )
                entry(TYPE_CONST,1);
			getsym();
        }
        else error(ERR_3);
    }
    else error(ERR_3);  
}
/*varible declaration*/
void vardeclaration(){
    char sym_name_var[MAX_VARNUM][MAX_SYMLEN+1];
    int k=0;//number of varibles
    int size;
    int i;
    strcpy(sym_name_var[k++],sym.name);
    getsym();
    while( sym.type == TYPE_ENDWORD && sym.id == EW_COMMA ){
        getsym();
        strcpy(sym_name_var[k++],sym.name);
		getsym();
    }
    if( sym.type == TYPE_ENDWORD && sym.id == EW_COLON ){               // :
        getsym();
        if( sym.type == TYPE_KEYWORD && sym.id == KW_INTEGER ){            //integer
            for( i=0; i<k; i++){
                strcpy(sym_id.name,sym_name_var[i]);
                entry(TYPE_VAR,1);
            }
        }else if( sym.type == TYPE_KEYWORD && sym.id == KW_CHAR){   //char
            for( i=0; i<k; i++){
                strcpy(sym_id.name,sym_name_var[i]);
                entry(TYPE_VAR,2);
            }
        }else if( sym.type == TYPE_KEYWORD && sym.id == KW_ARRAY){      //array
            getsym();
            if( sym.type==TYPE_ENDWORD && sym.id==EW_LMP ){             //[
                getsym();
                if( sym.type==TYPE_NUMBER ){                            
                    size = atoi(sym.name);
                    getsym();
                    if( sym.type==TYPE_ENDWORD && sym.id == EW_RMP ){   //]
                        getsym();
                        if( sym.type == TYPE_KEYWORD && sym.id == KW_OF ){  //of
                            getsym();
                            if( sym.type == TYPE_KEYWORD && sym.id == KW_INTEGER ){//integer
                                for( i=0; i<k; i++){
                                    strcpy(sym_id.name,sym_name_var[i]);
                                    entry(TYPE_VAR,3,size);
                                }
                            }else if( sym.type == TYPE_KEYWORD && sym.id == KW_CHAR ){//char
                                for( i=0; i<k; i++){
                                    strcpy(sym_id.name,sym_name_var[i]);
                                    entry(TYPE_VAR,4,size);
                                }
                            }
                        }else error(ERR_4);
                    }else error(ERR_4); 
                }else error(ERR_4); 
            }else error(ERR_4); 
        }else error(ERR_4); 
    }else error(ERR_4); 
	getsym();
}
/*function declaration*/
void procdeclaration(){
    bool isfunc = false;
    int isvar =0;
    int table_n_begin,table_adr_cpy;
    int k,i;
    char sym_name_var[MAX_VARNUM][MAX_SYMLEN+1];

    while(sym.type == TYPE_KEYWORD && (sym.id == KW_PROCEDURE||sym.id == KW_FUNCTION)){
        if( sym.id == KW_FUNCTION ) isfunc=true;   
            else isfunc=false;
        getsym();
        if( sym.type == TYPE_SYMBOL ){             
            if(isfunc) entry(TYPE_FUNCTION,0,0);
                else entry(TYPE_PROCEDURE,0,0);
            table_n_begin = table_n;
            table_lvl++;
            table_adr_cpy=table_adr;
            table_adr=0;
            getsym();

            if( sym.type == TYPE_ENDWORD && sym.id == EW_LP )  
                getsym();
            else
                error(ERR_5);
            if( sym.type == TYPE_ENDWORD && sym.id == EW_RP )   
                getsym();
            else
            {
            {
                isvar = 0;
                if( sym.type == TYPE_KEYWORD && sym.id == KW_VAR ){
                    isvar = 1;
                    getsym();
                }
                else
                    //12.29
					//isvar = 0;
					isvar=2;
              
                k=0;
                strcpy(sym_name_var[k++],sym.name);
                getsym();
                while(sym.type == TYPE_ENDWORD && sym.id == EW_COMMA ){
                    getsym();
                    strcpy(sym_name_var[k++],sym.name);
                    getsym();
                }
                if(sym.type == TYPE_ENDWORD && sym.id == EW_COLON){
                    getsym();
                    if( sym.type == TYPE_KEYWORD && sym.id == KW_INTEGER ){
                        for( i=0; i<k; i++){
                            strcpy(sym_id.name,sym_name_var[i]);
                            if(isvar==1)
                                entry(TYPE_VAR,1,1);
                            else
                                entry(TYPE_VAR,1,2);
                        }
                    }else if( sym.type == TYPE_KEYWORD && sym.id == KW_CHAR){
                        for( i=0; i<k; i++){
                            strcpy(sym_id.name,sym_name_var[i]);
                            if(isvar==1)
                                entry(TYPE_VAR,2,1);
                            else
                                entry(TYPE_VAR,2,2);
                        }
                    }
                    getsym();
                }
            }

            
                while(sym.type == TYPE_ENDWORD && sym.id == EW_SEM){
                    getsym();
                    if( sym.type == TYPE_KEYWORD && sym.id == KW_VAR ){
                        isvar = 1;
						getsym();
					}
                    else
						//12.29
                        //isvar = 0;
						isvar=2;
                      
                    k=0;
                    strcpy(sym_name_var[k++],sym.name);
                    getsym();
                    while(sym.type == TYPE_ENDWORD && sym.id == EW_COMMA ){
                        getsym();
                        strcpy(sym_name_var[k++],sym.name);
                        getsym();
                    }
                    if(sym.type == TYPE_ENDWORD && sym.id == EW_COLON){
                        getsym();
                        if( sym.type == TYPE_KEYWORD && sym.id == KW_INTEGER ){//integer
                            for( i=0; i<k; i++){
                                strcpy(sym_id.name,sym_name_var[i]);
                                entry(TYPE_VAR,1,isvar);
                            }
                        }else if( sym.type == TYPE_KEYWORD && sym.id == KW_CHAR){//symÊÇchar
                            for( i=0; i<k; i++){
                                strcpy(sym_id.name,sym_name_var[i]);
                                entry(TYPE_VAR,2,isvar);
                            }
                        }
                        getsym();
                    }
                }
                if(sym.type==TYPE_ENDWORD&& sym.id==EW_RP)  //)
                    getsym();
                else error(ERR_6);
            }
            //the number of argument of function
            table[table_n_begin].val = table_n-table_n_begin;

            if(isfunc){                         //return value
                if( sym.type == TYPE_ENDWORD && sym.id == EW_COLON )//:
                    getsym();
                else
                    error(ERR_7);
                if( sym.type == TYPE_KEYWORD && sym.id == KW_INTEGER )
                    table[table_n_begin].kind=1;
                else if( sym.type ==TYPE_KEYWORD && sym.id == KW_CHAR )
                    table[table_n_begin].kind=2;
                else error(ERR_8);
                getsym();
            }

            if( sym.type == TYPE_ENDWORD && sym.id == EW_SEM )//;
                getsym();
            else error(ERR_9);
        }
    block();   
    table_lvl--;
    table_adr=table_adr_cpy;
    //12.19
    if(sym.type==TYPE_ENDWORD&&sym.id==EW_SEM)
        getsym();
    else if(sym.type==TYPE_ENDWORD&&sym.id==EW_END)
        ;
    else error(ERR_10);
    }
}



/*factor*/
void factor(){
    int i;
    if( sym.type==TYPE_NUMBER ){        //unsigned int
        tableconst(sym.id,table_1);
        tabletmpval(++tmpvar_n,table_3);
        mlgen(mlcode_n,mlcode,ML_BEC,table_1,table_null,table_3);
		getsym();
    }
    else if( sym.type==TYPE_SYMBOL){   //symbol
        i=position(sym);
        if(i==0)error(ERR_11);
        if(table[i].type == TYPE_CONST){        //constant
            //12.18
            if( ISNUMBER(table[i].name[0]) )haveint=1;
            tabletmpval(++tmpvar_n,table_3);
            mlgen(mlcode_n,mlcode,ML_BEC,table[i],table_null,table_3);
			getsym();
        }
        else if(table[i].type == TYPE_VAR){    //variable
            //12.18
            if(table[i].kind==1||table[i].kind==3)haveint=1;
            if(table[i].kind==1||table[i].kind==2){    
                tabletmpval(++tmpvar_n,table_3);
                mlgen(mlcode_n,mlcode,ML_BEC,table[i],table_null,table_3);
				getsym();
            }
            else{                              //array
                getsym();
                if(sym.type==TYPE_ENDWORD&&sym.id==EW_LMP)
                    getsym();
                else
                    error(ERR_5);
                expression();
                tabletmpval(tmpvar_n,table_2);
                tabletmpval(++tmpvar_n,table_3);
                mlgen(mlcode_n,mlcode,ML_BEC,table[i],table_2,table_3);
                if(sym.type==TYPE_ENDWORD&&sym.id==EW_RMP)
                    getsym();
                else
                    error(ERR_6);
                    }
        }
        else if(table[i].type == TYPE_FUNCTION){//function call
            mlgen(mlcode_n,mlcode,ML_PUSH,table_null,table_null,table_null);//push back return value
            callprocedure();
            tabletmpval(++tmpvar_n,table_2);
            mlgen(mlcode_n,mlcode,ML_POP,table_null,table_2,table_null);//pop return value
        }else error(ERR_14);
    }
    else if( sym.type == TYPE_ENDWORD && sym.id == EW_LP ){//(
		getsym();
        expression();
		if(sym.type == TYPE_ENDWORD && sym.id == EW_RP)
			getsym();
		else
			error(ERR_6);
    }
    else error(ERR_14);
}
/*term*/
void term(){
    int mulop;
    int tmpvar_n_cur;
    factor();
    tmpvar_n_cur=tmpvar_n;
    while(sym.type==TYPE_OPWORD&&(sym.id==OW_TIMES||sym.id==OW_SLASH)){
        mulop=sym.id;
        getsym();
        factor();

        tabletmpval(tmpvar_n_cur,table_1);
        tabletmpval(tmpvar_n,table_2);
        tmpvar_n++;
        tabletmpval(tmpvar_n,table_3);

        if(mulop==OW_TIMES)
            mlgen(mlcode_n,mlcode,ML_TIMES,table_1,table_2,table_3);
        else
            mlgen(mlcode_n,mlcode,ML_SLASH,table_1,table_2,table_3);
        tmpvar_n_cur=tmpvar_n;
    }
}
/*expression*/
void expression(){
    int addop;
    int tmpvar_n_cur;
    if(sym.type==TYPE_OPWORD && (sym.id==OW_PLUS || sym.id==OW_MINUS)){
        addop = sym.id;
        getsym();
        term();
        tmpvar_n_cur=tmpvar_n;
        if(addop == OW_MINUS){      //-
            tmpvar_n++;
            tableconst(0,table_1);
            tabletmpval(tmpvar_n_cur,table_2);
            tabletmpval(tmpvar_n,table_3);
            mlgen(mlcode_n,mlcode,ML_MINUS,table_1,table_2,table_3);
            ;
        }
    }else
        term();
    tmpvar_n_cur=tmpvar_n;
    while(sym.type==TYPE_OPWORD && (sym.id==OW_PLUS || sym.id==OW_MINUS)){
        addop=sym.id;
        getsym();
        term();
        //create instruction 
            tabletmpval(tmpvar_n_cur,table_1);
            tabletmpval(tmpvar_n,table_2);
            tmpvar_n++;
            tabletmpval(tmpvar_n,table_3);
        if(addop==OW_PLUS)
            mlgen(mlcode_n,mlcode,ML_PLUS,table_1,table_2,table_3);
        else
            mlgen(mlcode_n,mlcode,ML_MINUS,table_1,table_2,table_3);
        tmpvar_n_cur=tmpvar_n;
    }
}

/*function call*/
void callprocedure(){
    int i,j,k;
    if(sym.type==TYPE_SYMBOL){
        i=position(sym);
        getsym();
    }
    else error(ERR_11);

    if(table[i].val==0){        //no argument
        if(sym.type==TYPE_ENDWORD&&sym.id==EW_LP)
            getsym();
        else error(ERR_5);
        if(sym.type==TYPE_ENDWORD&&sym.id==EW_RP)
            getsym();
        else error(ERR_6);
		//12.31
		tableconst(0,table_2);
		mlgen(mlcode_n,mlcode,ML_PUSH,table_null,table_2,table_null);
        mlgen(mlcode_n,mlcode,ML_CALL,table_null,table[i],table_null);
    }
    else{                       //argument
        if(sym.type==TYPE_ENDWORD&&sym.id==EW_LP){
            sym.type=TYPE_ENDWORD;
            sym.id=EW_COMMA;
        }else error(ERR_5);
        for(k=1;k<=table[i].val;k++){
            if(sym.type==TYPE_ENDWORD&&sym.id==EW_COMMA)
                getsym();
            else error(ERR_12);
            if(table[i+k].val==1){//var argument
                j=position(sym);
                if(table[j].type==TYPE_VAR)
				{
					if(table[j].kind>2){
						getsym();
						if(sym.type==TYPE_ENDWORD&&sym.id==EW_LMP)
							getsym();
						else error(ERR_5);
						expression();
						tableconst(1,table_1);
						tabletmpval(tmpvar_n,table_3);
						mlgen(mlcode_n,mlcode,ML_PUSH,table_1,table[j],table_3);
						if(sym.type==TYPE_ENDWORD&&sym.id==EW_RMP)
							getsym();
						else error(ERR_6);
					}
					else 
					{
						tableconst( 1,table_1);
						mlgen(mlcode_n,mlcode,ML_PUSH,table_1,table[j],table_null);
						getsym();
					}
				}
			}else{                 
				expression();
				tabletmpval(tmpvar_n,table_2);
				tableconst(2,table_1);
				mlgen(mlcode_n,mlcode,ML_PUSH,table_1,table_2,table_null);
			}
		}
		

        tableconst(table[i].val*4,table_2);
        //the number of argument pushed back
        mlgen(mlcode_n,mlcode,ML_PUSH,table_null,table_2,table_null);
        mlgen(mlcode_n,mlcode,ML_CALL,table_null,table[i],table_null);
            //1.display zone
        tableconst(table[i].lvl*4,table_2);
        mlgen(mlcode_n,mlcode,ML_POP,table_null,table_2,table_null);
            //2.argument zone
        tableconst(table[i].val*4+4,table_2);
        mlgen(mlcode_n,mlcode,ML_POP,table_null,table_2,table_null);
      

        if(sym.type==TYPE_ENDWORD&&sym.id==EW_RP)
            getsym();
        else error(ERR_6);
    }
}
/*condition*/
void condition(){
	char strop[MAX_SYMLEN];
	//12.17
	TABLE table_1_tmp;
	int mlcode_n_1,mlcode_n_2;
	if(sym.type==TYPE_KEYWORD&&sym.id==KW_IF)//IF
		getsym();
	else error(ERR_13);
	expression();
	tabletmpval(tmpvar_n,table_1);
	//12.17
	table_copy(table_1_tmp,table_1);
	if(sym.type==TYPE_OPWORD){
		strcpy(strop,sym.name);
		getsym();
	}else error(ERR_15);
    expression();
    tabletmpval(tmpvar_n,table_2);
    //create cmp instruction
    mlgen(mlcode_n,mlcode,ML_CMP,table_1_tmp,table_2,table_null);
    //jump to lable
    mlgen(mlcode_n,mlcode,ML_JWNT,table_null,table_null,table_null);
    strcpy(mlcode[mlcode_n].x1.name,strop);
    mlcode_n_1=mlcode_n;

    if(sym.type==TYPE_KEYWORD&&sym.id==KW_THEN)//THEN
        getsym();
    else error(ERR_16);
    statement();
    //12.19
	//if(sym.type==TYPE_ENDWORD && sym.id==EW_SEM)
	//	getsym();
	//else error();
    mlgen(mlcode_n,mlcode,ML_JMP,table_null,table_null,table_null);
    mlcode_n_2=mlcode_n;

    if(sym.type==TYPE_KEYWORD&&sym.id==KW_ELSE){//ELSE
        getsym();
        label_n++;
        tablelabel(label_n,table_2);
        //set lable
        mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);
        table_copy(mlcode[mlcode_n_1].x2,table_2);
        statement();
		//12.20
		//if(sym.type==TYPE_ENDWORD && sym.id==EW_SEM)
		//	getsym();
		//else error();
        label_n++;
        tablelabel(label_n,table_2);

        mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);
        table_copy(mlcode[mlcode_n_2].x2,table_2);
    }else{                                      
        label_n++;
        tablelabel(label_n,table_2);
        //set label
        mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);
        table_copy(mlcode[mlcode_n_1].x2,table_2);
        table_copy(mlcode[mlcode_n_2].x2,table_2);
    }

}
/*loop*/
void loop(){
    TABLE table_loop;//loop variable
    int i;
    bool f;//true-to or false-downto
    int mlcode_n_t;
    int label_n_t;
    if( sym.type == TYPE_KEYWORD && sym.id == KW_FOR )//FOR
        getsym();
    else error(ERR_17);
    if( sym.type == TYPE_SYMBOL ){
        i = position(sym);
        if( i==0 ) error(ERR_11);
        else if( table[i].type!=TYPE_VAR )	error(ERR_18);
        table_copy(table_loop,table[i]);
        getsym();
    }	else error(ERR_18);
    if( sym.type == TYPE_OPWORD && sym.id == OW_BEC )//:=
        getsym();
    else error(ERR_19);
    expression();
    tabletmpval(tmpvar_n,table_1);
    mlgen(mlcode_n,mlcode,ML_BEC,table_1,table_null,table_loop);//init loop variable


    if( sym.type == TYPE_KEYWORD && sym.id == KW_TO )   //to or downto
        f = true;
    else if( sym.type == TYPE_KEYWORD && sym.id ==KW_DOWNTO )
        f = false;
    else error(ERR_20);
    getsym();

    label_n++;
    tablelabel(label_n,table_2);
    mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);//create lable
    label_n_t=label_n;//record lable

    expression();
    tabletmpval(tmpvar_n,table_2);
    mlgen(mlcode_n,mlcode,ML_CMP,table_loop,table_2,table_null);//CMP
    mlgen(mlcode_n,mlcode,ML_JWNT,table_null,table_null,table_null);//JWNT
    //12.17
    if(f)//
        strcpy(mlcode[mlcode_n].x1.name,"<=");
    else
        strcpy(mlcode[mlcode_n].x1.name,">=");
    mlcode_n_t=mlcode_n;

    if( sym.type == TYPE_KEYWORD && sym.id == KW_DO )//DO
        getsym();
    else error(ERR_21);
    statement();
    //12.18
    /*
    if( sym.type == TYPE_ENDWORD && sym.id == EW_SEM )//;
        getsym();
    else error();
    */
    //update loop
    tableconst(1,table_2);
    tmpvar_n++;
    tabletmpval(tmpvar_n,table_3);
    if(f)
        mlgen(mlcode_n,mlcode,ML_PLUS,table_loop,table_2,table_3);
    else
        mlgen(mlcode_n,mlcode,ML_MINUS,table_loop,table_2,table_3);
    mlgen(mlcode_n,mlcode,ML_BEC,table_3,table_null,table_loop);

    tablelabel(label_n_t,table_2);
    mlgen(mlcode_n,mlcode,ML_JMP,table_null,table_2,table_null);//JMP

    label_n++;
    tablelabel(label_n,table_2);
    mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);//create lable
    table_copy(mlcode[mlcode_n_t].x2,table_2);//return lable


}
/*case*/
void mcase(){
    int tmpvar_n_t;
    int mlcode_n_each[MAX_CONSTEACHROW],num1;
    int mlcode_n_row;
    int mlcode_n_eachrow[MAX_ROW],num2;
    int i;
    if( sym.type == TYPE_KEYWORD && sym.id == KW_CASE )
        getsym();
    else error(ERR_22);

    expression();
    tmpvar_n_t=tmpvar_n;
    tabletmpval(tmpvar_n,table_1);

    if( sym.type == TYPE_KEYWORD && sym.id == KW_OF )
        getsym();
    else error(ERR_23);

    num2 = 0;
    while ( !(sym.type == TYPE_KEYWORD && sym.id == KW_END) ){
        num1 = 0;

        if( num2 != 0 ){
            label_n++;
            tablelabel(label_n,table_2);
            mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);
            table_copy( mlcode[ mlcode_n_row ].x2,table_2 );
        }

        tabletmpval(tmpvar_n_t,table_1);
        tableconst(sym.id,table_2);
        mlgen(mlcode_n,mlcode,ML_CMP,table_1,table_2,table_null);
        mlgen(mlcode_n,mlcode,ML_JWNT,table_null,table_null,table_null);
        mlcode_n_each[num1++] = mlcode_n;
        strcpy(mlcode[mlcode_n].x1.name,"<>");
        getsym();


        while( sym.type == TYPE_ENDWORD && sym.id == EW_COMMA ){
            getsym();
            if( sym.type == TYPE_NUMBER || sym.type == TYPE_CHAR )
            {
                tableconst(sym.id,table_2);
                getsym();
            }else error(ERR_24);
            tabletmpval(tmpvar_n_t,table_1);
            mlgen(mlcode_n,mlcode,ML_CMP,table_1,table_2,table_null);
            mlgen(mlcode_n,mlcode,ML_JWNT,table_null,table_null,table_null);
            mlcode_n_each[num1++] = mlcode_n;
            strcpy(mlcode[mlcode_n].x1.name,"<>");
        }
        mlgen(mlcode_n,mlcode,ML_JMP,table_null,table_null,table_null);
        mlcode_n_row = mlcode_n;

        if( sym.type == TYPE_ENDWORD && sym.id == EW_COLON )
            getsym();
        else error(ERR_7);
        label_n++;
        tablelabel(label_n,table_2);
        mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);
        for( i=0;i<num1;i++ )
            table_copy( mlcode[ mlcode_n_each[i] ].x2,table_2);

        statement();
        if( sym.type==TYPE_ENDWORD && sym.id == EW_SEM )
            getsym();
		//12.29
		else if(!( sym.type==TYPE_KEYWORD && sym.id == KW_END ))error(ERR_25);

        mlgen(mlcode_n,mlcode,ML_JMP,table_null,table_null,table_null);
        mlcode_n_eachrow[num2++] = mlcode_n;

    }
    label_n++;
    tablelabel(label_n,table_2);
    mlgen(mlcode_n,mlcode,ML_LABEL,table_null,table_2,table_null);
    for( i=0;i<num2;i++)
        table_copy( mlcode[ mlcode_n_eachrow[i] ].x2,table_2);
    table_copy( mlcode[ mlcode_n_row ].x2,table_2 );
    if(sym.type==TYPE_KEYWORD&&sym.id==KW_END)
        getsym();
    else error(ERR_26);
}
void mread(){
    int i;
    if(sym.type==TYPE_KEYWORD && sym.id==KW_READ)
        getsym();
    if(sym.type==TYPE_ENDWORD && sym.id==EW_LP){
        sym.type=TYPE_ENDWORD;
        sym.id=EW_COMMA;
    }
    while(sym.type==TYPE_ENDWORD && sym.id==EW_COMMA){
        getsym();
        i=position(sym);
        if(table[i].type==TYPE_VAR && table[i].kind<=2 && table[i].val==0){
            table_copy(table_2,table[i]);
            mlgen(mlcode_n,mlcode,ML_READ,table_null,table_2,table_null);
        }
        else error(ERR_27);
        getsym();
    }
    if(sym.type==TYPE_ENDWORD && sym.id==EW_RP)
        getsym();
    else
        error(ERR_6);
}
/*wriete*/
void mwrite(){
	TABLE table_ttmp;
    table_copy(table_1,table_null);
    table_copy(table_2,table_null);
    table_copy(table_3,table_null);
    if(sym.type==TYPE_KEYWORD && sym.id==KW_WRITE)
        getsym();
    if(sym.type==TYPE_ENDWORD && sym.id==EW_LP)
        getsym();
    else error(ERR_5);
    if(sym.type==TYPE_STRING){
        strcpy(table_2.name,sym.name);
        table_2.type=TYPE_CONST;
        getsym();
        if(sym.type==TYPE_ENDWORD&&sym.id==EW_COMMA){
			table_copy(table_ttmp,table_2);
            getsym();
            haveint=0;
            expression();
			table_copy(table_2,table_ttmp);
            tableconst(haveint,table_1);
            tabletmpval(tmpvar_n,table_3);
        }
    }
    else{
		
        haveint=0;
        expression();
        tableconst(haveint,table_1);
		table_copy(table_2,table_null);
        tabletmpval(tmpvar_n,table_3);
    }
    if(sym.type==TYPE_ENDWORD&&sym.id==EW_RP){
        mlgen(mlcode_n,mlcode,ML_WRITE,table_1,table_2,table_3);
        getsym();
    }
    else error(ERR_6);
}/*statement*/
void statement(){
    int i;
    //12.17
    TABLE table_2_tmp;
    if(sym.type == TYPE_SYMBOL){    
        i = position(sym);
        if(i==0)
            error(ERR_11);
        else if(table[i].type==TYPE_CONST)
            error(ERR_18);
        else if(table[i].type==TYPE_PROCEDURE){
            callprocedure();
			return ;
		}
        else getsym();
        if(sym.type==TYPE_OPWORD&&sym.id==OW_BEC){         
            getsym();
            expression();
            tabletmpval(tmpvar_n,table_1);
            mlgen(mlcode_n,mlcode,ML_BEC,table_1,table_null,table[i]);
        }else if(sym.type==TYPE_ENDWORD && sym.id==EW_LMP){ 
            getsym();
            expression();
            tabletmpval(tmpvar_n,table_2);
            //12.17
            table_copy(table_2_tmp,table_2);
            //getsym();
            if(sym.type==TYPE_ENDWORD&&sym.id==EW_RMP)
                getsym();
            else
                error(ERR_6);
            if(sym.type==TYPE_OPWORD&&sym.id==OW_BEC)
                getsym();
            else
                error(ERR_19);
            expression();
            tabletmpval(tmpvar_n,table_1);
            mlgen(mlcode_n,mlcode,ML_BEC,table_1,table_2_tmp,table[i]);
        }else error(ERR_28);
    }
    else if(sym.type == TYPE_KEYWORD && sym.id == KW_IF ){  //if

        condition();
    }
    else if(sym.type == TYPE_KEYWORD && sym.id == KW_FOR ){ //For
        loop();
    }
    else if(sym.type == TYPE_KEYWORD && sym.id==KW_CASE){   //case
        mcase();
    }
    else if(sym.type == TYPE_KEYWORD && sym.id==KW_BEGIN){  
        getsym();
		statement();
		while(sym.type==TYPE_ENDWORD && sym.id==EW_SEM){
			getsym();
			statement();
		}
        if(sym.type==TYPE_KEYWORD && sym.id ==KW_END)
            getsym();
        //12.19
        //if(sym.type==TYPE_ENDWORD && sym.id ==EW_SEM)
        //    getsym();
        // if(sym.type==TYPE_ENDWORD&&sym.id==EW_END)
        //   ;
        else error(ERR_26);
    }
    else if(sym.type == TYPE_KEYWORD && sym.id==KW_READ){  //read
        mread();
    }
    else if(sym.type == TYPE_KEYWORD && sym.id==KW_WRITE){ //write
        mwrite();
    }
	else ;
    //else error();

}


void block(){
    int mlcode_n_begin;
    int table_n_proc;
    int i;
    //12.17
    table_adr=0;
    if(sym.type == TYPE_KEYWORD && sym.id == KW_CONST){    
        getsym();
        while( sym.type == TYPE_SYMBOL){
            constdeclaration();
            while( sym.type == TYPE_ENDWORD && sym.id == EW_COMMA){
                getsym();
                constdeclaration();
            }
            if( sym.type == TYPE_ENDWORD && sym.id == EW_SEM )
                getsym();
            else
                error(ERR_9);         
        }
    }
    if(sym.type == TYPE_KEYWORD && sym.id == KW_VAR){      
        getsym();
        while( sym.type == TYPE_SYMBOL){
            vardeclaration();
			if( sym.type == TYPE_ENDWORD && sym.id == EW_SEM )
				getsym();
			else
				error(ERR_9);	
        }
   }
    i=table_adr;
    table_n_now = table_n;             
    if(sym.type == TYPE_KEYWORD && (sym.id == KW_PROCEDURE||sym.id == KW_FUNCTION)){//
        procdeclaration();
    }
    tmpvar_n=0;            
    table_n_proc=position(table_n_now);
    //12.17
    //table[table_n_proc].adr=i-table[table_n_proc].val;
    table[table_n_proc].adr = i;
    mlgen(mlcode_n,mlcode,ML_BEGIN,table_null,table[table_n_proc],table_null); 
	mlcode_n_begin=mlcode_n;
    statement();
    mlcode[mlcode_n_begin].x3.val=tmpvar_n;
    mlgen(mlcode_n,mlcode,ML_END,table_null,table[table_n_proc],table_null);
 /*   if(table_lvl==1){
        strcpy(mlcode[mlcode_n_begin].x2.name,"main");
        strcpy(mlcode[mlcode_n].x2.name,"main");
    }
*/
}

int main()
{
	char fnamein[1024];
	char fnameout[1024]="a.out";
	char c;
	printf("Input Source File\n");
	scanf("%s",&fnamein);
	file=fopen(fnamein,"r");
    fileout = fopen(fnameout,"w");

    presym = 0;

    table_lvl=1;	//lvl starts from 1
    table_n=0;
    table_adr=0;	//adr starts from 1

    mlcode_n=0;
    label_n=0;


    asmcode_n=0;
    asmcodehead_n=0;
    asmcodedata_n=0;

    memset(&sym,0,sizeof(SYM));
	memset(&sym_id,0,sizeof(SYM));
    memset(&table_null,0,sizeof(TABLE));
    table_null.type=TYPE_NULL;
    getsym();
    //block();       
    procdeclaration();


	//display symbol table
	scanf("%c",&c);
	printf("****************************\nShow SymTable?y/n...\t");
	scanf("%c",&c);
	if(c=='y')printsymtable();

	//display intermediate code
	scanf("%c",&c);
	printf("Show Mlcode?y/n...\t");
	scanf("%c",&c);
	if(c=='y')printmlcode();

	//DAG opt
	scanf("%c",&c);
	printf("****************************\nOptimize Option -> DAG ?y/n...");
	scanf("%c",&c);
	if(c=='y'){
		opt_dag(mlcode_n,mlcode);
		scanf("%c",&c);
		printf("Show Mlcode?y/n...");
		scanf("%c",&c);
		if(c=='y')printmlcode();
	}	

	//constant opt
	scanf("%c",&c);
	printf("****************************\nOptimize Option -> Constant Folding?y/n...");
	scanf("%c",&c);
	if(c=='y'){
		opt_constunion(mlcode_n,mlcode);
		scanf("%c",&c);
		printf("Show Mlcode?y/n...");
		scanf("%c",&c);
		if(c=='y')printmlcode();
	}	
	


	
	//peephole opt
	scanf("%c",&c);
	printf("****************************\nOptimize Option -> Peephole?y/n...");
	scanf("%c",&c);
	if(c=='y'){
		opt_peephole(mlcode_n,mlcode);
		scanf("%c",&c);
		printf("Show Mlcode?y/n...");
		scanf("%c",&c);
		if(c=='y')printmlcode();
	}

	//generate asm
	asmgen(mlcode_n,mlcode,asmcode_n,asmcode,
		asmcodehead_n,asmcodehead,
		asmcodedata_n,asmcodedata);
	
	printasmcode();

	fclose(fileout);
	fclose(file);

    return 0;
}

void printsymtable(){
	//display symbol table
    printf("\n***********************\n");
    printf("\ttable_n\tName\tLvl\tAdr\tType\tKind\tVal\n");
    for(int i=1;i<=table_n;i++)
        printf("\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
            i,table[i].name,table[i].lvl,table[i].adr,table[i].type,table[i].kind,table[i].val);
}
void printmlcode(){
	//display intermediate code
	printf("\n***********************\n");
	printf("\tn\topt\tx1\tx2\tx3\n");
	for(int i=1;i<=mlcode_n;i++)
		printf("\t%d\t%d\t%s\t%s\t%s\n",i,mlcode[i].type,mlcode[i].x1.name,mlcode[i].x2.name,mlcode[i].x3.name);
}
void printasmcode(){
	//display asm
	int i;
    printf("***********************\n");
    for(i=1;i<=asmcodehead_n;i++)
        fprintf(fileout,"%s\n",asmcodehead[i]);
    for(i=0;i<=asmcodedata_n;i++)
        fprintf(fileout,"%s\n",asmcodedata[i]);
    for(i=1;i<=asmcode_n;i++)
        fprintf(fileout,"%s\n",asmcode[i]);
    fprintf(fileout,"%s\n",asmcodehead[0]);

}
