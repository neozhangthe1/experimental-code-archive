#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <algorithm>
#include <utility>

using namespace std;

int n, m;
int *degree;
pair<int, int> *q;
bool *c;

int main()
{
	freopen("..\\gc_data.txt","r",stdin);
	freopen("..\\gc_data_10p.txt","w",stdout);
	scanf("%d%d", &n, &m);
	int *edges = new int[m + m];
	for (int i = 0; i < m + m; ++i)
		scanf("%d", &edges[i]);
	degree = new int[n];
	for (int i = 0; i < n; ++i)
		degree[i] = 0;
	for (int i = 0; i < m + m; ++i)
		++degree[edges[i]];
	q = new pair<int, int> [n];
	for (int i = 0; i < n; ++i)
		q[i] = make_pair(degree[i], i);
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
	printf("%d %d\n", n, new_m);
	for (int i = 0; i < m; ++i)
		if (c[edges[i + i]] && c[edges[i + i + 1]])
			printf("%d %d\n", edges[i + i], edges[i + i + 1]);
	return 0;
}