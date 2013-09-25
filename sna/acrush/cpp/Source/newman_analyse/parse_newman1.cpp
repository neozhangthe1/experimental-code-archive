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

void decode_newman1()
{	
//	freopen("gc_newman1_old.out", "r", stdin);
//	freopen("gc_newman1.out", "w", stdout);
//	freopen("twlarge_newman1_old.txt", "r", stdin);
//	freopen("twlarge_newman1.txt", "w", stdout);
	freopen("pr10p_newman1_old.txt", "r", stdin);
	freopen("pr10p_newman1.txt", "w", stdout);
	vector<int> a;
	int d;
	while (scanf("%d", &d) != -1)
		a.push_back(d);
	int *father = new int[n + n];
	for (int i = 0; i < n + n; ++i)
		father[i] = i;
	int id = n;
	for (int i = SIZE(a) - 2; i >= 0; i -= 2)
	//for (int i = 0; i < SIZE(a); i += 2)
	{
		int u = getfather(father, a[i]);
		int v = getfather(father, a[i + 1]);
		if (u == v)
			continue;
		printf("%d %d\n", u, v);
		father[u] = father[v] = (id++);
	}
}
int main()
{
	load_graph("..\\..\\Data\\GCRBC\\gc_data.txt");
	load_name("..\\..\\Data\\GCRBC\\gc_authors.txt");

	decode_newman1();
	return 0;

}