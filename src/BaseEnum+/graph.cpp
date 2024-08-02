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
    VI part_L, part_R;
    part_L.reserve(n); part_R.reserve(n);
    for (auto u : U)
    {
        part_L.clear(); part_R.clear();
        select_L[u] = 1;
        part_L.push_back(u);
        for (auto v : p_adj[u])
            if (v > u) {select_L[v] = 1; part_L.push_back(v);}
        for (auto v : n_adj[u])
            if (v > u) {select_R[v] = 2; part_R.push_back(v);}
        for (auto v : part_L)
            if (v > u)
                for (auto w : n_adj[v])
                    if (w > u && select_L[w] == 0 && select_R[w] == 0) {select_R[w] = 1; part_R.push_back(w);}
        I Lpart_size = part_L.size(), Rpart_size = part_R.size();
        I flag = 1;
        if (flag)
        {
            process_vertex++;
            cand_L[0].clear(); cand_R[0].clear();
            excl_L[0].clear(); excl_R[0].clear();
            mark.clear(); mark.resize(n, 0); mark_num = 0;

            ans_L[0] = u; in_L[u] = true;
            for (auto v : part_L)
                if (select_L[v] && v > u) {cand_L[0].push_back(v); in_cand_L[v] = 0;}
            for (auto v : part_R)
                if (select_R[v]) {cand_R[0].push_back(v); in_cand_R[v] = 0;}
            assert(Lpart_size == cand_L[0].size()+1);
            assert(Rpart_size == cand_R[0].size());
            for (auto v : p_adj[u])
                if (v < u) {select_L[v] = 1; excl_L[0].push_back(v); in_excl_L[v] = 0;}
            for (auto v : n_adj[u])
            {
                if (v < u) {select_R[v] = 1; excl_R[0].push_back(v); in_excl_R[v] = 0;}
                if (select_R[v]) negD[v]++;
            }
            for (auto w : cand_L[0])
                for (auto v : n_adj[w])
                    if (v < u && select_R[v] == 0 && select_L[v] == 0) {select_R[v] = 1; excl_R[0].push_back(v); in_excl_R[v] = 0;}
            sort(cand_L[0].begin(), cand_L[0].end());
            sort(cand_R[0].begin(), cand_R[0].end());
            sort(excl_L[0].begin(), excl_L[0].end());
            sort(excl_R[0].begin(), excl_R[0].end());
            subOPT_Swap(0, 1, 0, 0);
            in_L[u] = false;
            for (auto v : n_adj[u]) if (select_R[v]) negD[v]--;
            for (auto v : cand_L[0]) {in_cand_L[v] = -1; select_L[v] = 0;}
            for (auto v : cand_R[0]) {in_cand_R[v] = -1; select_R[v] = 0;}
            for (auto v : excl_L[0]) {in_excl_L[v] = -1; select_L[v] = 0;}
            for (auto v : excl_R[0]) {in_excl_R[v] = -1; select_R[v] = 0;}
        }
        for (auto v : part_L) select_L[v] = 0;
        for (auto v : part_R) select_R[v] = 0;
    }
}

void Graph::subOPT_Swap(I d, I sL, I sR, I swap)
{
	if ((double)clock() / CLOCKS_PER_SEC - throwtime > TIMEOVER)
		throw string("TIMEOVER");
        
    if (cand_L[d].size() == 0 && cand_R[d].size() == 0 && excl_L[d].size() == 0 && excl_R[d].size() == 0)
        if (sL >= R && sR >= R) 
        {
            OUT_Num++;
        }

    if (swap)
    {
        for (auto u : cand_L[d])
        {
            mark_num++;
            ans_L[sL] = u; in_L[u] = true;

            cand_L[d+1].clear(); cand_R[d+1].clear();
            excl_L[d+1].clear(); excl_R[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_R[v]) {negD[v]++; mark[v] = mark_num;}

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
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                }
                if (in_cand_L[v] == d)
                {
                    if (v > u)
                    {
                        if (cnt_num == 0) 
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                    }
                    else if (v < u)
                    {
                        if (cnt_num == 0)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1;}
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
                            {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                    }
                    else
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                }

            if (sL+1+cand_L[d+1].size() >= R && sR+cand_R[d+1].size() >= R)
                subOPT_Swap(d+1, sL+1, sR, !swap);

            in_L[u] = false;
            for (auto v : n_adj[u]) if (select_R[v]) negD[v]--;
            for (auto v : cand_L[d+1]) in_cand_L[v] = d;
            for (auto v : cand_R[d+1]) in_cand_R[v] = d;
            for (auto v : excl_L[d+1]) if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;
            for (auto v : excl_R[d+1]) in_excl_R[v] = d;
        }

        for (auto u : cand_R[d])
        {
            mark_num++;
            ans_R[sR] = u; in_R[u] = true;

            cand_R[d+1].clear(); cand_L[d+1].clear();
            excl_R[d+1].clear(); excl_L[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_L[v]) {negD[v]++; mark[v] = mark_num;}

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
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                }
                if (in_cand_R[v] == d)
                {
                    if (v > u)
                    {
                        if (cnt_num == 0) 
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                    }
                    else if (v < u)
                    {
                        if (cnt_num == 0)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1;}
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
                            {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                    }
                    else
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                }
            for (auto v : cand_L[d])
                if (sR+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_L)
                    {
                        if (mark[v] == mark_num)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1;}
                    }
                    else
                        {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1;}
                }

            if (sR+1+cand_R[d+1].size() >= R && sL+cand_L[d+1].size() >= R)
                subOPT_Swap(d+1, sL, sR+1, !swap);

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
            mark_num++;
            ans_R[sR] = u; in_R[u] = true;

            cand_R[d+1].clear(); cand_L[d+1].clear();
            excl_R[d+1].clear(); excl_L[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_L[v]) {negD[v]++; mark[v] = mark_num;}

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
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                }
                if (in_cand_R[v] == d)
                {
                    if (v > u)
                    {
                        if (cnt_num == 0) 
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_R[d+1].push_back(v); in_cand_R[v] = d+1;}
                    }
                    else if (v < u)
                    {
                        if (cnt_num == 0)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1;}
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
                            {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                    }
                    else
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                }

            if (sR+1+cand_R[d+1].size() >= R && sL+cand_L[d+1].size() >= R)
                subOPT_Swap(d+1, sL, sR+1, !swap);

            in_R[u] = false;
            for (auto v : n_adj[u]) if (select_L[v]) negD[v]--;
            for (auto v : cand_R[d+1]) in_cand_R[v] = d;
            for (auto v : cand_L[d+1]) in_cand_L[v] = d;
            for (auto v : excl_R[d+1]) if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;
            for (auto v : excl_L[d+1]) in_excl_L[v] = d;
        }

        for (auto u : cand_L[d])
        {
            mark_num++;
            ans_L[sL] = u; in_L[u] = true;

            cand_L[d+1].clear(); cand_R[d+1].clear();
            excl_L[d+1].clear(); excl_R[d+1].clear();

            for (auto v : n_adj[u]) 
                if (select_R[v]) {negD[v]++; mark[v] = mark_num;}

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
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                    else if (mark[v] == mark_num && cnt[v] == cnt_num)
                        {excl_L[d+1].push_back(v); in_excl_L[v] = d+1;}
                }
                if (in_cand_L[v] == d)
                {
                    if (v > u)
                    {
                        if (cnt_num == 0) 
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {cand_L[d+1].push_back(v); in_cand_L[v] = d+1;}
                    }
                    else if (v < u)
                    {
                        if (cnt_num == 0)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1;}
                        else if (mark[v] == mark_num && cnt[v] == cnt_num)
                            {excl_L[d+1].push_back(v); assert(in_excl_L[v] == -1); in_excl_L[v] = d+1;}
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
                            {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                    }
                    else
                        {excl_R[d+1].push_back(v); in_excl_R[v] = d+1;}
                }
            for (auto v : cand_R[d])
                if (sL+1-negD[v] <= K && mark[v] != -1*mark_num)
                {
                    if (flag_R)
                    {
                        if (mark[v] == mark_num)
                            {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1;}
                    }
                    else
                        {excl_R[d+1].push_back(v); assert(in_excl_R[v] == -1); in_excl_R[v] = d+1;}
                }

            if (sL+1+cand_L[d+1].size() >= R && sR+cand_R[d+1].size() >= R)
                subOPT_Swap(d+1, sL+1, sR, !swap);

            in_L[u] = false;
            for (auto v : n_adj[u]) if (select_R[v]) negD[v]--;
            for (auto v : cand_L[d+1]) in_cand_L[v] = d;
            for (auto v : cand_R[d+1]) in_cand_R[v] = d;
            for (auto v : excl_L[d+1]) if (in_cand_L[v] == d) in_excl_L[v] = -1; else in_excl_L[v] = d;
            for (auto v : excl_R[d+1]) if (in_cand_R[v] == d) in_excl_R[v] = -1; else in_excl_R[v] = d;
        }
    }
}
