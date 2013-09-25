#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <algorithm>
#include <ctime>

using namespace std;

typedef vector<int> VI;
#define SIZE(A) ((int)A.size())
#define ASSERT(X) {if (!(X)) { printf("ERROR   LINE %d\n",__LINE__); exit(0); }}

char gz_datafilename[1024], gz_outputfilename[1024];

int n, m, *degree, **g, **h, *edge_p1, *edge_p2;
int *q, *dist;
double *cnt, *pcnt, *w;
bool *visited;
pair<double, int> *results;

void init()
{
	printf("Loading...\n");
	FILE *f = fopen(gz_datafilename, "r");
	fscanf(f, "%d%d", &n, &m);
	degree = new int[n];
	g = new int* [n];
	h = new int* [n];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	int *edges = new int[m + m];
	for (int i = 0; i < m + m; ++i)
		fscanf(f, "%d", &edges[i]);
	edge_p1 = new int[m];
	edge_p2 = new int[m];
	for (int i = 0; i < m; ++i)
	{
		edge_p1[i] = edges[i + i];
		edge_p2[i] = edges[i + i + 1];
	}
	fclose(f);
	for (int i = 0; i < m + m; ++i)
		if (edges[i] != edges[i ^ 1])
			++degree[edges[i]];
	for (int i = 0; i < n; ++i)
	{
		g[i] = new int[degree[i]];
		h[i] = new int[degree[i]];
	}
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	for (int i = 0; i < m + m; ++i)
		if (edges[i] != edges[i ^ 1])
		{
			g[edges[i]][degree[edges[i]]] = edges[i ^ 1];
			h[edges[i]][degree[edges[i]]++] = (i >> 1);
		}
	delete[] edges;
	printf("nodes = %d  edges = %d\n", n, m);
	q = new int[n];
	dist = new int[n];
	visited = new bool[n];
	cnt = new double[m];
	pcnt = new double[n];
	w = new double[n];
	results = new pair<double, int> [m];
}

void solve()
{
	for (int i = 0; i < m; ++i)
		cnt[i] = 0;
	double start_time = (double)clock();
	for (int source = 0; source < n; ++source)
	{
		for (int i = 0; i < n; ++i)
		{
			visited[i] = false;
			dist[i] = n + 10;
		}
		int sizeq = 0;
		q[sizeq++] = source;
		dist[source] = 0;
		visited[source] = true;
		for (int cl = 0; cl < sizeq; ++cl)
			for (int k = q[cl], i = degree[k] - 1; i >= 0; --i)
			{
				int other = g[k][i];
				if (!visited[other])
				{
					visited[other] = true;
					q[sizeq++] = other;
					dist[other] = dist[k] + 1;
				}
			}
		pcnt[source] = 1;
		for (int cl = 1; cl < sizeq; ++cl)
		{
			int k = q[cl];
			pcnt[k] = 0;
			for (int ed = dist[k] - 1, i = degree[k] - 1; i >= 0; --i)
			{
				int other = g[k][i];
				if (dist[other] == ed)
					pcnt[k] += pcnt[other];
			}
		}
		for (int cl = sizeq - 1; cl >= 0; --cl)
		{
			int k = q[cl];
			w[k] = 1;
			for (int ed = dist[k] + 1, i = degree[k] - 1; i >= 0; --i)
			{
				int other = g[k][i];
				if (dist[other] == ed)
				{
					double sg = pcnt[k] / pcnt[other] * w[other];
					cnt[h[k][i]] += sg;
					w[k] += sg;
				}
			}
		}
		if ((source + 1) % 1000 == 0)
		{
			double current_time = (double)clock();
			double time_used = current_time - start_time;
			double exp_time_left = time_used / (source + 1) * (n - 1 - source);
			printf("%d / %d   exp_time_left = %.0lf seconds\n", source + 1, n, exp_time_left / CLOCKS_PER_SEC);
		}
	}
	for (int i = 0; i < m; ++i)
		results[i] = make_pair(cnt[i], i);
	sort(results, results + m);
	reverse(results, results + m);
	FILE *f = fopen(gz_outputfilename, "w");
	for (int i = 0; i < m; ++i)
	{
		int e_id = results[i].second;
		fprintf(f, "%d %d\n", edge_p1[e_id], edge_p2[e_id]);
	}
	fclose(f);
}

int main(int argc, char ** args)
{
//	strcpy(gz_datafilename, "..\\..\\Data\\twitter_data\\tw_data.txt"); strcpy(gz_outputfilename, "tw_edge_order.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\roadNet-PA\\rd_data.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\Slashdot\\st_data.txt"); strcpy(gz_outputfilename, "st_edge_order.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\Coauthor\\co_data.txt");	strcpy(gz_outputfilename, "co_edge_order.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\twitter_data\\twlarge.txt"); strcpy(gz_outputfilename, "twlarge_newman1.txt");
	strcpy(gz_datafilename, "gc_data_pr10p.txt"); strcpy(gz_outputfilename, "pr10p_newman1_old.txt");
	init();
	solve();
	return 0;
}
