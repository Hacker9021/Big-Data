#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cstring>

using namespace std;
using namespace std::chrono;

int value[100000000];   // 保存结果
int res[62] = {0};       // 打印蜂巢
int boolCount;           // 布尔变元数量
int clauseCount;         // 子句数量
char fileName[100];      // 文件名
double Time;

// 十字链表结构体
typedef struct SATNode {
    int data;             // 数据域
    SATNode* next;
} SATNode;
typedef struct SATList {
    SATNode* head;        // 表头
    SATList* next;
} SATList;
int chess[10][10];

// 函数声明
int ReadFile(const char* fileName, SATList*& cnf);
void destroyClause(SATList*& cnf);
int isUnitClause(SATNode* cnf);
int evaluateClause(SATNode* cnf, int v[]);
int removeClause(SATList*& cnf, SATList*& root);
int removeNode(SATNode*& cnf, SATNode*& head);
int addClause(SATList* cnf, SATList*& root);
int emptyClause(SATList* cnf);
int DPLL(SATList*& cnf, int value[], int Ans);
int better_DPLL(SATList*& cnf, int value[]);
void CopyClause(SATList*& a, SATList* b);
int WriteFile(int result, int value[]);
void CreateBinary();
void SolvePuzzle();
void drawHoneycomb();

int main(void) {
    SATList* CNFList = NULL, * lp;
    SATNode* tp;
    int op = 1, i, result;
    while (op) {
        system("cls");
        cout << "\n\n";
        cout << "        Function Menu \n";
        cout << "-------------------------------------------------\n";
        cout << "1. Read CNF File     2. Traverse and Output Clauses\n";
        cout << "3. Solve with DPLL and Save    4. Solve with improved DPLL and Save \n";
        cout << "5. Binary Sudoku Game          0. Exit\n";
        cout << "-------------------------------------------------\n";
        cout << "Select your operation [0~5]: ";
        cin >> op;

        switch (op) {
            case 1:
                cout << "Enter the CNF file to read: ";
                cin >> fileName;
                ReadFile(fileName, CNFList);
                getchar();
                getchar();
                break;
            case 2:
                if (CNFList == nullptr)
                    cout << "File not imported\n";
                else {
                    cout << "CNF clauses are as follows:\n";
                    for (lp = CNFList; lp != nullptr; lp = lp->next) {
                        for (tp = lp->head; tp != nullptr; tp = tp->next) {
                            cout << tp->data << " ";
                        }
                        cout << endl;
                    }
                }
                getchar();
                getchar();
                break;
            case 3:
                if (CNFList == nullptr)
                    cout << "File not imported\n";
                else {
                    for (int i = 1; i <= boolCount; i++)
                        value[i] = 1;  // Initialize, all set to 1
                    result = DPLL(CNFList, value, boolCount);
                    cout << "Solution result: " << result << endl;
                    if (result == 1) {
                        if (WriteFile(result, value) == 1)
                            cout << "Results saved to a file with the same name (.res)" << endl;
                        else
                            cout << "Failed to save results" << endl;
                    }
                }
                getchar();
                getchar();
                break;
            case 4:
                if (CNFList == nullptr)
                    cout << "File not imported\n";
                else {
                    for (int i = 1; i <= boolCount; i++)
                        value[i] = 1;  // Initialize, all set to 1
                    result = better_DPLL(CNFList, value);
                    cout << "Solution result: " << result << endl;
                    if (result == 1) {
                        if (WriteFile(result, value) == 1)
                            cout << "Results saved to a file with the same name (.res)" << endl;
                        else
                            cout << "Failed to save results" << endl;
                    }
                }
                getchar();
                getchar();
                break;
            case 5:
                CreateBinary();
                SolvePuzzle();
                drawHoneycomb();
                getchar();
                getchar();
                break;
            case 0:
                break;
        }
    }
    system("pause");
    return 0;
}



/*
 * 函数名称: ReadFile
 * 接受参数: SATList*&
 * 函数功能: 用文件指针fp打开用户指定的文件，并读取文件内容保存到给定参数中，读取成功返回1，失败返回0
 * 返回值: int
 */
int ReadFile(const char* fileName, SATList*& cnf) {
    FILE* fp = fopen(fileName, "r");
    if (!fp) {
        printf("File open failed!\n");
        return 0;
    }

    char ch;
    int number, i;
    SATList* lp;
    SATNode* tp;
    // 跳过以 'c' 开头的行
while ((ch = getc(fp)) == 'c') {
    while ((ch = getc(fp)) != '\n')
        continue; // 跳过整行
}

// 确保已经到达 'p cnf' 开头的行
if (ch!= 'p') {
    printf("Invalid file format!\n");
    fclose(fp);
    return 0;
}
getc(fp); getc(fp); getc(fp); getc(fp); 
// 读取布尔变元数量和子句数量
fscanf(fp, "%d %d", &boolCount, &clauseCount);
// 从这里开始读取 CNF 数据
// ... (后续的代码和逻辑)
    cnf = new SATList;
    cnf->next = nullptr;
    cnf->head = new SATNode;
    cnf->head->next = nullptr;
    lp = cnf;
    tp = cnf->head;

    for (i = 0; i < clauseCount; ++i, lp = lp->next, tp = lp->head) {
        fscanf(fp, "%d", &number);
        while(number != 0) {
            tp->data = number;
            fscanf(fp, "%d", &number);
            if (number == 0)
                tp->next = nullptr;
            else{
                tp->next = new SATNode;
                tp = tp->next;
            }
        }
        lp->next = new SATList;
        lp->next->head = new SATNode;
        if (i == clauseCount - 1) {
            lp->next = nullptr;
            break;
        }
    }

    fclose(fp);
    return 1;
}
/*
 * 函数名称: destroyClause
 * 接受参数: SATList*&
 * 函数功能: 销毁链表
 * 返回值: int
 */
void destroyClause(SATList*& cnf)
{
	SATList* lp1, * lp2;
	SATNode* tp1, * tp2;
	for (lp1 = cnf; lp1 != NULL; lp1 = lp2)
	{
		lp2 = lp1->next;
		for (tp1 = lp1->head; tp1 != NULL; tp1 = tp2)
		{
			tp2 = tp1->next;
			free(tp1);
		}
		free(lp1);
	}
	cnf = nullptr;
}
/*
 * 函数名称: isUnitClause
 * 接受参数: SATNode*
 * 函数功能: 判断是否为单子句，是返回1，不是返回0
 * 返回值: int
 */
int isUnitClause(SATNode* cnf)
{
	if (cnf != NULL && cnf->next == NULL)
		return 1;
	else
		return 0;
}
/*
 * 函数名称: evaluateClause
 * 接受参数: SATList*
 * 函数功能: 评估子句的真假状态，真返回1，假返回0
 * 返回值: int
 */
int evaluateClause(SATNode* cnf,int v[])
{
	SATNode* tp = cnf;
	while (tp != NULL)
	{
		if (tp->data > 0 && v[tp->data] == 1 ||
			tp->data < 0 && v[-tp->data] == 0)
			return 1;
	}
	return 0;
}

/*
 * 函数名称: removeClause
 * 接受参数: SATList*,SATList*
 * 函数功能: 在已有的十字链表中删除指定的子句，删除成功返回1，失败返回0
 * 返回值: int
 */
int removeClause(SATList*& cnf, SATList*& root)
{
	SATList* lp = root;
	if (lp == cnf) root = root->next;  //删除为头
	else
	{
		while (lp != nullptr && lp->next != cnf) lp = lp->next;
		lp->next = lp->next->next;
	}
	free(cnf);
	cnf = nullptr;
	return 1;
}

/*
 * 函数名称: removeNote
 * 接受参数: SATNode*,SATNode*
 * 函数功能: 在指定的子句中删除指定的文字，删除成功返回1，失败返回0
 * 返回值: int
 */
int removeNode(SATNode*& cnf, SATNode*& head)
{
	SATNode* lp = head;
	if (lp == cnf) head = head->next;  //删除为头
	else
	{
		while (lp != nullptr && lp->next != cnf) lp = lp->next;
		lp->next = lp->next->next;
	}
	free(cnf);
	cnf = NULL;
	return 1;
}

/*
 * 函数名称: addClause
 * 接受参数: SATList*,SATList*
 * 函数功能: 在已有的十字链表中添加指定的子句，添加成功返回1，失败返回0
 * 返回值: int
 */
int addClause(SATList* cnf, SATList*& root)
{
	//直接插入在表头
	if (cnf != NULL)
	{
		cnf->next = root;
		root = cnf;
		return 1;
	}
	return 0;
}
/*
 * 函数名称: emptyClause
 * 接受参数: SATList*
 * 函数功能: 判断是否含有空子句，是返回1，不是返回0
 * 返回值: int
 */
int emptyClause(SATList* cnf)
{
	SATList* lp = cnf;
	while (lp != nullptr)
	{
		if (lp->head == nullptr) return 1;
		lp = lp->next;
	}
	return 0;
}
/*
 * 函数名称: CopyClause
 * 接受参数: SATList*,SATList*
 * 函数功能: 将链表b的值复制到链表a中
 * 返回值: void
 */
void CopyClause(SATList*& a, SATList* b)
{
	SATList* lpa,*lpb;
	SATNode* tpa,*tpb;
	a = new SATList;
	a->head = new SATNode;
	a->next = nullptr;
	a->head->next = NULL;
	for (lpb = b, lpa = a; lpb != NULL; lpb = lpb->next, lpa = lpa->next)
	{
		for (tpb = lpb->head, tpa = lpa->head; tpb != NULL; tpb = tpb->next, tpa = tpa->next)
		{
			tpa->data = tpb->data;
			tpa->next = new SATNode;
			tpa->next->next = NULL;
			if (tpb->next == NULL)
			{
				free(tpa->next);
				tpa->next = NULL;
			}
		}
		lpa->next = new SATList;
		lpa->next->head = new SATNode;
		lpa->next->next = NULL;
		lpa->next->head->next = NULL;
		if (lpb->next == NULL)
		{
			free(lpa->next->head);
			free(lpa->next);
			lpa->next = nullptr;
		}
	}
}
/*
* 函数名称: DPLL
* 接受参数: SATList *
* 函数功能: 求解SAT问题，给出满足条件时的一个式子,若有解则返回1，无解返回0
* 返回值: int
*/
int better_DPLL(SATList*& cnf, int value[]) {
    auto start = high_resolution_clock::now();
    SATList* tp = cnf, * lp = cnf, * sp;
    SATNode* dp;
    int* count = new int[boolCount * 2 + 1];  // 记录每个文字出现次数的数组
    int re, MaxWord, max;

    // 查找并处理单子句
while (true) {
    while (tp != nullptr && isUnitClause(tp->head) == 0) {
        tp = tp->next;
    }
    if (tp == nullptr)
           break;
    else {
        // 单子句规则简化
        if (tp->head->data > 0) {
            value[tp->head->data] = 1;
        } else {
            value[-tp->head->data] = 0;
        }
        re = tp->head->data;

        for (lp = cnf; lp != nullptr; lp = sp) {
            sp = lp->next;

            // 查找含有核心文字的句子
            for (dp = lp->head; dp != nullptr; dp = dp->next) {
                if (dp->data == re) {
                    removeClause(lp, cnf);  // 删除子句，简化式子
                    break;
                }
                if (dp->data == -re) {
                    removeNode(dp, lp->head);  // 删除文字，简化子句
                    break;
                }
            }
        }
          tp = cnf; // 继续简化
    }
    // 极简化规则简化后
        if (cnf == nullptr) {
            delete[] count;
            return 1;  // 所有子句都满足
        } 
        else if (emptyClause(cnf)) {
            delete[] count;
            destroyClause(cnf);
            return 0;  // 出现空子句，不满足
        }

}
    for (int i = 0; i <= boolCount * 2; i++) {
        count[i] = 0;
    }

    // 计算子句中各文字出现次数
    for (lp = cnf; lp != nullptr; lp = lp->next) {
        for (dp = lp->head; dp != nullptr; dp = dp->next) {
            if (dp->data > 0) {
                count[dp->data]++;
            } else {
                count[boolCount - dp->data]++;
            }
        }
    }
    max = 0;

    // 找到出现次数最多的正文字
    for (int i = 2; i <= boolCount; i++) {
        if (max < count[i]) {
            max = count[i];
            MaxWord = i;
        }
    }

    if (max == 0) {
        // 若没有出现正文字，找到出现次数最多的负文字
        for (int i = boolCount + 1; i <= boolCount * 2; i++) {
            if (max < count[i]) {
                max = count[i];
                MaxWord = boolCount-i;
            }
        }
    }
    delete[] count;
    
    // 在一个分支中搜索
    SATList* lp1 = new SATList;
    lp1->head = new SATNode;
    lp1->head->data = MaxWord;
    lp1->head->next = nullptr;
    lp1->next = nullptr;
    CopyClause(tp, cnf);
    addClause(lp1, tp);
    if (better_DPLL(tp, value) == 1) {
        return 1;
    }
    destroyClause(tp);

    // 在另一个分支中搜索
    SATList* lp2 = new SATList;
    lp2->head = new SATNode;
    lp2->head->data = -MaxWord;
    lp2->head->next = nullptr;
    lp2->next = nullptr;
    addClause(lp2, cnf);
    re = better_DPLL(cnf, value);
    destroyClause(cnf);
    
    auto end = high_resolution_clock::now();
    duration<double> duration = end - start;
    Time = duration.count();
    return re;
}
/*
* 函数名称: 优化前的DPLL
* 接受参数: SATList *
* 函数功能: 求解SAT问题，给出满足条件时的一个式子,若有解则返回1，无解返回0
* 返回值: int
*/
int DPLL(SATList*& cnf, int value[], int Ans) {
    auto start = high_resolution_clock::now();
    SATList* tp = cnf, * lp = cnf, * sp;
    SATNode* dp;
    int* count = new int[boolCount * 2 + 1];  // 记录每个文字出现次数的数组
    int re, MaxWord, max;

    // 查找并处理单子句
while (true) {
    while (tp != nullptr && isUnitClause(tp->head) == 0) {
        tp = tp->next;
    }
    if (tp == nullptr)
           break;
    else {
        // 单子句规则简化
        if (tp->head->data > 0) {
            value[tp->head->data] = 1;  
        } else {
            value[-tp->head->data] = 0;
        }
        re = tp->head->data;

        for (lp = cnf; lp != nullptr; lp = sp) {
            sp = lp->next;

            // 查找含有核心文字的句子
            for (dp = lp->head; dp != nullptr; dp = dp->next) {
                if (dp->data == re) {
                    removeClause(lp, cnf);  // 删除子句，简化式子
                    break;
                }
                if (dp->data == -re) {
                    removeNode(dp, lp->head);  // 删除文字，简化子句
                    break;
                }
            }
        }
          tp = cnf; // 继续简化
    }
    // 极简化规则简化后
        if (cnf == nullptr) {
            delete[] count;
            return 1;  // 所有子句都满足
        } 
        else if (emptyClause(cnf)) {
            delete[] count;
            destroyClause(cnf);
            return 0;  // 出现空子句，不满足
        }

}
    for (int i = 0; i <= boolCount * 2; i++) {
        count[i] = 0;
    }
    if (Ans > 0)
         Ans = Ans / 2;
    // 在一个分支中搜索
    SATList* lp1 = new SATList;
    lp1->head = new SATNode;
    lp1->head->data = Ans;
    lp1->head->next = nullptr;
    lp1->next = nullptr;
    CopyClause(tp, cnf);
    addClause(lp1, tp);
    if (better_DPLL(tp, value) == 1) {
        return 1;
    }
    destroyClause(tp);

    // 在另一个分支中搜索
    SATList* lp2 = new SATList;
    lp2->head = new SATNode;
    lp2->head->data = -Ans;
    lp2->head->next = nullptr;
    lp2->next = nullptr;
    addClause(lp2, cnf);
    re = better_DPLL(cnf, value);
    destroyClause(cnf);
    
    auto end = high_resolution_clock::now();
    duration<double> duration = end - start;
    Time = duration.count();
    return re;
}
/*
* 函数名称: WriteFile
* 接受参数: int,int,int[]
* 函数功能: 将运行结果保存至同名文件，文件拓展名为.res,保存成功返回1，失败返回0
* 返回值: int
*/
int WriteFile(int result, int value[]) {
    FILE* fp;
    int i;

    for (i = 0; fileName[i] != '\0'; i++) {
        // 修改扩展名为 ".res"
        if (fileName[i] == '.' && fileName[i + 4] == '\0') {
            fileName[i + 1] = 'r';
            fileName[i + 2] = 'e';
            fileName[i + 3] = 's';
            break;
        }
    }

    fp = fopen(fileName, "w");
    if (!fp) {
        printf("Failed to open file for writing!\n");
        return 0;
    }

    fprintf(fp, "s %d\nv ", result);  // 求解结果
    if (result == 1) {
        // 保存解值
        for (i = 1; i <= boolCount; i++) {
            if (value[i] == 1) {
                fprintf(fp, "%d ", i);
            } else {
                fprintf(fp, "%d ", -i);
            }
        }
    }
    fprintf(fp, "\nt %lf", Time * 1000);  // 运行时间（毫秒）
    fclose(fp);
    
    return 1;
}
void CreateBinary()
{
      int op , k;
      FILE * fp;
      char ch;
      char inputfile [100];
      cout << "Please choose a file to initialize your puzzle : ";
      cin >> inputfile;
      strcpy (fileName ,inputfile);
      fileName[2]='9';
      fp = fopen (inputfile , "r");
      k = 1;
      while (fscanf (fp ,"%c", &ch) != EOF){
        if (ch != '\n')
            res [k++] = ch - '0';
      } 
      fclose (fp);
      cout << " Your Initial puzzle is : ";
      drawHoneycomb();
}
void SolvePuzzle( )
{
      const char * cnf_file = "limit.cnf";
      char ch;
      SATList* CNFList = nullptr;
      ReadFile (cnf_file, CNFList);
      FILE * fp = fopen (fileName , "r");
       for (int i = 1; i <= 9; i++)
            for (int j = 1; j <= 9; j++){
                fscanf (fp ,"%c", &ch );
                chess[i][j] = ch - '0';
            }
        fclose (fp);
      for (int i = 1; i <= 9; i++)
            for (int j = 1; j <= 9; j++){
                if (chess[i][j] > 0){
                     SATList* cnf = new SATList;
                     cnf->head = new SATNode;
                     cnf->next = nullptr;
                     cnf->head->data = i * 100 + j * 10 + chess[i][j];
                     cnf->head->next = nullptr;
                     addClause (cnf,CNFList);
                }
            }   // 添加单子句
       boolCount = 1000;
      
        if (better_DPLL (CNFList,value) == 1){
           cout << "Suceesful solve the puzzle! "<< endl << "The ultimate puzzle is :" << endl;
           for (int i = 1; i <= 9; i++)
               for (int j = 1; j <= 9; j++)
                  for (int l = 1; l <= 9; l++){
                      if (value [i * 100 + j * 10 + l] && l != 0)
                             chess[i][j] = l;
        }
            int count = 1;
             for (int i = 1; i <= 9; i++)
                for (int j = 1; j <= 9; j++){
                   if (chess[i][j])
                       res [count++] = chess[i][j];
                 }
        }
        else
          cout << "fail to solve the puzzle!" << endl;
}
void drawHoneycomb() {
    int rowcount = 19, colomncount_1 = 5, spacecount = 9, colomncount_2 = 6;
    int i, j, count = 1;
    cout << endl<<endl<<endl;
    count = 1;
    for (i = 1; i <= 10; i++, spacecount--) {
        for (int k = 1; k <= spacecount; k++)
            cout << " ";
        
        if (i % 2) {
            for (j = 1; j <= colomncount_1; j++) {
                if (j != colomncount_1)
                    cout << "/" << " " << "\\" << " ";
                else
                    cout << "/" << " " << "\\" << endl;
            }
            colomncount_1++;
        } else {
            for (j = 1; j <= colomncount_2; j++) {
                if (j != colomncount_2)
                    cout << "|" << " " << res [count ++] /*在这个位置填入数字*/<< " ";
                else
                    cout << "|" << endl;
            }
            colomncount_2++;
        }
    }
    
    spacecount = 1;
    colomncount_1 = 9, colomncount_2 = 9;
    
    for (i = 11; i <= 19; i++, spacecount++) {
        for (int k = 1; k <= spacecount; k++)
            cout << " ";
        
        if (i % 2) {
            for (j = 1; j <= colomncount_1; j++) {
                if (j != colomncount_1)
                    cout << "\\" << " " << "/" << " ";
                else
                    cout << "\\" << " " << "/" << endl;
            }
            colomncount_1--;
        } else {
            for (j = 1; j <= colomncount_2; j++) {
                if (j != colomncount_2)
                    cout << "|" << " " << res [count ++] << " ";
                else
                    cout << "|" << endl;
            }
            colomncount_2--;
        }
    }
}
