#include "graph.h"

VI operator+(VI &v1, VI &v2)
{
	VI res(v1.size() + v2.size());
	int sz = set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), res.begin()) - res.begin();
	res.resize(sz);
	return res;
};

VI operator-(VI &v1, VI &v2)
{
	VI res(v1.size());
	int sz = set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), res.begin()) - res.begin();
	res.resize(sz);
	return res;
};

VI operator*(VI &v1, VI &v2)
{
	VI res(v1.size());
	int sz = set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), res.begin()) - res.begin();
	res.resize(sz);
	return res;
};


void Graph::dataInput(string graphname)
{
	ifstream infile;
	infile.open(graphname);
	infile >> n >> m >> pm >> nm;

	cout << "n: " << n << "  m: " << m << "  pm: " << pm << "  nm: " << nm << endl;

	U.resize(n);
	for (I i = 0; i < n; i++) U[i] = i;

	adj.resize(n);
	p_adj.resize(n);
	n_adj.resize(n);
	non_adj.resize(n);
	deg.resize(n, 0);
	p_deg.resize(n, 0);
	n_deg.resize(n, 0);
	cn_deg.resize(n, 0);
	exist.resize(n, 1);

	I u, v, sign;
	for (I i = 0; i < m; i++)
	{
		infile >> u >> v >> sign;
		adj[u].push_back(v);
		deg[u]++;
		adj[v].push_back(u);
		deg[v]++;
		if (sign == 1)
		{
			p_adj[u].push_back(v);
			p_deg[u]++;
			p_adj[v].push_back(u);
			p_deg[v]++;
		}
		else
		{
			n_adj[u].push_back(v);
			n_deg[u]++;
			n_adj[v].push_back(u);
			n_deg[v]++;
		}
	}

	infile.close();
}

void Graph::Pruning()
{
    for (auto u : U)
        if (exist[u])
            if (p_deg[u] < R-1 || n_deg[u] < R-K)
            {
                exist[u] = 0;
                subPruning(u);
            }

    I new_id = 0;
    for (auto u : U)
        if (exist[u])
        {
            newid[u] = new_id;
            back_newid[new_id] = u;
            new_id++;
        }

    I _m = 0, _pm = 0, _nm = 0;
    for (auto u : U)
        if (exist[u])
        {
            I p = 0, p1 = 0;
            for (auto v : adj[u])
                if (exist[v])
                    adj[u][p++] = newid[v];
            adj[u].resize(p);
            deg[u] = p;
            _m += p;
            p1 = p;

            p = 0;
            for (auto v : p_adj[u])
                if (exist[v])
                    p_adj[u][p++] = newid[v];
            p_adj[u].resize(p);
            p_deg[u] = p;
            _pm += p;
            p1 -= p;

            p = 0;
            for (auto v : n_adj[u])
                if (exist[v])
                    n_adj[u][p++] = newid[v];
            n_adj[u].resize(p);
            n_deg[u] = p;
            _nm += p;
            p1 -= p;

            assert(p1 == 0);
        }

    for (auto u : U)
        if (exist[u])
        {
            adj[newid[u]] = adj[u];
            p_adj[newid[u]] = p_adj[u];
            n_adj[newid[u]] = n_adj[u];
            deg[newid[u]] = deg[u];
            p_deg[newid[u]] = p_deg[u];
            n_deg[newid[u]] = n_deg[u];
            exist[newid[u]] = exist[u];
        }

    adj.resize(new_id);
    p_adj.resize(new_id);
    n_adj.resize(new_id);
    non_adj.resize(new_id);
    deg.resize(new_id);
    p_deg.resize(new_id);
    n_deg.resize(new_id);
    cn_deg.resize(new_id);
    exist.resize(new_id);

    U.resize(new_id);

    n_del = n - new_id;
    m_del = m - _m/2;
    assert(_m%2 == 0);
    n = new_id; m = _m/2; pm = _pm/2; nm = _nm/2;
}

void Graph::subPruning(I u)
{
    for (auto v : p_adj[u])
        if (exist[v])
        {
            p_deg[v]--;
            if (p_deg[v] < R-1)
            {
                exist[v] = 0;
                subPruning(v);
            }
        }
    for (auto v : n_adj[u])
        if (exist[v])
        {
            n_deg[v]--;
            if (n_deg[v] < R-K)
            {
                exist[v] = 0;
                subPruning(v);
            }
        }
}

void Graph::OPT()
{
    ans_L.resize(n, -1); ans_R.resize(n, -1);
    in_L.resize(n, false); in_R.resize(n, false);
    cand_L.resize(n); cand_R.resize(n);
    excl_L.resize(n); excl_R.resize(n);
    in_cand_L.resize(n, -1); in_cand_R.resize(n, -1);
    in_excl_L.resize(n, -1); in_excl_R.resize(n, -1);
    negD.resize(n, 0);
    cnt.resize(n, 0);

    select_L.resize(n, 0); select_R.resize(n, 0);
    temp_save.reserve(n);
    VI part_L, part_R, small_L, small_R;
    part_L.reserve(n); part_R.reserve(n); small_L.reserve(n); small_R.reserve(n);
    for (auto u : U)
    {
        part_L.clear(); part_R.clear(); small_L.clear(); small_R.clear();
        select_L[u] = 1;
        part_L.push_back(u);
        for (auto v : p_adj[u])
            if (v > u) {select_L[v] = 1; part_L.push_back(v);}
            else {select_L[v] = 1; small_L.push_back(v); p_deg[v] = 0; n_deg[v] = 0;}
        for (auto v : n_adj[u])
            if (v > u) {select_R[v] = 2; part_R.push_back(v);}
            else {select_R[v] = 1; small_R.push_back(v); p_deg[v] = 0; n_deg[v] = 0;}
        for (auto v : part_L)
            if (v > u)
                for (auto w : n_adj[v])
                {
                    if (w > u && select_L[w] == 0 && select_R[w] == 0) {select_R[w] = 1; part_R.push_back(w);}
                    else if (w < u && select_L[w] == 0 && select_R[w] == 0) {select_R[w] = 1; small_R.push_back(w); p_deg[w] = 0; n_deg[w] = 0;}
                }
        for (auto v : part_L)
        {
            p_deg[v] = 0; n_deg[v] = 0; cn_deg[v] = 0;
            for (auto w : p_adj[v])
                if (select_L[w]) {if (w >= u) p_deg[v]++; else p_deg[w]++;}
            for (auto w : n_adj[v])
            {
                if (select_R[w]) {if (w >= u) n_deg[v]++; else n_deg[w]++;}
                if (select_R[w] == 2) cn_deg[v]++;
            }
        }
        for (auto v : part_R)
        {
            p_deg[v] = 0; n_deg[v] = 0;
            for (auto w : p_adj[v])
                if (select_R[w]) {if (w >= u) p_deg[v]++; else p_deg[w]++;}
            for (auto w : n_adj[v])
                if (select_L[w]) {if (w >= u) n_deg[v]++; else n_deg[w]++;}
        }
        Lpart_size = part_L.size(); Rpart_size = part_R.size();
        I flag = 1;
        for (auto v : part_L)
            if (select_L[v])
                if (p_deg[v] < R-1 || n_deg[v] < R-K || cn_deg[v] < R-2*K)
                {
                    select_L[v] = 0;
                    Lpart_size--;
                    if (v == u) {flag = 0; break;}
                    Pruning_PratL(v, u, flag);
                    if (!flag) break;
                }
        if (flag)
        {
            for (auto v : part_R)
                if (select_R[v])
                    if (p_deg[v] < R-1 || n_deg[v] < R-K)
                    {
                        if (select_R[v] == 2)
                        {
                            select_R[v] = 0;
                            Rpart_size--;
                            Pruning_PratR(v, u, true, flag);
                            if (!flag) break;
                        }
                        else
                        {
                            select_R[v] = 0;
                            Rpart_size--;
                            Pruning_PratR(v, u, false, flag);
                            if (!flag) break;
                        }
                    }
        }
        if (flag)
        {
            process_vertex++;
            cand_L[0].clear(); cand_R[0].clear();
            excl_L[0].clear(); excl_R[0].clear();
            mark.clear(); mark.resize(n, 0); mark_num = 0;

            I sL = 0, sR = 0;
            ans_L[sL++] = u; in_L[u] = true;
            I flag_excl = 1;
            for (auto v : small_L)
                if (select_L[v]) 
                {
                    if (p_deg[v] == Lpart_size && n_deg[v] == Rpart_size) {flag_excl = 0; break;}
                    excl_L[0].push_back(v); in_excl_L[v] = 0;
                }
            if (flag_excl)
            {
                for (auto v : small_R)
                    if (select_R[v]) 
                    {
                        if (p_deg[v] == Rpart_size && n_deg[v] == Lpart_size) {flag_excl = 0; break;}
                        excl_R[0].push_back(v); in_excl_R[v] = 0;
                    }
                if (flag_excl)
                {
                    mark_num++;
                    for (auto v : n_adj[u]) if (select_R[v]) negD[v]++;
                    for (auto v : part_L)
                        if (select_L[v] && v > u) 
                        {
                            if (p_deg[v] == Lpart_size-1 && n_deg[v] == Rpart_size)
                            {
                                ans_L[sL++] = v; in_L[v] = true;
                                for (auto w : p_adj[v])
                                {
                                    if (in_excl_L[w] == 0)
                                    {
                                        if (mark[w] != mark_num && mark[w] != -1*mark_num) {mark[w] = mark_num; cnt[w] = 1;}
                                        else if (mark[w] == mark_num) cnt[w]++;
                                    }
                                    if (in_excl_R[w] == 0) mark[w] = -1*mark_num;
                                }
                                for (auto w : n_adj[v]) if (select_R[w]) negD[w]++;
                            } 
                            else {cand_L[0].push_back(v); in_cand_L[v] = 0;}
                        }
                    for (auto v : part_R)
                        if (select_R[v])
                        {
                            if (p_deg[v] == Rpart_size-1 && n_deg[v] == Lpart_size)
                            {
                                ans_R[sR++] = v; in_R[v] = true;
                                for (auto w : p_adj[v])
                                {
                                    if (in_excl_R[w] == 0)
                                    {
                                        if (mark[w] != mark_num && mark[w] != -1*mark_num) {mark[w] = mark_num; cnt[w] = 1;}
                                        else if (mark[w] == mark_num) cnt[w]++;
                                    }
                                    if (in_excl_L[w] == 0) mark[w] = -1*mark_num;
                                }
                                for (auto w : n_adj[v]) if (select_L[w]) negD[w]++;
                            } 
                            else {cand_R[0].push_back(v); in_cand_R[v] = 0;}
                        }
                    assert(Lpart_size == cand_L[0].size()+sL);
                    assert(Rpart_size == cand_R[0].size()+sR);
                    sort(cand_L[0].begin(), cand_L[0].end());
                    sort(cand_R[0].begin(), cand_R[0].end());
                    sort(excl_L[0].begin(), excl_L[0].end());
                    sort(excl_R[0].begin(), excl_R[0].end());
                    I pivot_u = -1, pivot_count = 0, pivot_from = 1;
                    if (sL != 1 || sR != 0)
                    {
                        temp_save.clear();
                        for (auto v : excl_L[0])
                        {
                            if (mark[v] == -1*mark_num || sR-negD[v] > K || (sL > 1 && (mark[v] != mark_num || cnt[v] != sL-1)))
                                {in_excl_L[v] = -1; select_L[v] = 0; negD[v] = 0;}
                            else {temp_save.push_back(v); if (p_deg[v]-sL+n_deg[v]-negD[v] > pivot_count) {pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-sL+n_deg[v]-negD[v];}}
                        }
                        excl_L[0] = temp_save; temp_save.clear();
                        for (auto v : excl_R[0])
                        {
                            if (mark[v] == -1*mark_num || sL-negD[v] > K || (sR > 0 && (mark[v] != mark_num || cnt[v] != sR)))
                                {in_excl_R[v] = -1; select_R[v] = 0; negD[v] = 0;}
                            else {temp_save.push_back(v); if (p_deg[v]-sR+n_deg[v]-negD[v] > pivot_count) {pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-sR+n_deg[v]-negD[v];}}
                        }
                        excl_R[0] = temp_save;
                    }
                    for (auto v : cand_L[0]) if (p_deg[v]-sL+n_deg[v]-negD[v] > pivot_count) {pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-sL+n_deg[v]-negD[v];}
                    for (auto v : cand_R[0]) if (p_deg[v]-sR+n_deg[v]-negD[v] > pivot_count) {pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-sR+n_deg[v]-negD[v];}
                    for (auto v : n_adj[u]) if (select_R[v] == 2) select_R[v] = 1;
                    subOPT_Swap(0, sL, sR, 0, pivot_u, pivot_from);
                    for (I i = 1; i < sL; i++)
                    {
                        in_L[ans_L[i]] = false;
                        for (auto v : n_adj[ans_L[i]]) if (select_R[v]) negD[v] = 0;
                    }
                    for (I i = 0; i < sR; i++)
                    {
                        in_R[ans_R[i]] = false;
                        for (auto v : n_adj[ans_R[i]]) if (select_L[v]) negD[v] = 0;
                    }
                    for (auto v : n_adj[u]) if (select_R[v]) negD[v] = 0;
                    for (auto v : cand_L[0]) {in_cand_L[v] = -1; select_L[v] = 0;}
                    for (auto v : cand_R[0]) {in_cand_R[v] = -1; select_R[v] = 0;}
                }
                for (auto v : excl_R[0]) {in_excl_R[v] = -1; select_R[v] = 0;}
            }
            for (auto v : excl_L[0]) {in_excl_L[v] = -1; select_L[v] = 0;}
            in_L[u] = false;
        }
        for (auto v : part_L) select_L[v] = 0;
        for (auto v : part_R) select_R[v] = 0;
        for (auto v : small_L) select_L[v] = 0;
        for (auto v : small_R) select_R[v] = 0;
    }
}

void Graph::Pruning_PratL(I v, I &u, I &flag)
{
    if (!flag) return;
    for (auto w : p_adj[v])
        if (select_L[w])
        {
            p_deg[w]--;
            if (p_deg[w] < R-1)
            {
                select_L[w] = 0;
                if (w < u) continue;
                Lpart_size--;
                if (w == u) {flag = 0; return;}
                Pruning_PratL(w, u, flag);
                if (!flag) return;
            }
        }
    for (auto w : n_adj[v])
        if (select_R[w])
        {
            n_deg[w]--;
            if (n_deg[w] < R-K)
            {
                if (select_R[w] == 2)
                {
                    select_R[w] = 0;
                    Rpart_size--;
                    Pruning_PratR(w, u, true, flag);
                    if (!flag) return;
                }
                else
                {
                    select_R[w] = 0;
                    if (w < u) continue;
                    Rpart_size--;
                    Pruning_PratR(w, u, false, flag);
                    if (!flag) return;
                }
            }
        }
}

void Graph::Pruning_PratR(I v, I &u, bool use_cn, I &flag)
{
    if (!flag) return;
    for (auto w : p_adj[v])
        if (select_R[w])
        {
            p_deg[w]--;
            if (p_deg[w] < R-1)
            {
                if (select_R[w] == 2)
                {
                    select_R[w] = 0;
                    Rpart_size--;
                    Pruning_PratR(w, u, true, flag);
                    if (!flag) return;
                }
                else
                {
                    select_R[w] = 0;
                    if (w < u) continue;
                    Rpart_size--;
                    Pruning_PratR(w, u, false, flag);
                    if (!flag) return;
                }
            }
        }
    for (auto w : n_adj[v])
        if (select_L[w])
        {
            n_deg[w]--;
            if (w < u) {if (n_deg[w] < R-K) select_L[w] = 0; continue;}
            if (use_cn) cn_deg[w]--;
            if (n_deg[w] < R-K || cn_deg[w] < R-2*K)
            {
                select_L[w] = 0;
                Lpart_size--;
                if (w == u) {flag = 0; return;}
                Pruning_PratL(w, u, flag);
                if (!flag) return;
            }
        }
}

void Graph::subOPT_Swap(I d, I sL, I sR, I swap, I pivot_u, I pivot_from)
{
    if (cand_L[d].size() == 0 && cand_R[d].size() == 0 && excl_L[d].size() == 0 && excl_R[d].size() == 0)
        if (sL >= R && sR >= R) 
        {
            OUT_Num++;
        }

    USI pivot_pass = {};
    if (pivot_u != -1)
    {
        mark_num++; cnt_num = 0;
        if (pivot_from == 1)
        {
            for (auto v : n_adj[pivot_u]) 
            {
                if (in_cand_R[v] == d) pivot_pass.insert(v);
                if (in_R[v]) mark[v] = mark_num;
            }
            for (I i = 0; i < sR; i++) 
                if (mark[ans_R[i]] != mark_num)
                {
                    cnt_num++;
                    for (auto v : n_adj[ans_R[i]]) 
                        if (in_cand_L[v] == d) 
                            {if (mark[v] != mark_num) {mark[v] = mark_num; cnt[v] = 1;} else cnt[v]++;}
                }
            for (auto v : p_adj[pivot_u]) if (in_cand_L[v] == d && (cnt_num == 0 || (mark[v] == mark_num && cnt[v] == cnt_num))) pivot_pass.insert(v);
        }
        else
        {
            for (auto v : n_adj[pivot_u]) 
            {
                if (in_cand_L[v] == d) pivot_pass.insert(v);
                if (in_L[v]) mark[v] = mark_num;
            }
            for (I i = 0; i < sL; i++) 
                if (mark[ans_L[i]] != mark_num)
                {
                    cnt_num++;
                    for (auto v : n_adj[ans_L[i]]) 
                        if (in_cand_R[v] == d) 
                            {if (mark[v] != mark_num) {mark[v] = mark_num; cnt[v] = 1;} else cnt[v]++;}
                }
            for (auto v : p_adj[pivot_u]) if (in_cand_R[v] == d && (cnt_num == 0 || (mark[v] == mark_num && cnt[v] == cnt_num))) pivot_pass.insert(v);
        }
    }

    if (swap)
    {
        for (auto u : cand_L[d])
        {
            if (pivot_u != -1 && pivot_pass.find(u) != pivot_pass.end()) continue;

            mark_num++;
            ans_L[sL] = u; in_L[u] = true;

            cand_L[d+1].clear(); cand_R[d+1].clear();
            excl_L[d+1].clear(); excl_R[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_R[v]) {negD[v]++; mark[v] = mark_num; assert(select_R[v] == 1); select_R[v] = 2;}

            I flag_R = 0;
            if (sR-negD[u] == K)
                flag_R = 1;
            cnt_num = 0;
            for (I i = 0; i < sR; i++)
            {
                I w = ans_R[i];
                if (mark[w] != mark_num && sL+1-negD[w] == K)
                {
                    cnt_num++;
                    for (auto v : n_adj[w])
                        if (select_L[v])
                        {
                            if (mark[v] != mark_num) {mark[v] = mark_num; cnt[v] = 1;}
                            else cnt[v]++;
                        }
                }
            }
            for (auto v : p_adj[u])
            {
                if (in_excl_L[v] == d)
                {
                    if (cnt_num == 0)
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }
                if (in_cand_L[v] == d)
                {
                    if (v > u || (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end()))
                    {
                        if (cnt_num == 0) 
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                    }
                    else if (v < u && (pivot_u == -1 || pivot_pass.find(v) == pivot_pass.end()))
                    {
                        if (cnt_num == 0)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                }
                if (in_cand_R[v] == d || in_excl_R[v] == d) {mark[v] = -1*mark_num;}
            }
            for (auto v : cand_R[d])
                if (sL+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_R)
                    {
                        if (mark[v] == mark_num)
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                    }
                    else
                        {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                }
            for (auto v : excl_R[d])
                if (sL+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_R)
                    {
                        if (mark[v] == mark_num)
                            {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                    else
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }
            
            mark_num++; Lpart_size = 0; Rpart_size = 0;
            I flag = 1;
            getDegree(d, sL+1, sR, flag);
            
            if (flag)
                for (auto v : cand_L[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K || cn_deg[v] < R-2*K))
                    {
                        mark[v] = -1*mark_num;
                        Lpart_size--;
                        Pruning_L(d, v, flag);
                        if (!flag) break;
                    }
            
            if (flag)
                for (auto v : cand_R[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K))
                    {
                        mark[v] = -1*mark_num;
                        Rpart_size--;
                        Pruning_R(d, v, flag);
                        if (!flag) break;
                    }
            
            if (flag)
                for (auto v : excl_L[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Lpart_size && n_deg[v] == Rpart_size) {flag = 0; break;}}
            if (flag)
                for (auto v : excl_R[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Rpart_size && n_deg[v] == Lpart_size) {flag = 0; break;}}
            I temp_sL = sL+1, temp_sR = sR;
            I pivot_count = 0, cur_pivot_u = -1;
            if (flag)
            {
                updateCand_LR(d, temp_sL, temp_sR);

                temp_save.clear();
                for (auto v : excl_L[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sR-negD[v] > K || (sL+1 != temp_sL && (mark[v] != mark_num || cnt[v] != temp_sL-sL-1)))
                        {if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}}
                }
                excl_L[d+1] = temp_save; temp_save.clear();

                for (auto v : excl_R[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sL-negD[v] > K || (sR != temp_sR && (mark[v] != mark_num || cnt[v] != temp_sR-sR)))
                        {in_excl_R[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}}
                }
                excl_R[d+1] = temp_save;
            }
            for (auto v : cand_L[d+1]) if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}
            for (auto v : cand_R[d+1]) if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}
            for (auto v : n_adj[u]) if (select_R[v] == 2) select_R[v] = 1;
            if (flag && temp_sL+cand_L[d+1].size() >= R && temp_sR+cand_R[d+1].size() >= R)
                subOPT_Swap(d+1, temp_sL, temp_sR, !swap, cur_pivot_u, pivot_from);

            if (flag)
            {
                for (I i = sL+1; i < temp_sL; i++)
                {
                    in_L[ans_L[i]] = false;
                    for (auto v : n_adj[ans_L[i]]) if (select_R[v]) negD[v]--;
                }
                for (I i = sR; i < temp_sR; i++)
                {
                    in_R[ans_R[i]] = false;
                    for (auto v : n_adj[ans_R[i]]) if (select_L[v]) negD[v]--;
                }
            }
            in_L[u] = false;
            for (auto v : n_adj[u]) if (select_R[v]) negD[v]--;
            for (auto v : cand_L[d+1]) in_cand_L[v] = d;
            for (auto v : cand_R[d+1]) in_cand_R[v] = d;
            for (auto v : excl_L[d+1]) if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;
            for (auto v : excl_R[d+1]) in_excl_R[v] = d;
        }

        for (auto u : cand_R[d])
        {
            if (pivot_u != -1 && pivot_pass.find(u) != pivot_pass.end()) continue;

            mark_num++;
            ans_R[sR] = u; in_R[u] = true;

            cand_R[d+1].clear(); cand_L[d+1].clear();
            excl_R[d+1].clear(); excl_L[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_L[v]) {negD[v]++; mark[v] = mark_num; assert(select_L[v] == 1); select_L[v] = 2;}

            I flag_L = 0;
            if (sL-negD[u] == K)
                flag_L = 1;
            cnt_num = 0;
            for (I i = 0; i < sL; i++)
            {
                I w = ans_L[i];
                if (mark[w] != mark_num && sR+1-negD[w] == K)
                {
                    cnt_num++;
                    for (auto v : n_adj[w])
                        if (select_R[v])
                        {
                            if (mark[v] != mark_num) {mark[v] = mark_num; cnt[v] = 1;}
                            else cnt[v]++;
                        }
                }
            }
            
            for (auto v : p_adj[u])
            {
                if (in_excl_R[v] == d)
                {
                    if (cnt_num == 0)
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }
                if (in_cand_R[v] == d)
                {
                    if (v > u || (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end()))
                    {
                        if (cnt_num == 0) 
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                    }
                    else if (v < u && (pivot_u == -1 || pivot_pass.find(v) == pivot_pass.end()))
                    {
                        if (cnt_num == 0)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                }
                if (in_cand_L[v] == d || in_excl_L[v] == d) {mark[v] = -1*mark_num;}
            }
            for (auto v : excl_L[d])
                if (sR+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_L)
                    {
                        if (mark[v] == mark_num)
                            {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                    else
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }
            for (auto v : cand_L[d])
                if (sR+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_L)
                    {
                        if (mark[v] == mark_num)
                        {
                            if (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end())
                                {cand_L[d+1].push_back(v), in_cand_L[v] = d+1;}
                            else
                                {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                        }
                    }
                    else
                    {
                        if (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end())
                            {cand_L[d+1].push_back(v), in_cand_L[v] = d+1;}
                        else
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                }
            
            mark_num++; Lpart_size = 0; Rpart_size = 0;
            I flag = 1;
            getDegree(d, sL, sR+1, flag);
            if (flag)
                for (auto v : cand_R[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K || cn_deg[v] < R-2*K))
                    {
                        mark[v] = -1*mark_num;
                        Rpart_size--;
                        Pruning_R(d, v, flag);
                        if (!flag) break;
                    }
            if (flag)
                for (auto v : cand_L[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K))
                    {
                        mark[v] = -1*mark_num;
                        Lpart_size--;
                        Pruning_L(d, v, flag);
                        if (!flag) break;
                    }
            if (flag)
                for (auto v : excl_L[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Lpart_size && n_deg[v] == Rpart_size) {flag = 0; break;}}
            if (flag)
                for (auto v : excl_R[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Rpart_size && n_deg[v] == Lpart_size) {flag = 0; break;}}
            I temp_sL = sL, temp_sR = sR+1;
            I pivot_count = 0, cur_pivot_u = -1;
            if (flag)
            {
                updateCand_LR(d, temp_sL, temp_sR);

                temp_save.clear();
                for (auto v : excl_L[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sR-negD[v] > K || (sL != temp_sL && (mark[v] != mark_num || cnt[v] != temp_sL-sL)))
                        {if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}}
                }
                excl_L[d+1] = temp_save; temp_save.clear();

                for (auto v : excl_R[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sL-negD[v] > K || (sR+1 != temp_sR && (mark[v] != mark_num || cnt[v] != temp_sR-sR-1)))
                        {if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}}
                }
                excl_R[d+1] = temp_save;
            }
            for (auto v : cand_L[d+1]) if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}
            for (auto v : cand_R[d+1]) if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}
            for (auto v : n_adj[u]) if (select_L[v] == 2) select_L[v] = 1;
            if (flag && temp_sL+cand_L[d+1].size() >= R && temp_sR+cand_R[d+1].size() >= R)
                subOPT_Swap(d+1, temp_sL, temp_sR, !swap, cur_pivot_u, pivot_from);

            if (flag)
            {
                for (I i = sL; i < temp_sL; i++)
                {
                    in_L[ans_L[i]] = false;
                    for (auto v : n_adj[ans_L[i]]) if (select_R[v]) negD[v]--;
                }
                for (I i = sR+1; i < temp_sR; i++)
                {
                    in_R[ans_R[i]] = false;
                    for (auto v : n_adj[ans_R[i]]) if (select_L[v]) negD[v]--;
                }
            }
            in_R[u] = false;
            for (auto v : n_adj[u]) if (select_L[v]) negD[v]--;
            for (auto v : cand_R[d+1]) in_cand_R[v] = d;
            for (auto v : cand_L[d+1]) in_cand_L[v] = d;
            for (auto v : excl_R[d+1]) if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;
            for (auto v : excl_L[d+1]) if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;
        }
    }
    else
    {
        for (auto u : cand_R[d])
        {
            if (pivot_u != -1 && pivot_pass.find(u) != pivot_pass.end()) continue;

            mark_num++;
            ans_R[sR] = u; in_R[u] = true;

            cand_R[d+1].clear(); cand_L[d+1].clear();
            excl_R[d+1].clear(); excl_L[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_L[v]) {negD[v]++; mark[v] = mark_num; assert(select_L[v] == 1); select_L[v] = 2;}

            I flag_L = 0;
            if (sL-negD[u] == K)
                flag_L = 1;
            cnt_num = 0;
            for (I i = 0; i < sL; i++)
            {
                I w = ans_L[i];
                if (mark[w] != mark_num && sR+1-negD[w] == K)
                {
                    cnt_num++;
                    for (auto v : n_adj[w])
                        if (select_R[v])
                        {
                            if (mark[v] != mark_num) {mark[v] = mark_num; cnt[v] = 1;}
                            else cnt[v]++;
                        }
                }
            }
            for (auto v : p_adj[u])
            {
                if (in_excl_R[v] == d)
                {
                    if (cnt_num == 0)
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }
                if (in_cand_R[v] == d)
                {
                    if (v > u || (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end()))
                    {
                        if (cnt_num == 0) 
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                    }
                    else if (v < u && (pivot_u == -1 || pivot_pass.find(v) == pivot_pass.end()))
                    {
                        if (cnt_num == 0)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                }
                if (in_cand_L[v] == d || in_excl_L[v] == d) {mark[v] = -1*mark_num;}
            }
            for (auto v : cand_L[d])
                if (sR+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_L)
                    {
                        if (mark[v] == mark_num)
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                    }
                    else
                        {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                }
            for (auto v : excl_L[d])
                if (sR+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_L)
                    {
                        if (mark[v] == mark_num)
                            {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                    else
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }

            mark_num++; Lpart_size = 0; Rpart_size = 0;
            I flag = 1;
            getDegree(d, sL, sR+1, flag);
            if (flag)
                for (auto v : cand_R[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K || cn_deg[v] < R-2*K))
                    {
                        mark[v] = -1*mark_num;
                        Rpart_size--;
                        Pruning_R(d, v, flag);
                        if (!flag) break;
                    }
            if (flag)
                for (auto v : cand_L[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K))
                    {
                        mark[v] = -1*mark_num;
                        Lpart_size--;
                        Pruning_L(d, v, flag);
                        if (!flag) break;
                    }
            if (flag)
                for (auto v : excl_R[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Rpart_size && n_deg[v] == Lpart_size) {flag = 0; break;}}
            if (flag)
                for (auto v : excl_L[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Lpart_size && n_deg[v] == Rpart_size) {flag = 0; break;}}
            I temp_sL = sL, temp_sR = sR+1;
            I pivot_count = 0, cur_pivot_u = -1;
            if (flag)
            {
                updateCand_LR(d, temp_sL, temp_sR);

                temp_save.clear();
                for (auto v : excl_R[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sL-negD[v] > K || (sR+1 != temp_sR && (mark[v] != mark_num || cnt[v] != temp_sR-sR-1)))
                        {if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}}
                }
                excl_R[d+1] = temp_save; temp_save.clear();

                for (auto v : excl_L[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sR-negD[v] > K || (sL != temp_sL && (mark[v] != mark_num || cnt[v] != temp_sL-sL)))
                        {in_excl_L[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}}
                }
                excl_L[d+1] = temp_save;
            }
            for (auto v : cand_L[d+1]) if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}
            for (auto v : cand_R[d+1]) if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}
            for (auto v : n_adj[u]) if (select_L[v] == 2) select_L[v] = 1;
            if (flag && temp_sL+cand_L[d+1].size() >= R && temp_sR+cand_R[d+1].size() >= R)
                subOPT_Swap(d+1, temp_sL, temp_sR, !swap, cur_pivot_u, pivot_from);

            if (flag)
            {
                for (I i = sR+1; i < temp_sR; i++)
                {
                    in_R[ans_R[i]] = false;
                    for (auto v : n_adj[ans_R[i]]) if (select_L[v]) negD[v]--;
                }
                for (I i = sL; i < temp_sL; i++)
                {
                    in_L[ans_L[i]] = false;
                    for (auto v : n_adj[ans_L[i]]) if (select_R[v]) negD[v]--;
                }
            }
            in_R[u] = false;
            for (auto v : n_adj[u]) if (select_L[v]) negD[v]--;
            for (auto v : cand_R[d+1]) in_cand_R[v] = d;
            for (auto v : cand_L[d+1]) in_cand_L[v] = d;
            for (auto v : excl_R[d+1]) if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;
            for (auto v : excl_L[d+1]) in_excl_L[v] = d;
        }

        for (auto u : cand_L[d])
        {
            if (pivot_u != -1 && pivot_pass.find(u) != pivot_pass.end()) continue;

            mark_num++;
            ans_L[sL] = u; in_L[u] = true;

            cand_L[d+1].clear(); cand_R[d+1].clear();
            excl_L[d+1].clear(); excl_R[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_R[v]) {negD[v]++; mark[v] = mark_num; assert(select_R[v] == 1); select_R[v] = 2;}

            I flag_R = 0;
            if (sR-negD[u] == K)
                flag_R = 1;
            cnt_num = 0;
            for (I i = 0; i < sR; i++)
            {
                I w = ans_R[i];
                if (mark[w] != mark_num && sL+1-negD[w] == K)
                {
                    cnt_num++;
                    for (auto v : n_adj[w])
                        if (select_L[v])
                        {
                            if (mark[v] != mark_num) {mark[v] = mark_num; cnt[v] = 1;}
                            else cnt[v]++;
                        }
                }
            }
            for (auto v : p_adj[u])
            {
                if (in_excl_L[v] == d)
                {
                    if (cnt_num == 0)
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }
                if (in_cand_L[v] == d)
                {
                    if (v > u || (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end()))
                    {
                        if (cnt_num == 0) 
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                    }
                    else if (v < u && (pivot_u == -1 || pivot_pass.find(v) == pivot_pass.end()))
                    {
                        if (cnt_num == 0)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                }
                if (in_cand_R[v] == d || in_excl_R[v] == d) {mark[v] = -1*mark_num;}
            }
            for (auto v : excl_R[d])
                if (sL+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_R)
                    {
                        if (mark[v] == mark_num)
                            {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                    else
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                }
            for (auto v : cand_R[d])
                if (sL+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_R)
                    {
                        if (mark[v] == mark_num)
                        {
                            if (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end())
                                {cand_R[d+1].push_back(v), in_cand_R[v] = d+1;}
                            else
                                {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                        }
                    }
                    else
                    {
                        if (pivot_u != -1 && pivot_pass.find(v) != pivot_pass.end())
                            {cand_R[d+1].push_back(v), in_cand_R[v] = d+1;}
                        else
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1; p_deg[v] = 0; n_deg[v] = 0;}
                    }
                }
            mark_num++; Lpart_size = 0; Rpart_size = 0;
            I flag = 1;
            getDegree(d, sL+1, sR, flag);
            if (flag)
                for (auto v : cand_L[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K || cn_deg[v] < R-2*K))
                    {
                        mark[v] = -1*mark_num;
                        Lpart_size--;
                        Pruning_L(d, v, flag);
                        if (!flag) break;
                    }
            if (flag)
                for (auto v : cand_R[d+1])
                    if (mark[v] == mark_num && (p_deg[v] < R-1 || n_deg[v] < R-K))
                    {
                        mark[v] = -1*mark_num;
                        Rpart_size--;
                        Pruning_R(d, v, flag);
                        if (!flag) break;
                    }
            
            if (flag)
                for (auto v : excl_L[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Lpart_size && n_deg[v] == Rpart_size) {flag = 0; break;}}
            if (flag)
                for (auto v : excl_R[d+1])
                    {cnt[v] = 0; if (mark[v] == mark_num && p_deg[v] == Rpart_size && n_deg[v] == Lpart_size) {flag = 0; break;}}
            I temp_sL = sL+1, temp_sR = sR;
            I pivot_count = 0, cur_pivot_u = -1;
            if (flag)
            {
                updateCand_LR(d, temp_sL, temp_sR);

                temp_save.clear();
                for (auto v : excl_L[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sR-negD[v] > K || (sL+1 != temp_sL && (mark[v] != mark_num || cnt[v] != temp_sL-sL-1)))
                        {if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}}
                }
                excl_L[d+1] = temp_save; temp_save.clear();

                for (auto v : excl_R[d+1])
                {
                    if (mark[v] == -1*mark_num || temp_sL-negD[v] > K || (sR != temp_sR && (mark[v] != mark_num || cnt[v] != temp_sR-sR)))
                        {if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;}
                    else {temp_save.push_back(v); if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}}
                }
                excl_R[d+1] = temp_save;
            }

            for (auto v : cand_L[d+1]) if (p_deg[v]-temp_sL+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = 1; pivot_count = p_deg[v]-temp_sL+n_deg[v]-negD[v];}
            for (auto v : cand_R[d+1]) if (p_deg[v]-temp_sR+n_deg[v]-negD[v] > pivot_count) {cur_pivot_u = v; pivot_from = -1; pivot_count = p_deg[v]-temp_sR+n_deg[v]-negD[v];}

            for (auto v : n_adj[u]) if (select_R[v] == 2) select_R[v] = 1;
            if (flag && temp_sL+cand_L[d+1].size() >= R && temp_sR+cand_R[d+1].size() >= R)
                subOPT_Swap(d+1, temp_sL, temp_sR, !swap, cur_pivot_u, pivot_from);

            if (flag)
            {
                for (I i = sL+1; i < temp_sL; i++)
                {
                    in_L[ans_L[i]] = false;
                    for (auto v : n_adj[ans_L[i]]) if (select_R[v]) negD[v]--;
                }
                for (I i = sR; i < temp_sR; i++)
                {
                    in_R[ans_R[i]] = false;
                    for (auto v : n_adj[ans_R[i]]) if (select_L[v]) negD[v]--;
                }
            }
            in_L[u] = false;
            for (auto v : n_adj[u]) if (select_R[v]) negD[v]--;
            for (auto v : cand_L[d+1]) in_cand_L[v] = d;
            for (auto v : cand_R[d+1]) in_cand_R[v] = d;
            for (auto v : excl_L[d+1]) if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;
            for (auto v : excl_R[d+1]) if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;
        }
    }
}

void Graph::getDegree(I d, I sL, I sR, I &flag)
{
    for (I i = 0; i < sL; i++)
    {
        I v = ans_L[i]; p_deg[v] = 0; n_deg[v] = 0; mark[v] = mark_num; Lpart_size++;
        for (auto w : p_adj[v])
        {
            if (in_L[w] || in_cand_L[w] == d+1) p_deg[v]++;
            else if (in_excl_L[w] == d+1) {p_deg[w]++; mark[w] = mark_num;}
        }
        if (p_deg[v] < R-1) {flag = 0; return;}
        for (auto w : n_adj[v])
        {
            if (in_R[w] || in_cand_R[w] == d+1) n_deg[v]++;
            else if (in_excl_R[w] == d+1) {n_deg[w]++; mark[w] = mark_num;}
        }
        if (n_deg[v] < R-K) {flag = 0; return;}
    }

    for (I i = 0; i < sR; i++)
    {
        I v = ans_R[i]; p_deg[v] = 0; n_deg[v] = 0; mark[v] = mark_num; Rpart_size++;
        for (auto w : p_adj[v])
        {
            if (in_R[w] || in_cand_R[w] == d+1) p_deg[v]++;
            else if (in_excl_R[w] == d+1) {p_deg[w]++; mark[w] = mark_num;}
        }
        if (p_deg[v] < R-1) {flag = 0; return;}
        for (auto w : n_adj[v])
        {
            if (in_L[w] || in_cand_L[w] == d+1) n_deg[v]++;
            else if (in_excl_L[w] == d+1) {n_deg[w]++; mark[w] = mark_num;}
        }
        if (n_deg[v] < R-K) {flag = 0; return;}
    }

    for (auto v : cand_L[d+1])
    {
        p_deg[v] = 0; n_deg[v] = 0; cn_deg[v] = 0; mark[v] = mark_num; Lpart_size++;
        for (auto w : p_adj[v])
        {
            if (in_L[w] || in_cand_L[w] == d+1) p_deg[v]++;
            else if (in_excl_L[w] == d+1) {p_deg[w]++; mark[w] = mark_num;}
        }
        for (auto w : n_adj[v])
        {
            if (in_R[w] || in_cand_R[w] == d+1) 
                {n_deg[v]++; if (select_R[w] == 2) cn_deg[v]++;}
            else if (in_excl_R[w] == d+1) {n_deg[w]++; mark[w] = mark_num;}
        }
    }
    
    for (auto v : cand_R[d+1])
    {
        p_deg[v] = 0; n_deg[v] = 0; cn_deg[v] = 0; mark[v] = mark_num; Rpart_size++;
        for (auto w : p_adj[v])
        {
            if (in_R[w] || in_cand_R[w] == d+1) p_deg[v]++;
            else if (in_excl_R[w] == d+1) {p_deg[w]++; mark[w] = mark_num;}
        }
        for (auto w : n_adj[v])
        {
            if (in_L[w] || in_cand_L[w] == d+1) 
                {n_deg[v]++; if (select_L[w] == 2) cn_deg[v]++;}
            else if (in_excl_L[w] == d+1) {n_deg[w]++; mark[w] = mark_num;}
        }
    }
}

void Graph::Pruning_L(I d, I v, I &flag)
{
    if (!flag) return;
    for (auto w : p_adj[v])
        if (mark[w] == mark_num && (in_cand_L[w] == d+1 || in_excl_L[w] == d+1 || in_L[w]))
        {
            p_deg[w]--;
            if (p_deg[w] < R-1)
            {
                mark[w] = -1*mark_num;
                if (in_excl_L[w] == d+1) continue;
                Lpart_size--;
                if (in_L[w]) {flag = 0; return;}
                Pruning_L(d, w, flag);
                if (!flag) return;
            }
        }
    if (select_L[v] == 2)
    {
        for (auto w : n_adj[v])
            if (mark[w] == mark_num)
            {
                if (in_excl_R[w] == d+1 || in_R[w])
                {
                    n_deg[w]--; 
                    if (n_deg[w] < R-K) 
                    {
                        if (in_excl_R[w] == d+1) {mark[w] = -1*mark_num; continue;}
                        if (in_R[w]) {flag = 0; return;}
                    }
                }
                else if (in_cand_R[w] == d+1)
                {
                    n_deg[w]--; cn_deg[w]--;
                    if (n_deg[w] < R-K || cn_deg[w] < R-2*K)
                    {
                        mark[w] = -1*mark_num;
                        Rpart_size--;
                        Pruning_R(d, w, flag);
                        if (!flag) return;
                    }
                }
            }
    }
    else
    {
        for (auto w : n_adj[v])
            if (mark[w] == mark_num && (in_cand_R[w] == d+1 || in_excl_R[w] == d+1 || in_R[w]))
            {
                n_deg[w]--;
                if (n_deg[w] < R-K)
                {
                    mark[w] = -1*mark_num;
                    if (in_excl_R[w] == d+1) continue;
                    Rpart_size--;
                    if (in_R[w]) {flag = 0; return;}
                    Pruning_R(d, w, flag);
                    if (!flag) return;
                }
            }
    }
}

void Graph::Pruning_R(I d, I v, I &flag)
{
    if (!flag) return;
    for (auto w : p_adj[v])
        if (mark[w] == mark_num && (in_cand_R[w] == d+1 || in_excl_R[w] == d+1 || in_R[w]))
        {
            p_deg[w]--;
            if (p_deg[w] < R-1)
            {
                mark[w] = -1*mark_num;
                if (in_excl_R[w] == d+1) continue;
                Rpart_size--;
                if (in_R[w]) {flag = 0; return;}
                Pruning_R(d, w, flag);
                if (!flag) return;
            }
        }
    if (select_R[v] == 2)
    {
        for (auto w : n_adj[v])
            if (mark[w] == mark_num)
            {
                if (in_excl_L[w] == d+1 || in_L[w])
                {
                    n_deg[w]--;
                    if (n_deg[w] < R-K)
                    {
                        if (in_excl_L[w] == d+1) {mark[w] = -1*mark_num; continue;}
                        if (in_L[w]) {flag = 0; return;}
                    }
                }
                else if (in_cand_L[w] == d+1)
                {
                    n_deg[w]--; cn_deg[w]--;
                    if (n_deg[w] < R-K || cn_deg[w] < R-2*K)
                    {
                        mark[w] = -1*mark_num;
                        Lpart_size--;
                        Pruning_L(d, w, flag);
                        if (!flag) return;
                    }
                }
            }
    }
    else
    {
        for (auto w : n_adj[v])
            if (mark[w] == mark_num && (in_cand_L[w] == d+1 || in_excl_L[w] == d+1 || in_L[w]))
            {
                n_deg[w]--;
                if (n_deg[w] < R-K)
                {
                    mark[w] = -1*mark_num;
                    if (in_excl_L[w] == d+1) continue;
                    Lpart_size--;
                    if (in_L[w]) {flag = 0; return;}
                    Pruning_L(d, w, flag);
                    if (!flag) return;
                }
            }
    }
}

void Graph::updateCand_LR(I d, I &temp_sL, I &temp_sR)
{
    temp_save.clear();
    for (auto v : cand_L[d+1])
    {
        if (mark[v] == -1*mark_num) {in_cand_L[v] = d;}
        else if (p_deg[v] == Lpart_size-1 && n_deg[v] == Rpart_size)
        {
            ans_L[temp_sL++] = v; in_L[v] = true; in_cand_L[v] = d;
            for (auto w : p_adj[v])
            {
                if (in_excl_L[w] == d+1) cnt[w]++;
                if (in_excl_R[w] == d+1) mark[w] = -1*mark_num;
            }
            for (auto w : n_adj[v]) if (select_R[w]) negD[w]++;
        }
        else temp_save.push_back(v);
    }
    cand_L[d+1] = temp_save; temp_save.clear();
    assert(Lpart_size == temp_sL+cand_L[d+1].size());

    for (auto v : cand_R[d+1])
    {
        if (mark[v] == -1*mark_num) {in_cand_R[v] = d;}
        else if (p_deg[v] == Rpart_size-1 && n_deg[v] == Lpart_size)
        {
            ans_R[temp_sR++] = v; in_R[v] = true; in_cand_R[v] = d;
            for (auto w : p_adj[v])
            {
                if (in_excl_R[w] == d+1) cnt[w]++;
                if (in_excl_L[w] == d+1) mark[w] = -1*mark_num;
            }
            for (auto w : n_adj[v]) if (select_L[w]) negD[w]++;
        }
        else temp_save.push_back(v);
    }
    cand_R[d+1] = temp_save;
    assert(Rpart_size == temp_sR+cand_R[d+1].size());
}
