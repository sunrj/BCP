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
#include <unordered_set>
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
typedef unordered_set<int> USI;

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
	VI temp_save;
    I process_vertex = 0;
	I Lpart_size, Rpart_size;
    void OPT();
    void Pruning_PratL(I v, I &u, I &flag);
    void Pruning_PratR(I v, I &u, bool use_cn, I &flag);
    VI ans_L, ans_R;
    vector<bool> in_L, in_R;
    VVI cand_L, cand_R, excl_L, excl_R;
    VI in_cand_L, in_cand_R, in_excl_L, in_excl_R;
    VI negD;
    I mark_num = 0;
    VI mark;
    VI cnt;
    I cnt_num = 0;
    void subOPT_Swap(I d, I sL, I sR, I swap, I pivot_u, I pivot_from);
	void getDegree(I d, I sL, I sR, I &flag);
	void Pruning_L(I d, I v, I &flag);
	void Pruning_R(I d, I v, I &flag);
	void updateCand_LR(I d, I &temp_sL, I &temp_sR);
};

#endif // !GRAPH
