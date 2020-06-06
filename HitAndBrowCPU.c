#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// 最大数当て試行回数
#define MAXTURN 200
// 桁数(4～6桁)
#define NUMLONG 4
// True False
#define TRUE 1
#define FALSE 0

// Hit Brow構造体
typedef struct {
	int Hit;
	int Brow;
}t_HBresult;
// Log構造体
typedef struct {
	char Num[NUMLONG + 1];
	int Hit;
	int Brow;
}t_HBlog;

// AI用グローバル変数
// mode:1(完全ランダム),2(numList使用),3(numList+ログ使用)
int mode;
// numList:0～9の各数字が正解の数に含まれる可能性があるか(あれば1,なければ0)
int numList[10] = { 1,1,1,1,1,1,1,1,1,1 };
// numLog:CPUが出した数字とHit,Browのログ
t_HBlog numLog[MAXTURN];

// numがHit and Browで有効な数か検証
// ※4つの桁の数が異なるか
int checkHBNum(char *num)
{
	int i, j;
	// NUMLONG文字目は'\0'か
	if (num[NUMLONG] != '\0')return FALSE;
	// 検証ループ
	for (i = 0;i < NUMLONG;i++) {
		// 全桁数字か
		if (num[i] < '0' || '9' < num[i])return FALSE;
		// 4つの桁の数が異なるか
		// ループ範囲注意
		for (j = i + 1;j < NUMLONG;j++) {
			if (num[i] == num[j])return FALSE;
		}
	}
	return TRUE;
}

// numAとnumBのHit and Brow判定
// 4HitのみTrue 他はFalse
int judgeHB(char *numA, char *numB, t_HBresult *result)
{
	int i, j;
	result->Hit = 0, result->Brow = 0;
	for (i = 0; i < NUMLONG; i++) {
		for (j = 0; j < NUMLONG; j++) {
			if (i == j) {
				// i==j:Hit判定
				if (numA[i] == numB[j])result->Hit++;
			}
			else {
				// i<>j:Brow判定
				if (numA[i] == numB[j])result->Brow++;
			}
		}
	}
	if(result->Hit==4)return TRUE;
	return FALSE;
}

// 既に出した数と一致していないか検証
int checkLog(char *num, int turnNum)
{
	t_HBresult resultForCheck = { 0,0 };
	int i;
	for (i = 0;i < turnNum;i++) {
		// 過去に出した数と一致するか判定
		if (judgeHB(num, numLog[i].Num, &resultForCheck)) {
			return FALSE;
		}
	}
	return TRUE;
}

// 過去に出した数とhit and Browを判定し
// それが過去に出した数と当てる数とのhit and Browと一致するか検証
int checkLog2(char *num, int turnNum)
{
	t_HBresult resultForCheck = { 0,0 };
	int i;
	for (i = 0;i < turnNum;i++) {
		// 過去に出した数と一致するか判定
		if (judgeHB(num, numLog[i].Num, &resultForCheck)) {
			return FALSE;
		}
		// 過去に出した数とのHit and Brow判定
		if ((numLog[i].Hit != resultForCheck.Hit) || (numLog[i].Brow != resultForCheck.Brow)) {
			// HitかBrowどちらか不一致でやり直し
			return FALSE;
		}
	}
	return TRUE;
}

// 候補から外れた数字を使っていないか検証
int checkList(char *num)
{
	int i;
	for (i = 0;i < NUMLONG;i++) {
		if (numList[num[i] - '0'] == 0) {
			return FALSE;
		}
	}
	return TRUE;
}

// CPUが数を考える
void decideDraftA(char *myNumDraft, int turnNum)
{
	int myNumDraftInt, i, numOK;
	// numOK:TRUEならこの数を採用
	numOK = FALSE;
	do {
		// 数の候補(0～10^NUMLONG)
		for (i = 0;i < NUMLONG;i++) {
			myNumDraft[i] = (rand() % 10) + '0';
		}
		// 終端符号'\0'
		myNumDraft[NUMLONG] = '\0';
		// 今考えた数
		//printf("\tCPUが考えた数候補:%s", myNumDraft);
		// Hit and Browの数でいいかチェック
		if (!(checkHBNum(myNumDraft))) {
			//printf(" 不正な数\n");
			continue;
		}
		if (mode < 1)break;
		// 過去に出した数と重複がないか
		if (!(checkLog(myNumDraft, turnNum))) {
			//printf(" 今まで出した数と同じ\n");
			continue;
		}
		if (mode < 2)break;
		// 候補から外れた数字を使っていないか
		if (!(checkList(myNumDraft))) {
			//printf(" 候補から外れた数を使用\n");
			continue;
		}
		if (mode < 3)break;
		// 過去に出した数とhit and Browを判定し
		// それが過去に出した数と当てる数とのhit and Browと一致するか判定
		if (!(checkLog2(myNumDraft, turnNum))) {
			//printf(" 不採用\n");
			continue;
		}
		numOK = TRUE;
	} while (!numOK);
	printf(" この数を採用\n");
}

// CPUのターン
int CPUTurn(char *myNum, int turnNum)
{
	t_HBresult hbresult = { 0,0 };
	char myNumDraft[NUMLONG + 1];
	int i;

	// CPUが数を考える
	decideDraftA(myNumDraft, turnNum);
	printf("CPU:CPUが考えた数:%s\n", myNumDraft);

	// myNumとCPUが考えた数を比較
	judgeHB(myNum, myNumDraft, &hbresult);
	printf("CPU:%dHit %dBrow\n", hbresult.Hit, hbresult.Brow);

	// 全桁Hitで終了
	if (hbresult.Hit == NUMLONG)return TRUE;

	// そうでないならログを取る
	if (1 < mode) {
		// modeが1(完全ランダム)でない
		if (hbresult.Hit == 0 && hbresult.Brow == 0) {
			// 0hit0brow⇒今出した数に含まれる数字は、候補から外す
			for (i = 0;i < NUMLONG;i++) {
				numList[(int)(myNumDraft[i] - '0')] = 0;
			}
			printf("CPU「今出した数字は4つとも正解の候補ではないか…」\n");
		}
		else if (hbresult.Hit + hbresult.Brow == 4) {
			// 0hit0brow⇒今出した数に含まれる数字以外を候補から外す
			// 一度numListを全要素0にする
			for (i = 0;i < 10;i++) {
				numList[i] = 0;
			}
			// 今出した数に含まれる数字のみ1(候補)にする
			for (i = 0;i < NUMLONG;i++) {
				numList[myNumDraft[i] - '0'] = 1;
			}
			printf("CPU「今出した数字4つの順番を入れ替えれば正解だな!!」\n");
		}
		// ログを取る
		strcpy_s(numLog[turnNum].Num, NUMLONG + 1, myNumDraft);
		numLog[turnNum].Hit = hbresult.Hit;
		numLog[turnNum].Brow = hbresult.Brow;
		printf("CPU:ログを書いた[%s %dHit %dBrow]\n", numLog[turnNum].Num, numLog[turnNum].Hit, numLog[turnNum].Brow);
		// 現在のnumList
		printf("現在の候補数値リスト\n0,1,2,3,4,5,6,7,8,9,\n");
		for (i = 0;i < 10;i++) {
			printf("%d,", numList[i]);
		}
		printf("\n");
	}
	// Falseを返して続行
	return FALSE;
}

int main(void)
{
	// Your code here!
	int loop, i;
	char myNum[NUMLONG + 1];
	// 乱数初期化
	srand((unsigned int)time(NULL));

	// モード選択
	printf("CPUモード選択(0以下の数:完全ランダムのCPU、1:同じ数を2度出さない弱いCPU、2:普通のCPU、3以上の数：強いCPU)：");
	scanf("%d", &mode);

	// CPUに当ててほしい数を入れる
	printf("Hit and Brow CPUアルゴリズム検証\n");
	printf("自分の数を入力※%d桁\n", NUMLONG);
	scanf("%s", myNum);
	// 数チェック
	if (!checkHBNum(myNum)) {
		printf("その数はHit and Browで使えません。\n");
		// 終了処理
		getchar();
		getchar();
		return 0;
	}
	printf("数当て開始\n");
	// CPUによる数当て開始
	for (i = 0;i < MAXTURN;i++) {
		printf("CPU:%d回目\n", i + 1);
		if (CPUTurn(myNum, i)) {
			// 数当て成功
			printf("CPU:%d回目で%dHit\n", i + 1, NUMLONG);
			// 終了処理
			getchar();
			getchar();
			return 0;
		}
	}
	// 数当て失敗
	printf("回数が上限に達したため終了\n");
	// 終了処理
	getchar();
	getchar();
	return 0;
}
