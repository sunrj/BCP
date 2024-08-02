#ifndef GRAPH
#define GRAPH

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <cmath>
#include <queue>
#include <iterator>
#include <bitset>
#include <math.h>
#include <unordered_map>
#include <map>
#include <stack>
#define NDEBUG
#include <assert.h>

#define TIMEOVER 43200

using namespace std;

typedef int I;

typedef vector<int> VI;
typedef vector<vector<int>> VVI;

typedef unordered_map<int, int> UMII;

VI operator+(VI &v1, VI &v2);
VI operator-(VI &v1, VI &v2);
VI operator*(VI &v1, VI &v2);

class Graph
{
public:
	double throwtime = 0;

    I OUT_Num = 0;

	I K, R;

	I n, m, pm, nm;

	I n_del, m_del;

	VI unit = {0};

	VI U;

	VVI adj;
	VVI p_adj;
	VVI n_adj;
	VVI non_adj;
	VI deg;
	VI p_deg;
	VI n_deg;
	VI cn_deg;
	VI exist;

	void dataInput(string graphname);

	UMII newid;
	UMII back_newid;
	void Pruning();
	void subPruning(I u);

    VI select_L, select_R;
    I process_vertex = 0;
    void OPT();    VI ans_L, ans_R;
    vector<bool> in_L, in_R;
    VVI cand_L, cand_R, excl_L, excl_R;
    VI in_cand_L, in_cand_R, in_excl_L, in_excl_R;
    VI negD;
    I mark_num = 0;
    VI mark;
    VI cnt;
    I cnt_num = 0;
    void subOPT_Swap(I d, I sL, I sR, I swap);
};

#endif // !GRAPH
