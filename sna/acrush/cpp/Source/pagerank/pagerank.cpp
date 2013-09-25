#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <algorithm>
#include <utility>

using namespace std;

int n, m;
int *degree;
pair<double, int> *q;
bool *c;
double *f1, *f2;

int main()
{
	freopen("..\\gc_data.txt","r",stdin);
	scanf("%d%d", &n, &m);
	int *edges = new int[m + m];
	for (int i = 0; i < m + m; ++i)
		scanf("%d", &edges[i]);
	degree = new int[n];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	for (int i = 0; i < m + m; ++i)
		++degree[edges[i]];
	f1 = new double[n];
	f2 = new double[n];
	for (int i = 0; i < n; ++i)
		f1[i] = 1;
	for (int step = 0; step < 10000; ++step)
	{
		for (int i = 0; i < n; ++i)
			f2[i] = 0;
		for (int i = 0; i < n; ++i)
			f1[i] /= degree[i];
		for (int i = 0; i < m; ++i)
		{
			int a = edges[i + i];
			int b = edges[i + i + 1];
			f2[a] += f1[b];
			f2[b] += f1[a];
		}
		for (int i = 0; i < n; ++i)
			f1[i] = f2[i];
	}
	q = new pair<double, int> [n];
	for (int i = 0; i < n; ++i)
		q[i] = make_pair(f1[i], i);
	sort(q, q + n);
	reverse(q, q + n);
	c = new bool[n];
	for (int i = 0; i < n; ++i)
		c[i] = false;
	for (int i = 0; i < n / 10; ++i)
		c[q[i].second] = true;
	int new_m = 0;
	for (int i = 0; i < m; ++i)
		if (c[edges[i + i]] && c[edges[i + i + 1]])
			++new_m;
	freopen("..\\gc_data_pr10p.txt","w",stdout);
	printf("%d %d\n", n, new_m);
	for (int i = 0; i < m; ++i)
		if (c[edges[i + i]] && c[edges[i + i + 1]])
			printf("%d %d\n", edges[i + i], edges[i + i + 1]);
	return 0;
}