#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

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

int n, m, *edge_ep, *degree, **g;


void trim(string &s)
{
	while (LENGTH(s) > 0 && s[0] <= 32) s = s.substr(1);
	while (LENGTH(s) > 0 && s[LENGTH(s) - 1] <= 32) s = s.substr(0, LENGTH(s) - 1);
}

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
}

string *names;

void load_name(string filename)
{
	names = new string[n];
	FILE *f = fopen(filename.c_str(), "r");
	char *str = new char[1 << 20];
	while (1)
	{
		str[0] = 0;
		fgets(str, 1 << 20, f);
		if (str[0] == 0)
			break;
		int p = 0;
		for (; str[p] != ' '; p++);
		int id;
		sscanf(str, "%d", &id);
		names[id] = str + p + 1;
		trim(names[id]);
	}
	delete[] str;
	fclose(f);
}


ipair get_alpha_beta(VI s)
{
	ASSERT(SIZE(s) > 0 && SIZE(s) < n);
	bool *used = new bool[n];
	int *e = new int[n];
	for (int i = 0; i < n; ++i)
	{
		used[i] = false;
		e[i] = 0;
	}
	for (int i = 0; i < SIZE(s); ++i)
	{
		int key = s[i];
		used[key] = true;
		for (int j = 0; j < degree[key]; ++j)
			++e[g[key][j]];
	}
	int alpha = 0, beta = n + 1;
	for (int i = 0; i < n; ++i)
		if (used[i])
			ckmin(beta, e[i]);
		else
			ckmax(alpha, e[i]);
	delete[] e;
	delete[] used;
	return MP(alpha, beta);
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

void save_community(string filename, VI s)
{
	FILE *f = fopen(filename.c_str(), "w");
	for (int i = 0; i < SIZE(s); ++i)
		fprintf(f, "%d %s\n", s[i], names[s[i]].c_str());
	fclose(f);
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

int getfather(int father[], int v)
{
	return (father[v] == v) ? v : (father[v] = getfather(father, father[v]));
}

int main()
{
	load_graph("..\\..\\Data\\GCRBC\\gc_data.txt");
	load_name("..\\..\\Data\\GCRBC\\gc_authors.txt");

//	freopen("gc_newman1.out", "r", stdin);
//	freopen("gc_newman2.out", "r", stdin);
//	freopen("gc_newman2_new.out", "r", stdin);
	freopen("pr20p_newman2.txt", "r", stdin);

	string datasets[5] = {"gc_AI.txt", "gc_DB.txt", "gc_DP.txt", "gc_GV.txt", "gc_NC.txt"};
	VI init_sets[5];
	for (int i = 0; i < 5; ++i)
		init_sets[i] = load_community(datasets[i]);
	double maxs[5][4];
	for (int k = 0; k < 5; ++k)
		for (int i = 0; i < 4; ++i)
			maxs[k][i] = 0;
	VI *h = new VI[n + n];
	for (int i = 0; i < n; ++i)
		h[i].push_back(i);
	for (int id = n, u, v; scanf("%d%d", &u, &v) != -1; ++id)
	{
		if ((id - n) % 1000 == 0)
		{
			printf("%d / %d\n", id - n, n);
			for (int k = 0; k < 5; ++k)
			{
				for (int i = 0; i < 4; ++i)
					printf("%.6lf  ", maxs[k][i]);
				printf("\n");
			}
		}
		h[id] = h[u];
		for (int j = 0; j < SIZE(h[v]); ++j)
			h[id].push_back(h[v][j]);
		h[u].clear();
		h[v].clear();
		for (int k = 0; k < 5; ++k)
		{
			VI a = h[id];
			VI b = init_sets[k];
			if (SIZE(a) > 3 * SIZE(b))
				continue;
			if (SIZE(b) > 3 * SIZE(a))
				continue;
			double s0 = score_precision(a, b);
			double s1 = score_recall(a, b);
			double s2 = (2 * s0 * s1) / (s0 + s1);
			double s3 = score_distance(a, b);
			if (s2 > maxs[k][2])
				maxs[k][2] = s2, maxs[k][0] = s0, maxs[k][1] = s1;
			if (s3 > maxs[k][3])
				maxs[k][3] = s3;
		}
	}
	for (int k = 0; k < 5; ++k)
	{
		for (int i = 0; i < 4; ++i)
			printf("%.6lf  ", maxs[k][i]);
		printf("\n");
	}
	//VI result1 = greedy(SIZE(init_set), init_set);
	//printf("c1 = %d\n", SIZE(overlap(init_set, result1)));
	//VI result2 = annealing(SIZE(init_set) * 15, init_set);
	//printf("c2 = %d\n", SIZE(overlap(init_set, result2)));
	//save_community("..\\..\\Data\\GCRBC\\egc_GV.txt", overlap(init_set, result2));
	return 0;
}