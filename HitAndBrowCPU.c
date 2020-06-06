#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// 最大数当て試行回数
#define MAXTURN 200
// 桁数
#define NUMLONG 4
// True False
#define TRUE 1
#define FALSE 0

typedef struct{
	int Hit;
	int Brow;
}t_HBresult;

// numがHit and Browで有効な数か検証
// ※4つの桁の数が異なるか
int checkHBNum(char *num)
{
	int i,j;
	// 検証ループ
	for(i=0;i<NUMLONG;i++){
		// ループ範囲注意
		for(j=i+1;j<NUMLONG;j++){
			if(num[i]==num[j])return FALSE;
		}
	}
	return TRUE;
}

// numAとnumBのHit and Brow判定
int judgeHB(char *numA, char *numB, t_HBresult *result)
{
	int i,j;
	result->Hit = 0,result->Brow = 0;
	for(i=0; i<NUMLONG; i++){
		for(j=0; j<NUMLONG; j++){
			if(i==j){
				// i==j:Hit判定
				if(numA[i]==numB[j])result->Hit++;
			}else{
				// i<>j:Brow判定
				if(numA[i]==numB[j])result->Brow++;
			}
		}
	}
	return TRUE;
}

// CPUが数を考える
void decideDraftA(char *myNumDraft){
	int myNumDraftInt,i;
	do{
		// 数の候補(0～10^NUMLONG)
		for(i=0;i<NUMLONG;i++){
			myNumDraft[i]=(rand()%10)+'0';
		}
		// 終端符号'\0'
		myNumDraft[NUMLONG]='\0';
		// 今考えた数
		printf("\tCPUが考えた数候補:%s %d\n",myNumDraft,checkHBNum(myNumDraft));
	}while(!checkHBNum(myNumDraft));
}

// CPUのターン
int CPUTurn(char *myNum){
	t_HBresult hbresult;
	char myNumDraft[NUMLONG+1];
	// CPUが数を考える
	decideDraftA(myNumDraft);
	printf("CPU:CPUが考えた数:%s\n",myNumDraft);
	// myNumとCPUが考えた数を比較
	judgeHB(myNum, myNumDraft, &hbresult);
	printf("CPU:%dHit %dBrow\n",hbresult.Hit, hbresult.Brow);
	// 全桁Hitで終了
	if(hbresult.Hit==NUMLONG)return TRUE;
	// そうでないならFalseを返して続行
	return FALSE;
}

int main(void){
	// Your code here!
	int loop,i;
	char myNum[NUMLONG+1];
	// 乱数初期化
	srand((unsigned int)time(NULL));

	// CPUに当ててほしい数を入れる
	printf("Hit and Brow CPUアルゴリズム検証\n");
	printf("自分の数を入力※4桁\n");
	scanf("%s",myNum);
	// 数チェック
	if(!checkHBNum(myNum)){
		printf("その数はHit and Browで使えません。\n");
		// 終了処理
		getchar();
		getchar();
		return 0;
	}
	printf("数当て開始\n");
	// CPUによる数当て開始
	for(i=0;i<MAXTURN;i++){
		printf("CPU:%d回目\n", i+1);
		if(CPUTurn(myNum)){
			// 数当て成功
			printf("CPU:%d回目で%dHit\n", i + 1, NUMLONG);
			// 終了処理
			getchar();
			getchar();
			return 0;
		}
		printf("CPU:%d回目\n", i+1);
	}
	// 数当て失敗
	printf("回数が上限に達したため終了\n");
	// 終了処理
	getchar();
	getchar();
	return 0;
}
