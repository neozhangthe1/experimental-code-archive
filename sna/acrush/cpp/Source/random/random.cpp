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

int n, m, *edge_ep, *degree, **g, max_degree;


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
	max_degree = 0;
	for (int i = 0; i < n; ++i)
		ckmax(max_degree, degree[i]);
}

VI refine(const int csize, const VI &init_set)
{
	VI results;
	vector<ipair> q;
	for (int i = 0; i < SIZE(init_set); ++i)
		q.push_back(MP(-degree[init_set[i]], init_set[i]));
	sort(q.begin(), q.end());
	for (int i = 0; i < csize; ++i)
		results.push_back(q[i].second);
	return results;
}

int random(int n)
{
	return (n < 32768) ? (rand() % n) : ((((rand() & 32767) << 15) + (rand () & 32767)) % n);
}

VI random(const int csize, const VI &init_set)
{
	VI results = init_set;
	set<int> exists, closed;
	for (int i = 0; i < SIZE(results); ++i)
		exists.insert(results[i]);
	for (int i = 0; i < SIZE(results); ++i)
		for (int key = results[i], j = 0; j < degree[key]; ++j)
			if (exists.find(g[key][j]) == exists.end())
				closed.insert(g[key][j]);
	for (; SIZE(results) < csize; )
	{
		int key;
		if (SIZE(closed) == 0)
		{
			key = random(n);
			while (exists.find(key) != exists.end())
				key = random(n);
		}
		else
		{
			int k = random(SIZE(closed));
			set<int>::iterator it = closed.begin();
			for (; k > 0; --k)
				++it;
			key = *it;
		}
		exists.insert(key);
		results.push_back(key);
		closed.erase(key);
		for (int j = 0; j < degree[key]; ++j)
			if (exists.find(g[key][j]) == exists.end())
				closed.insert(g[key][j]);
	}
	return results;
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

double score(VI a, VI b, int t)
{
	if (t == 0) return score_precision(a, b);
	if (t == 1) return score_recall(a, b);
	if (t == 2) return score_F(a, b);
	if (t == 3) return score_distance(a, b);
	return 0;
}

int main()
{
	load_graph("..\\..\\Data\\GCRBC\\gc_data.txt");
	load_name("..\\..\\Data\\GCRBC\\gc_authors.txt");
	string datasets[5] = {"gc_AI.txt", "gc_DB.txt", "gc_DP.txt", "gc_GV.txt", "gc_NC.txt"};
	VI init_sets[5];
	for (int i = 0; i < 5; ++i)
		init_sets[i] = load_community(datasets[i]);
	double maxs[5][4];
	VI gs2[5], gs3[5];
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 4; ++j)
			maxs[i][j] = 0;
	for (int step = 0; step < 1000; ++step)
	{
		srand(step);
		int key = step % 5;
		int csize = SIZE(init_sets[key]) * (rand() % 15000 + 5000) / 10000;
		VI src_set;
		int p = init_sets[key][rand() % SIZE(init_sets[key])];
		src_set.push_back(p);
		srand(step);
		VI results = random(csize, src_set);
		double s0 = score_precision(init_sets[key], results);
		double s1 = score_recall(init_sets[key], results);
		double s2 = (2 * s0 * s1) / (s0 + s1);
		double s3 = score_distance(init_sets[key], results);
		if (s2 > maxs[key][2])
		{
			maxs[key][2] = s2;
			maxs[key][1] = s1;
			maxs[key][0] = s0;
			gs2[key] = results;
		}
		if (s3 > maxs[key][3])
		{
			maxs[key][3] = s3;
			gs3[key] = results;
		}
		if (step % 100 == 0)
		{
			printf("step = %d\n", step);
			for (int k = 0; k < 5; ++k)
			{
				for (int i = 0; i < 4; ++i)
					printf("%.6lf  ", maxs[k][i]);
				printf("\n");
			}
			printf("\n");
			FILE *f = fopen("tmp_results_random.txt", "w");
			for (int k = 0; k < 5; ++k)
			{
				for (int i = 0; i < SIZE(gs2[k]); ++i)
					fprintf(f, "%d ", gs2[k][i]);
				fprintf(f, "\n");
				for (int i = 0; i < SIZE(gs3[k]); ++i)
					fprintf(f, "%d ", gs3[k][i]);
				fprintf(f, "\n\n");
			}
			fclose(f);
		}
	}	
	for (int k = 0; k < 5; ++k)
	{
		for (int i = 0; i < 4; ++i)
			printf("%.6lf  ", maxs[k][i]);
		printf("\n");
	}
	return 0;
}