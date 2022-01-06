#include <cstdio>
#include <cstring>
#include <cmath>
#include <stack>
#include <cstdlib>

using namespace std;

struct token {
	// 0-关键字，1-变量， 2-数字，3-临时变量
	int code;
	int value;
};

struct map                             //当前字符到状态转换矩阵列标记的映射

{

	char str[50];

	int col;

};

int num_key = 6;

int aut[11][8]={ 0, 0, 0, 0, 0, 0, 0, 0,

	0, 2, 0, 0, 0, 8, 9,14,

	0, 2, 3, 5,11, 0, 0,11,

	0, 4, 0, 0, 0, 0, 0, 0,

	0, 4, 0, 5,11, 0, 0,11,

	0, 7, 0, 0, 6, 0, 0, 0,

	0, 7, 0, 0, 0, 0, 0, 0,

	0, 7, 0, 0,11, 0, 0,11,

	0, 8, 0, 0, 0, 8, 0,12,

	0, 0, 0, 0, 0, 0, 13,14};


char keywords[10][2] = {"+", "-", "*", "/", "(", ")"};

char ID[20][12];

int C[20];

int num_ID = 0, num_C = 0;

struct token tok[100];                    //Token数组

int i_token=0,num_token=0;              //Token计数器和Token个数

char strTOKEN[15];                    //当前单词

int i_str;                              //当前单词指针

int n,p,m,e,t;                           //尾数值，指数值，小数位数，指数符号，类型

double num;                           //常数值

char w[50];                            //源程序缓冲区

int i;                                  //源程序缓冲区指针,当前字符为w[i]

struct map col1[4]={{"0123456789",1},{".",2},{"Ee",3},{"+-",4}};    //数字

struct map col2[2]={{"abcdefghijklmnopqrstuvwxyz",5},{"0123456789",1}}; //标识符

struct map col3[1]={{"()+-*/",6}};                    //界符

struct map *ptr;

int num_map;

int num_t = 0; //临时变量数量

struct QT{ //四元式
	token oper; // 运算符
	token va1; // 变量1
	token va2; //变量2
	token t; //运算结果

};

struct QT qt[20]; //四元式数组

int i_qt = 0, num_qt = 0; //四元式计数器和四元式个数

stack<token> SYN; //算符栈

stack<token> SEM; //语义栈

void act(int s);

int find(int s, char ch);

int InsertConst(double num);

int Reserve(char *str);

int InsertID(char *str);

int readword();

void printtoken();

void E();

void T();

void F();

void QUAT();

void printqt();

int main() {
	FILE *fp;
	int k;
	fp = fopen("equation.txt", "r");
	while (!feof(fp)) {
		fgets(w, 50, fp);
		if(feof(fp)) break;
		i = 0;
		do{
			k = readword();
			if (k != 1) break;

		} while(w[i]!=10);
	}
	fclose(fp);
	printtoken();
	i_token = 0;
	E();
	if (i_token != num_token) {
		printf("未结束的表达式\n");
		// printf("%d %d\n", i_token, num_token);
		exit(1);
	}
	printqt();
	return 0;
}

void act(int s)

{

	int value;

	switch (s)

	{

		case 1:n=0;m=0;p=0;t=0;e=1;num=0;i_str=0;

		       strTOKEN[i_str]='\0';                   //其它变量初始化

		       break;

		case 2:n=10*n+w[i]-48;

		       break;

		case 3:t=1;

		       break;

		case 4:n=10*n+w[i]-48; m++;

		       break;

		case 5:t=1;

		       break;

		case 6:if (w[i]=='-') e=-1;

			       break;

		case 7:p=10*p+w[i]-48;

		       break;

		case 8:strTOKEN[i_str++]=w[i];  //将ch中的符号拼接到strTOKEN的尾部；

		       break;

		case 9:strTOKEN[i_str++]=w[i];  //将ch中的符号拼接到strTOKEN的尾部；

		       break;

		case 10:strTOKEN[i_str++]=w[i]; //将ch中的符号拼接到strTOKEN的尾部；

			break;

		case 11:num=n*pow(10,e*p-m);           //计算常数值

			tok[i_token].code=2;  tok[i_token++].value=InsertConst(num);  //生成常数Token

			num_token++;

			break;

		case 12:strTOKEN[i_str]='\0';

			value=Reserve(strTOKEN);                   //查关键字表if (code)
			if (value)
			{ tok[i_token].code=0;  tok[i_token++].value=value; }   //生成关键字Token

			else

			{ tok[i_token].code=1;

				tok[i_token++].value=InsertID(strTOKEN); }    //生成标识符Token

			num_token++;

			break;

		case 13:strTOKEN[i_str]='\0';

			value=Reserve(strTOKEN);                   //查界符表

			tok[i_token].code=0;  tok[i_token++].value=value;    //生成界符Token

			num_token++;

			break;
		case 14:strTOKEN[i_str]='\0';

			value=Reserve(strTOKEN);                   //查界符表

			tok[i_token].code=0;  tok[i_token++].value=value;    //生成界符Token

			num_token++;

			break;


	}
}

int find(int s,char ch)
{
	int i,col=7;

	struct map *p;

	p=ptr;

	for (i=0;i<num_map;i++)

		if (strchr((p+i)->str,ch))

		{

			col=(p+i)->col;

			break;

		}

	return aut[s][col];

}

int InsertConst(double num)

{

	int i;

	for (i=0;i<num_C;i++)

		if (num==C[i])

			return i;

	C[i]=num;

	num_C++;

	return i;

}

int Reserve(char *str)

{

	int i;

	for (i=0;i<num_key;i++)

		if (!strcmp(keywords[i],str))

			return (i+1);

	return 0;

}

int InsertID(char *str)

{

	int i;

	for (i=0;i<num_ID;i++)

		if (!strcmp(ID[i],str))

			return i;

	strcpy(ID[i],str);

	num_ID++;

	return i;

}

int readword() {
	int s = 0;

	while (w[i]==' ')                       //滤空格

		i++;

	if (w[i]>='a' && w[i]<='z')                     //判定单词类别

	{

		ptr=col2;  num_map=2;

	}

	else if (w[i]>='0' && w[i]<='9')

	{

		ptr=col1;  num_map=4;

	}

	else if (strchr(col3[0].str,w[i])==NULL)

	{

		printf("非法字符%c\n",w[i]);

		i++;

		return -1;

	}

	else
	{

		ptr=col3;  num_map=1;
	}



	i--;

	s=1;                                   //开始处理一个单词

	while (s!=0)
	{

		act(s);

		if (s>=11 && s<=14)
		{
			return 1;

		}

		i++;                                  //getchar()
		// printf("%d %c\n", s, w[i]);
		s=find(s,w[i]);
		if (s==0)

		{

			strTOKEN[i_str]='\0';

			printf("词法错误：%s\n",strTOKEN);

			return 0;

		}
	}
}

void printtoken() {
	printf("关键字表：");                        //输出结果

	for (i=0;i<num_key;i++)

		printf("%s ",keywords[i]);

	printf("\n");

	printf("Token序列：");

	for (i=0;i<num_token;i++)

		printf("(%d,%d)",tok[i].code,tok[i].value);

	printf("\n");
	printf("翻译token序列：");
	for (i = 0; i < num_token; i++) {
		if (tok[i].code == 0) {
			printf("(关键字, %s)", keywords[tok[i].value - 1]);
		} else if(tok[i].code == 1) {
			printf("(变量, %s)", ID[tok[i].value]);
		} else if (tok[i].code == 2) {
			printf("(数字, %d)", C[tok[i].value]);
		}
	}
	printf("\n");
	printf("符号表：");

	for (i=0;i<num_ID;i++)

		printf("%s ",ID[i]);

	printf("\n");

	printf("常数表：");

	for (i=0;i<num_C;i++)

		printf("%d ",C[i]);

	printf("\n");

}

void QUAT() {
	struct token w;
	struct token a;
	struct token b;
	struct token t;
	if (SYN.empty()) return;
       	w.code = SYN.top().code;
	w.value = SYN.top().value;
	SYN.pop();
	if (SEM.empty()) return;
	b.code = SEM.top().code;
	b.value = SEM.top().value;
	SEM.pop();
	if (SEM.empty()) return;
	a.code = SEM.top().code;
	a.value	= SEM.top().value;
	SEM.pop();
	t.code = 3;
	t.value = num_t;
	SEM.push(t);
	num_t++;
	struct QT q;
	q.oper = w;
	q.va1 = a;
	q.va2 = b;
	q.t = t;
	qt[num_qt] = q;
	num_qt++;

}

void E() {
	T();
	while (tok[i_token].code == 0 && (tok[i_token].value == 1 || tok[i_token].value == 2)) {
		SYN.push(tok[i_token]);
		i_token++;
		T();
		QUAT();
	}
}

void T() {
	F();
	while (tok[i_token].code == 0 && (tok[i_token].value == 3 || tok[i_token].value == 4)) {
		SYN.push(tok[i_token]);
		i_token++;
		F();
		QUAT();
	}

}

void F() {
	if (tok[i_token].code == 0 && tok[i_token].value == 5) {
		i_token++;
		E();
		if (tok[i_token].code == 0 && tok[i_token].value != 6) {
			printf("未完结的括号\n");
			// printf("%d %s\n", i_token, keywords[tok[i_token].value-1]);
			exit(1);
		}
	} else {
		if(tok[i_token].code == 0) {
			printf("运算符号使用错误:");
			printf("%s\n", keywords[tok[i_token].value-1]);
			exit(1);
		}
		SEM.push(tok[i_token]);
	}
	i_token++;
}

void printqt() {
	printf("四元式token序列:\n");
	for (i = 0; i < num_qt; i++) {
		printf("((%d, %d), (%d, %d), (%d, %d), (%d, %d))\n", qt[i].oper.code, qt[i].oper.value, qt[i].va1.code, qt[i].va1.value, qt[i].va2.code, qt[i].va2.value, qt[i].t.code, qt[i].t.value);
	}

	printf("四元式序列:\n");
	for (i = 0; i < num_qt; i++) {
		printf("(");
		printf("%s, ", keywords[qt[i].oper.value-1]);
		if (qt[i].va1.code == 1)
			printf("%s, ", ID[qt[i].va1.value]);
		else if (qt[i].va1.code == 2)
			printf("%d, ", C[qt[i].va1.value]);
		else if (qt[i].va1.code == 3)
			printf("t%d, ", qt[i].va1.value);

		if (qt[i].va2.code == 1)
			printf("%s, ", ID[qt[i].va2.value]);
		else if (qt[i].va2.code == 2)
			printf("%d, ", C[qt[i].va2.value]);
		else if (qt[i].va2.code == 3)
			printf("t%d, ", qt[i].va2.value);

		printf("t%d)\n", qt[i].t.value);
	}
}
