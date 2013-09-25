#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <ctime>

using namespace std;

typedef vector<int> VI;
typedef set<int> SI;
#define LENGTH(A) ((int)A.length())
typedef pair<int,int> ipair;
#define MP(A,B) make_pair(A,B)
#define SIZE(A) ((int)A.size())
#define ASSERT(X) {if (!(X)) { printf("ERROR   LINE %d\n",__LINE__); exit(0); }}
template<class T> void ckmin(T &a,T b) { if (b<a) a=b; }
template<class T> void ckmax(T &a,T b) { if (b>a) a=b; }
typedef vector<int> VI;
typedef long long int64;

int n, m, *edge_ep;
int *degree, **g, max_degree;
int *father, *e;
VI **components;
int *marker1, counter1;
VI init_sets[5];
double maxs[5][4];

void load_graph(string filename)
{
	FILE *f = fopen(filename.c_str(), "r");
	fscanf(f, "%d%d", &n, &m);
	edge_ep = new int[m + m];
	for (int i = 0; i < m + m; ++i)
		fscanf(f, "%d", &edge_ep[i]);
	fclose(f);
	for (int i = 0; i < m + m; ++i)
		ASSERT(edge_ep[i] != edge_ep[i ^ 1]);
	degree = new int[n];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	for (int i = 0; i < m + m; ++i)
		++degree[edge_ep[i]];
	g = new int* [n];
	for (int i = 0; i < n; ++i)
		g[i] = new int[degree[i]];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	for (int i = 0; i < m + m; ++i)
		g[edge_ep[i]][degree[edge_ep[i]]++] = edge_ep[i ^ 1];
	max_degree = 0;
	for (int i = 0; i < n; ++i)
		ckmax(max_degree, degree[i]);
}

int getfather(int p)
{
	return (father[p] < 0) ? p : (father[p] = getfather(father[p]));
}

int count_edge_between(int a, int b)
{
	if (components[a]->size() > components[b]->size())
		swap(a, b);
	counter1++;
	for (int i = 0; i < components[b]->size(); ++i)
		marker1[(*components[b])[i]] = counter1;
	int results = 0;
	for (int i = 0; i < components[a]->size(); ++i)
		for (int key = (*components[a])[i], j = 0; j < degree[key]; ++j)
			if (marker1[g[key][j]] == counter1)
				results++;
	return results;
}


VI load_community(string filename)
{
	VI results;
	FILE *f = fopen(filename.c_str(), "r");
	char *s = new char[1 << 20];
	while (1)
	{
		s[0] = 0;
		fgets(s, 1 << 20, f);
		if (s[0] == 0)
			break;
		int id;
		sscanf(s, "%d", &id);
		results.push_back(id);
	}
	delete[] s;
	fclose(f);
	return results;
}

VI overlap(VI a, VI b)
{
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	VI c;
	for (int i = 0, j = 0; i < SIZE(a) && j < SIZE(b); )
		if (a[i] == b[j])
		{
			c.push_back(a[i]);
			i++;
			j++;
		}
		else if (a[i] < b[j])
			i++;
		else
			j++;
	return c;
}

VI getunion(VI a, VI b)
{
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	VI c;
	int i, j;
	for (i = 0, j = 0; i < SIZE(a) && j < SIZE(b); )
		if (a[i] == b[j])
		{
			c.push_back(a[i]);
			i++;
			j++;
		}
		else if (a[i] < b[j])
		{
			c.push_back(a[i]);
			i++;
		}
		else
		{
			c.push_back(b[j]);
			j++;
		}
	for (; i < SIZE(a); ++i)
		c.push_back(a[i]);
	for (; j < SIZE(b); ++j)
		c.push_back(b[j]);
	return c;
}

double score_precision(VI a, VI b)
{
	SI sa, sb;
	for (int i = 0; i < SIZE(a); ++i)
		sa.insert(a[i]);
	for (int i = 0; i < SIZE(b); ++i)
		sb.insert(b[i]);
	int c1 = 0, c2 = 0;
	for (int i = 0; i < SIZE(b); ++i)
	{
		int key = b[i];
		for (int j = 0; j < degree[key]; ++j)
		{
			int other = g[key][j];
			if (sb.find(other) == sb.end())
				continue;
			++c1;
			if (sa.find(key) != sa.end() && sa.find(other) != sa.end())
				++c2;
		}
	}
	return (double)c2 / c1;
}

double score_recall(VI a, VI b)
{
	SI sa, sb;
	for (int i = 0; i < SIZE(a); ++i)
		sa.insert(a[i]);
	for (int i = 0; i < SIZE(b); ++i)
		sb.insert(b[i]);
	int c1 = 0, c2 = 0;
	for (int i = 0; i < SIZE(a); ++i)
	{
		int key = a[i];
		for (int j = 0; j < degree[key]; ++j)
		{
			int other = g[key][j];
			if (sa.find(other) == sa.end())
				continue;
			++c1;
			if (sb.find(key) != sb.end() && sb.find(other) != sb.end())
				++c2;
		}
	}
	return (double)c2 / c1;
}

double score_F(VI a, VI b)
{
	double p = score_precision(a, b);
	double r = score_recall(a, b);
	return 2 * p * r / (p + r);
}

double score_distance(VI a, VI b)
{
	VI c = overlap(a, b);
	return (double)(SIZE(c)) / (double)(SIZE(a) + SIZE(b) - SIZE(c));
}

double score(VI a, VI b, int t)
{
	if (t == 0) return score_precision(a, b);
	if (t == 1) return score_recall(a, b);
	if (t == 2) return score_F(a, b);
	if (t == 3) return score_distance(a, b);
	return 0;
}

int step = 0;

void update_results(const VI &results)
{
	for (int key = 0; key < 1; ++key)
	{
		if (SIZE(results) < SIZE(init_sets[key]) / 10)
			continue;
		if (SIZE(results) > SIZE(init_sets[key]) * 10)
			continue;
		double s0 = score_precision(init_sets[key], results);
		double s1 = score_recall(init_sets[key], results);
		double s2 = (2 * s0 * s1) / (s0 + s1);
		double s3 = score_distance(init_sets[key], results);
		if (s2 > maxs[key][2])
		{
			maxs[key][2] = s2;
			maxs[key][1] = s1;
			maxs[key][0] = s0;
		}
		if (s3 > maxs[key][3])
		{
			maxs[key][3] = s3;
		}
	}
	if ((++step) % 100 == 0)
	{
		printf("step = %d\n", step);
		for (int k = 0; k < 1; ++k)
		{
			for (int i = 0; i < 4; ++i)
				printf("%.6lf  ", maxs[k][i]);
			printf("\n");
		}
		printf("\n");
	}
}

void solve()
{
	marker1 = new int[n + n];
	for (int i = 0; i < n + n; ++i)
		marker1[i] = 0;
	counter1 = 0;
	father = new int[n + n];
	for (int i = 0; i < n + n; ++i)
		father[i] = -1;
	e = new int[n + n];
	for (int i = 0; i < n + n; ++i)
		e[i] = 0;
	components = new VI* [n + n];
	for (int i = 0; i < n; ++i)
	{
		components[i] = new VI;
		components[i]->push_back(i);
	}
	int next = n;
	map<ipair, int> checked;
	while (1)
	{
		bool find_more = false;
		while (1)
		{
			bool update = false;
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < degree[i]; ++j)
				{
					int other = g[i][j];
					int a = getfather(i);
					int b = getfather(other);
					if (a == b)
						continue;
					if (a > b)
						swap(a, b);
					if (checked.find(MP(a, b)) == checked.end())
						checked[MP(a, b)] = count_edge_between(a, b);
					int eij = checked[MP(a, b)];
					int sa = components[a]->size();
					int sb = components[b]->size();
					if ((double)(e[a] + e[b] + eij) / (sa + sb) > (double)e[a] / sa + (double)e[b] / sb)
					{
						find_more = update = true;
						if (components[a]->size() < components[b]->size())
							swap(a, b);
						components[next] = components[a];
						for (int k = 0; k < components[b]->size(); ++k)
							components[next]->push_back((*components[b])[k]);
						update_results(*components[next]);
						father[a] = next;
						father[b] = next;
						e[next] = e[a] + e[b] + eij;
						delete components[b];
						next++;
					}
				}
			int cnt = 0;
			for (int i = 0; i < next; ++i)
				if (father[i] < 0)
					cnt++;
			printf("cnt = %d\n", cnt);
			if (cnt == 1)
				break;
			if (!update)
				break;
		}
		if (!find_more) 
			break;
		for (int i = 0; i < next; ++i)
			e[i] = 0;
	}
}

int main()
{
	load_graph("gc_large.txt");
	//string datasets[5] = {"gc_AI.txt", "gc_DB.txt", "gc_DP.txt", "gc_GV.txt", "gc_NC.txt"};
	//string datasets[1] = {"admin_user2.txt"};
	//for (int i = 0; i < 1; ++i)
	//	init_sets[i] = load_community(datasets[i]);
	for (int i = 0; i < 1; ++i)
		for (int j = 0; j < 4; ++j)
			maxs[i][j] = 0;
	double start_time = (double)clock();
	solve();
	double end_time = (double)clock();
	printf("timeUsed = %.3lf\n", (end_time - start_time) / CLOCKS_PER_SEC);
	return 0;
}