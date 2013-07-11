#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <algorithm>
#include <ctime>
#include <queue>

using namespace std;

typedef vector<int> VI;
#define SIZE(A) ((int)(A).size())
#define ASSERT(X) {if (!(X)) { printf("ERROR   LINE %d\n",__LINE__); exit(0); }}
#define MP(A,B) make_pair(A,B)
typedef long long int64;

char gz_datafilename[1024], gz_outputfilename[1024];

class Counter
{
public:
	int m, n, current_idx;
	int *list, *idx, *cnt;
	Counter(int n = 0)
	{
		this->n = n;
		m = 0;
		current_idx = 1;
		idx = new int[n];
		list = new int[n];
		cnt = new int[n];
		for (int i = 0; i < n; ++i)
			idx[i] = 0;
	}
	~Counter()
	{
		delete[] idx;
		delete[] list;
		delete[] cnt;
	}
	void clear()
	{
		++current_idx;
		m = 0;
	}
	void add(int key)
	{
		if (key < 0 || key >= n)
			return;
		if (idx[key] == current_idx)
			++cnt[key];
		else
		{
			idx[key] = current_idx;
			cnt[key] = 1;
			list[m++] = key;
		}
	}
};

int n, m, *degree, **g, *e1, *e2;
int *color, *ea;
VI **h;
priority_queue<pair<int64, int> > **a;
Counter *map1;

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

void prepare(int idx)
{
	map1->clear();
	ea[idx] = 0;
	for (int i = 0; i < SIZE(*h[idx]); ++i)
	{
		int key = (*h[idx])[i];
		ea[idx] += degree[key];
		for (int j = 0; j < degree[key]; ++j)
		{
			int other = color[g[key][j]];
			if (other != idx)
				map1->add(other);
		}
	}
	while (!a[idx]->empty())
		a[idx]->pop();
	for (int i = 0; i < map1->m; ++i)
	{
		int other = map1->list[i];
		a[idx]->push(MP((int64)map1->cnt[other] * (int64)m - (int64)ea[idx] * (int64)ea[other], -other));
	}
}

pair<int64, pair<pair<int, int>, int> > get_state(int idx)
{
	int other = -a[idx]->top().second;
	return MP(a[idx]->top().first, MP(MP(-min(idx, other), -max(idx, other)), idx));
}

void solve()
{
	color = new int[n];
	map1 = new Counter(n + n);
	ea = new int[n + n];
	h = new VI* [n + n];
	a = new priority_queue<pair<int64, int> >* [n + n];
	for (int i = 0; i < n + n; ++i)
	{
		h[i] = new VI;
		a[i] = new priority_queue<pair<int64, int> >;
	}
	for (int i = 0; i < n; ++i)
	{
		color[i] = i;
		h[i]->push_back(i);
	}
	priority_queue<pair<int64, pair<pair<int, int>, int> > > heap;
	for (int i = 0; i < n; ++i)
		ea[i] = degree[i];
	for (int i = 0; i < n; ++i)
	{
		prepare(i);
		if (!a[i]->empty())
			heap.push(get_state(i));
	}
	FILE *f = fopen(gz_outputfilename, "w");
	for (int idx = n; ; ++idx)
	{
		if ((idx - n + 1) % 1000 == 0)
			printf("%d / %d\n", idx - n + 1, n);
		int c1, c2;
		bool exists = false;
		while (!heap.empty())
		{
			c1 = heap.top().second.second;
			c2 = - heap.top().second.first.first - heap.top().second.first.second - c1;
			heap.pop();
			if (h[c1] == NULL)
				continue;
			if (h[c2] == NULL)
			{
				a[c1]->pop();
				if (!a[c1]->empty())
					heap.push(get_state(c1));
				continue;
			}
			exists = true;
			break;
		}
		if (!exists)
			break;
		for (int i = 0; i < SIZE(*h[c1]); ++i)
			h[idx]->push_back((*h[c1])[i]);
		for (int i = 0; i < SIZE(*h[c2]); ++i)
			h[idx]->push_back((*h[c2])[i]);
		delete a[c1];
		delete a[c2];
		delete h[c1];
		delete h[c2];
		a[c1] = a[c2] = NULL;
		h[c1] = h[c2] = NULL;
		for (int i = 0; i < SIZE(*h[idx]); ++i)
			color[(*h[idx])[i]] = idx;
		prepare(idx);
		if (!a[idx]->empty())
			heap.push(get_state(idx));
		fprintf(f, "%d %d\n", min(c1, c2), max(c1, c2));
	}
	fclose(f);
}

int main(int argc, char ** args)
{
//	strcpy(gz_datafilename, "..\\..\\Data\\twitter_data\\tw_data.txt"); strcpy(gz_outputfilename, "tw_edge_order2.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\roadNet-PA\\rd_data.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\Coauthor\\co_data.txt");	strcpy(gz_outputfilename, "co_edge_order2.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\Slashdot\\st_data.txt");	strcpy(gz_outputfilename, "st_edge_order2.txt");
//	strcpy(gz_datafilename, "..\\..\\Data\\twitter_data\\twlarge.txt"); strcpy(gz_outputfilename, "twlarge_newman2.txt");
	strcpy(gz_datafilename, "gc_data_pr10p.txt"); strcpy(gz_outputfilename, "pr10p_newman2.txt");
	init();
	solve();
	return 0;
}
