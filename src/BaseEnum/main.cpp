# include "graph.h"

int main(int argc, char *argv[])
{
    string algo, graphname, graphfile;
    I K, R;

    algo=argv[1];
    graphname=argv[2];
    K = atoi(argv[3]);
    R = atoi(argv[4]);

    cout << algo << "  " << graphname << "  K:" << K << "  R:" << R << endl;

    graphfile="../../sample_dataset/" + graphname + ".txt";

    Graph g;
    g.K = K;
    g.R = R;

    g.dataInput(graphfile);
    cout << "Input Done" << endl;

    double pruning_begin = (double)clock() / CLOCKS_PER_SEC;
    g.Pruning();
    double PruningTime = (double)clock() / CLOCKS_PER_SEC - pruning_begin;
    cout << "Pruning Done" << endl;
    cout << "Pruning U: " << g.n_del << "  left_n: " << g.n << endl;
    cout << "Pruning E: " << g.m_del << "  left_m: " << g.m << "  left_pm: " << g.pm << "  left_nm: " << g.nm << endl;
    cout << "PruningTime: " << PruningTime << endl;

    double algorithm_begin = (double)clock() / CLOCKS_PER_SEC;
    g.OPT();
    double EnumTime = (double)clock() / CLOCKS_PER_SEC - algorithm_begin;
    cout << "process_vertex: " << g.process_vertex << endl;
    cout << "OUT_Num: " << g.OUT_Num << endl;
    cout << "EnumTime: " << EnumTime << endl;
    cout << "AllTime: " << PruningTime + EnumTime << endl;

    return 0;
}