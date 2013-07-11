//#define DEBUG
#define LOCAL

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <cassert>

#ifdef LOCAL
#include <ctime>
#endif

using namespace std;

typedef vector<int> VI;
#define SIZE(A) ((int)A.size())
#define ASSERT(X) {if (!(X)) { printf("ERROR   LINE %d\n",__LINE__); exit(0); }}

const bool IS_PURE_RANDOM_DEFAULT = false;

int n, *degree, **g;
int *c, *w, *pos;
int maxw[2], minw[2];
VI *h[2];
set<VI> *unique_sets;
char *gz_datafilename, *gz_outputfilename;

double sum_density, sum_alpha, sum_beta, sum_total_degree, sum_total_intra_degree;

void init()
{
	printf("Loading...\n");
	int m;
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
	for (int i = 0; i < m + m; ++i)
		if (edges[i] != edges[i ^ 1])
			++degree[edges[i]];
	for (int i = 0; i < n; ++i)
		g[i] = new int[degree[i]];
	int *pdegree = new int[n];
	for (int i = 0; i < n; ++i)
		pdegree[i] = degree[i];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	for (int i = 0; i < m + m; ++i)
		if (edges[i] != edges[i ^ 1])
			g[edges[i]][degree[edges[i]]++] = edges[i ^ 1];
	delete[] pdegree;
	delete[] edges;
	c = new int[n];
	w = new int[n];
	pos = new int[n];
	for (int b = 0; b < 2; b++)
		h[b] = new VI[n + 1];
	printf("nodes = %d  edges = %d\n", n, m);
}

void discard()
{
	delete[] degree;
	for (int i = 0; i < n; ++i)
		delete[] g[i];
	delete[] g;
	delete[] c;
	delete[] w;
	delete[] pos;
	for (int b = 0; b < 2; b++)
		delete[] h[b];
}

void erase(int p)
{
	VI &nodes = h[c[p]][w[p]];
	int p0 = pos[p];
	if (p0 == SIZE(nodes))
		nodes.pop_back();
	else
	{
		nodes[p0] = nodes[SIZE(nodes) - 1];
		pos[nodes[p0]] = p0;
		nodes.pop_back();
	}
}

void insert(int p)
{
	int cp = c[p];
	int wp = w[p];
	VI &nodes = h[cp][wp];
	pos[p] = SIZE(nodes);
	nodes.push_back(p);
	if (wp > maxw[cp])
		maxw[cp] = wp;
	if (wp < minw[cp])
		minw[cp] = wp;
}

void update_w(int p, int delta)
{
	erase(p);
	w[p] += delta;
	insert(p);
}

void set_c(int p, int new_c)
{
	if (c[p] == new_c)
		return;
	if (new_c == 1)
		for (int i = 0; i < degree[p]; ++i)
			update_w(g[p][i], 1);
	else
		for (int i = 0; i < degree[p]; ++i)
			update_w(g[p][i], -1);
	erase(p);
	c[p] = new_c;
	insert(p);
}

int random()
{
	return ((rand() & 32767) << 15) + (rand() & 32767);
}

void process(int size, int count, bool is_pure_random)
{
	FILE *f = fopen(gz_outputfilename, "w");
	for (int step = 0; step < count; ++step)
	{
		printf("%d / %d   ", step + 1, count);
		for (int i = 0; i <= n; ++i)
		{
			h[0][i].clear();
			h[1][i].clear();
		}
		for (int i = 0; i < n; ++i)
		{
			c[i] = 0;
			w[i] = 0;
			pos[i] = SIZE(h[c[i]][w[i]]);
			h[c[i]][w[i]].push_back(i);
		}
		maxw[0] = maxw[1] = minw[0] = minw[1] = 0;
		if (is_pure_random)
		{
			int *p = new int[n];
			for (int i = 0; i < n; ++i)
				p[i] = i;
			for (int i = 0; i < size; ++i)
			{
				swap(p[i], p[i + random() % (n - size)]);
				set_c(p[i], 1);
			}
			delete[] p;
		}
		else
		{
			bool *visited = new bool[n];
			int *p = new int[n];
			int edges = 0;
			for (int i = 0; i < n; ++i)
				edges += degree[i];
			edges /= 2;
			int *q = new int[edges];
			int *w = new int[size];
			for (int i = 0; i < n; ++i)
			{
				visited[i] = false;
				p[i] = i;
			}
			for (int e = 0; e <= n; ++e)
			{
				if (e == n)
				{
					printf("Invalid size.\n");
					exit(0);
				}
				swap(p[e], p[e + random()%(n - e)]);
				int k = p[e];
				if (visited[k])
					continue;
				int sizeq = 0;
				q[sizeq++] = k;
				bool is_complete = true;
				for (int i = 0; i < size; ++i)
				{
					int next;
					while (sizeq > 0)
					{
						int pos = random() % sizeq;
						next = q[pos];
						if (visited[next])
							swap(q[pos], q[--sizeq]);
						else
							break;
					}
					if (sizeq == 0)
					{
						is_complete = false;
						break;
					}
					visited[next] = true;
					w[i] = next;
					for (int j = 0; j < degree[next]; ++j)
					{
						int other = g[next][j];
						if (!visited[other])
							q[sizeq++] = other;
					}
				}
				if (is_complete)
					break;
			}
#ifdef DEBUG
			
#endif
			for (int i = 0; i < size; ++i)
				set_c(w[i], 1);
			delete[] visited;
			delete[] q;
			delete[] w;
			delete[] p;
		}
		bool is_biclique = false;
		while (1)
		{
			for (; maxw[0] >= 0 && h[0][maxw[0]].empty(); --maxw[0]);
			for (; minw[0] <= n && h[1][minw[1]].empty(); ++minw[1]);
			if (maxw[0] < 0 || maxw[1] > n)
				break;
#ifdef DEBUG
			int _maxw0 = 0, _minw1 = n + 1;
			for (int i = 0; i < n; ++i)
			{
				if (c[i] == 0 && w[i] > _maxw0) _maxw0 = w[i];
				if (c[i] == 1 && w[i] < _minw1) _minw1 = w[i];
			}
			ASSERT(_maxw0 == maxw[0]);
			ASSERT(_minw1 == minw[1]);
			for (int i = 0; i < n; ++i)
			{
				int _w = 0;
				for (int j = 0; j < degree[i]; ++j)
					if (c[g[i][j]])
						_w++;
				ASSERT(_w == w[i]);
			}
			int counter = 0;
			for (int i = 0; i < n; ++i)
				if (c[i])
					counter++;
			ASSERT(counter == size);
			for (int b = 0; b < 2; ++b)
				for (int i = 0; i <= n; ++i)
					for (int j = 0; j < h[b][i].size(); ++j)
					{
						int idx = h[b][i][j];
						ASSERT(idx >= 0 && idx < n && c[idx] == b && w[idx] == i);
					}
#endif
			int alpha = maxw[0];
			int beta = minw[1];
			if (alpha > beta + 1)
			{
				int p_alpha = h[0][alpha][rand() % SIZE(h[0][alpha])];
				int p_beta = h[1][beta][rand() % SIZE(h[1][beta])];
#ifdef DEBUG
				ASSERT(c[p_alpha] == 0);
				ASSERT(c[p_beta] == 1);
#endif
				set_c(p_alpha, 1);
				set_c(p_beta, 0);
			}
			else if (alpha == beta + 1)
			{
				bool is_exists = false;
				VI &v1 = h[0][alpha];
				VI &v2 = h[1][beta];
				int s1 = SIZE(v1);
				int s2 = SIZE(v2);
				int *p1 = new int[s1];
				int *p2 = new int[s2];
				bool *visited = new bool[s2];
				for (int i = 0; i < s1; ++i)
					p1[i] = i;
				for (int i = 0; i < s2; ++i)
					p2[i] = i;
				for (int i = 0; i < s1; ++i)
					swap(p1[i], p1[rand() % (i + 1)]);
				for (int i = 0; i < s2; ++i)
					swap(p2[i], p2[rand() % (i + 1)]);
				for (int i = 0; !is_exists && i < s1; ++i)
				{
					int p_alpha = v1[p1[i]];
					for (int j = 0; j < s2; ++j)
						visited[j] = false;
					for (int j = 0; !is_exists && j < degree[p_alpha]; ++j)
					{
						int p_beta = g[p_alpha][j];					
						if (c[p_beta] == 1 && w[p_beta] == beta)
							visited[pos[p_beta]] = true;
					}
					for (int j = 0; !is_exists && j < s2; ++j)
						if (!visited[p2[j]])
						{
							is_exists = true;
							int p_beta = v2[p2[j]];
#ifdef DEBUG
							ASSERT(c[p_alpha] == 0);
							ASSERT(c[p_beta] == 1);
#endif
							set_c(p_alpha, 1);
							set_c(p_beta, 0);
						}
				}
				delete[] p1;
				delete[] p2;
				delete[] visited;
				if (!is_exists)
				{
					is_biclique = true;
					break;
				}
			}
			else
				break;
		}
		VI states;
		for (int i = 0; i < n; ++i)
			if (c[i])
				states.push_back(i);
		if (unique_sets->find(states) != unique_sets->end())
		{
			printf("duplicated.\n");
			if (is_biclique)
				printf("BI-CLIQUE\n");
			continue;
		}
		unique_sets->insert(states);
		int total_degree = 0;
		int total_inter_degree = 0;
		for (int i = 0; i < n; ++i)
			if (c[i])
			{
				total_degree += degree[i];
				total_inter_degree += w[i];
			}
		sum_total_degree += total_degree;
		sum_total_intra_degree += total_inter_degree;
		sum_alpha += maxw[0];
		sum_beta += minw[1] + 1;
		sum_density += (double)total_inter_degree / size / size;
		printf("size = %d   alpha = %d   beta = %d   idx = %d\n", size, maxw[0], minw[1] + 1, (int)unique_sets->size());
		if (is_biclique)
			printf("BI-CLIQUE\n");
		fprintf(f,"%d ", minw[1]);
		for (int i = 0; i < n; ++i)
			if (c[i])
				fprintf(f, "%d ", i);
		fprintf(f, "\n");
		fflush(f);
	}
	fclose(f);
}

void solve(char *datafilename, char *outputfilename, int size, int count, bool is_pure_random)
{
	gz_datafilename = datafilename;
	gz_outputfilename = outputfilename;
	srand(1028);
	init();
	if (size < 1 || size > n)
	{
		printf("Invalid size.\n");
		return;
	}
	unique_sets = new set<VI>;
	sum_density = sum_alpha = sum_beta = sum_total_degree = sum_total_intra_degree = 0;
	process(size, count, is_pure_random);
	int unique_count = (int)unique_sets->size();
	printf("unique_count = %d\n", unique_count);
	printf("average_density = %.6lf\n", sum_density / unique_count);
	printf("average_alpha = %.6lf\n", sum_alpha / unique_count);
	printf("average_beta = %.6lf\n", sum_beta / unique_count);
	printf("average_total_degree = %.6lf\n", sum_total_degree / unique_count);
	printf("average_total_intra_degree = %.6lf\n", sum_total_intra_degree / unique_count);
	delete unique_sets;
	discard();
}

int main(int argc, char ** args)
{
	//freopen("test4b.log", "w", stdout);
#ifdef DEBUG
	printf("DEBUG MODEL,  REALLY SLOW\n\n");
#endif
	char datafilename[1024], outputfilename[1024];
	int size, count;
	bool is_pure_random = IS_PURE_RANDOM_DEFAULT;
#ifndef LOCAL
	if (argc != 5 && argc != 6)
	{
		printf("Usage : [exec] datafilename outputfilename clustersize clustercount [-r]\n");		
		return 0;
	}
	strcpy(datafilename, args[1]);
	strcpy(outputfilename, args[2]);
	sscanf(args[3], "%d", &size);
	sscanf(args[4], "%d", &count);
	if (argc == 6 && strcmp(args[5], "-r") == 0)
		is_pure_random = true;
#else
	strcpy(datafilename, "..\\..\\Data\\GCRBC\\gc_data.txt");
	strcpy(outputfilename, "gc500d.txt");
	size = 500;
	count = 100;
	is_pure_random = true;
#endif
	printf("datafile : %s\n", datafilename);
	printf("outputfile : %s\n", outputfilename);
	printf("size = %d\n", size);
	printf("count = %d\n", count);
#ifdef LOCAL
	double __time1 = (double)clock();
#endif
	solve(datafilename, outputfilename, size, count, is_pure_random);
#ifdef LOCAL
	double __time2 = (double)clock();
	printf("#LOG : TimeUsed = %.6lf\n", (__time2 - __time1) / CLOCKS_PER_SEC);
#endif
#ifdef DEBUG
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
