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
#define MP(A,B) make_pair(A,B)
typedef long long int64;

char gz_datafilename[1024], gz_outputfilename[1024];

int n, m, *degree, **g, *e1, *e2;
int *c, *outer, *inner;

int64 sqr(int64 n)
{
	return n * n;
}

void init()
{
	printf("Loading...\n");
	FILE *f = fopen(gz_datafilename, "r");
	fscanf(f, "%d%d", &n, &m);
	degree = new int[n];
	g = new int* [n];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	int *edges = new int[m + m];
	for (int i = 0; i < m + m; ++i)
		fscanf(f, "%d", &edges[i]);
	fclose(f);
	e1 = new int[m];
	e2 = new int[m];
	for (int i = 0; i < m; ++i)
	{
		e1[i] = edges[i + i];
		e2[i] = edges[i + i + 1];
	}
	for (int i = 0; i < m + m; ++i)
		if (edges[i] != edges[i ^ 1])
			++degree[edges[i]];
	for (int i = 0; i < n; ++i)
		g[i] = new int[degree[i]];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	for (int i = 0; i < m + m; ++i)
		if (edges[i] != edges[i ^ 1])
			g[edges[i]][degree[edges[i]]++] = edges[i ^ 1];
	delete[] edges;
	printf("nodes = %d  edges = %d\n", n, m);
}

void solve()
{
	c = new int[n];
	for (int i = 0; i < n; ++i)
		c[i] = i;
	outer = new int[n + n];
	inner = new int[n + n];
	FILE *f = fopen(gz_outputfilename, "w");
	set<int> colors;
	for (int i = 0; i < n; ++i)
		colors.insert(i);
	for (int idx = n; ; ++idx)
	{
		printf("%d %d\n", idx - n, n);
		map<pair<int, int>, int> M;
		for (int i = 0; i < n + n; ++i)
			inner[i] = outer[i] = 0;
		for (int i = 0; i < m; ++i)
		{
			int c1 = c[e1[i]];
			int c2 = c[e2[i]];
			if (c1 > c2)
				swap(c1, c2);
			if (c1 != c2)
			{
				++outer[c1];
				++outer[c2];
			}
			else
				inner[c1] += 2;
			++M[MP(c1, c2)];
		}
		int64 bw = -1000000000LL * 1000000000LL;
		int bc1 = -1, bc2 = -1;
		vector<int> c_list(colors.begin(),colors.end());
		for (int i = 0; i < SIZE(c_list); ++i)
			for (int j = i + 1; j < SIZE(c_list); ++j)
			{
				int c1 = c_list[i];
				int c2 = c_list[j];
				if (M.find(MP(c1, c2)) == M.end())
					continue;
				int d = M[MP(c1, c2)];
				int ac1 = inner[c1] + outer[c1];
				int ac2 = inner[c2] + outer[c2];
				int64 w = (int64)d * m - (int64)ac1 * (int64)ac2;
				if (w > bw || w == bw && c1 < bc1 || w == bw && c1 == bc1 && c2 < bc2)
				{
					bw = w;
					bc1 = c1;
					bc2 = c2;
				}
			}
		if (bc1 < 0)
			break;
		fprintf(f, "%d %d\n", bc1, bc2);
		fflush(stdout);
		colors.erase(bc1);
		colors.erase(bc2);
		colors.insert(idx);
		for (int i = 0; i < n; ++i)
			if (c[i] == bc1 || c[i] == bc2)
				c[i] = idx;
	}
	fclose(f);
}

int main(int argc, char ** args)
{
//	strcpy(gz_datafilename, "..\\..\\Data\\twitter_data\\tw_data.txt"); strcpy(gz_outputfilename, "tw_edge_order2.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\roadNet-PA\\rd_data.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\Coauthor\\co_data.txt");	strcpy(gz_outputfilename, "co_edge_order2.ans");
//	strcpy(gz_datafilename, "test.in"); strcpy(gz_outputfilename, "test.ans");	
	strcpy(gz_datafilename, "large.in"); strcpy(gz_outputfilename, "large.ans");	
	init();
	solve();
	return 0;
}
