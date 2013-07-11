#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <algorithm>
#include <utility>

using namespace std;

typedef pair<int, int> ipair;

int n, m;
int *p;
int *d;
ipair *q;
int *c;
int s[2][2];
int h[2];

int main()
{
//	freopen("..\\..\\Data\\amazon\\ama_data.txt", "r", stdin);
//	freopen("..\\..\\Data\\roadNet-PA\\rd_data.txt", "r", stdin);
//	freopen("..\\..\\Data\\web\\web_data.txt", "r", stdin);
	freopen("..\\..\\Data\\slashdot\\st_data.txt", "r", stdin);
	scanf("%d%d", &n, &m);
	p = new int[m + m];
	for (int i = 0; i < m + m; ++i)
		scanf("%d", &p[i]);
	d = new int[n];
	for (int i = 0; i < n; ++i)
		d[i] = 0;
	for (int i = 0; i < m + m; ++i)
		d[p[i]]++;
	q = new ipair[n];
	for (int i = 0; i < n; ++i)
		q[i] = make_pair(-d[i], i);
	sort(q, q + n);
	memset(s, 0, sizeof(s));
	c = new int[n];
	for (int i = 0; i < n; ++i)
		c[i] = 0;
	for (int i = 0; i < n / 10; ++i)
		c[q[i].second] = 1;
	for (int i = 0; i < m + m; i += 2)
		++s[c[p[i]]][c[p[i + 1]]];
	h[0] = h[1] = 0;
	for (int i = 0; i < n; ++i)
		++h[c[i]];
	s[0][1] += s[1][0];
	s[1][0] = 0;
	printf("%.3lf\n", 1e6 * (double)s[0][0] / h[0] / h[0] / 2.0);
	printf("%.3lf\n", 1e6 * (double)s[0][1] / h[0] / h[1]);
	printf("%.3lf\n", 1e6 * (double)s[1][1] / h[1] / h[1] / 2.0);
	return 0;
}