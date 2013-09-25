#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

using namespace std;

#define SIZE(A) ((int)A.size())
typedef pair<int,int> ipair;
#define MP(A,B) make_pair(A,B)
typedef vector<int> VI;

int n;
VI *g,*g2;
map<string,int> M;
VI q;
bool *visited;
VI mp;
int *father;
int *color;

int getfather(int v)
{
	return (father[v]<0)?v:(father[v]=getfather(father[v]));
}

int get_id(string key)
{
	if (M.find(key)!=M.end()) return M[key];
	int size=SIZE(M);
	return M[key]=size;
}

void DFS(int v)
{
	visited[v]=true;
	for (int i=0;i<SIZE(g[v]);i++)
		if (!visited[g[v][i]])
			DFS(g[v][i]);
	q.push_back(v);
}

void DFS2(int v,VI &p)
{
	p.push_back(v);
	visited[v]=true;
	for (int i=0;i<SIZE(g2[v]);i++)
		if (!visited[g2[v][i]])
			DFS2(g2[v][i],p);
}

void DFS3(int v)
{
	if (visited[v]) return;
	visited[v]=true;
	for (int i=0;i<SIZE(g[v]);i++) DFS3(g[v][i]);
}
void DFS4(int v)
{
	if (visited[v]) return;
	visited[v]=true;
	for (int i=0;i<SIZE(g2[v]);i++) DFS4(g2[v][i]);
}

int main()
{
//	freopen("small.txt","r",stdin);
	freopen("socialgraph.data","r",stdin);
	vector<string> p;
/*
	char s1[1024],s2[1024];
	for (int d;scanf("%s%s%d",s1,s2,&d)!=-1;) 
	{ 
		p.push_back(s1); 
		p.push_back(s2);
		get_id(s1); 
		get_id(s2);
	}
*/
	char str[1024];
	while (scanf("%s",str)!=-1) 
	{
		p.push_back(str);
		get_id(str);
	}
	int n=SIZE(M);
	printf("n = %d\n",n);
	printf("m = %d\n",SIZE(p)/2);
	g=new VI[n];
	g2=new VI[n];
	father=new int[n];
	for (int i=0;i<n;i++) father[i]=-1;
	for (int i=0;i<SIZE(p);i+=2)
	{
		int a=get_id(p[i]);
		int b=get_id(p[i+1]);
		int pa=getfather(a);
		int pb=getfather(b);
		if (pa!=pb) father[pa]=pb;
		g[a].push_back(b);
		g2[b].push_back(a);
	}
	int cnt=0;
	for (int i=0;i<n;i++) if (father[i]<0) cnt++;
	printf("cc = %d\n",cnt);
	visited=new bool[n];
	for (int i=0;i<n;i++) visited[i]=false;
	for (int i=0;i<n;i++) if (!visited[i]) DFS(i);
	for (int i=0;i<n;i++) visited[i]=false;
	vector<int> GS;
	mp.clear();
	for (int k=n-1;k>=0;k--)
	{
		int key=q[k];
		if (visited[key]) continue;
		VI p;
		DFS2(key,p);
		GS.push_back(SIZE(p));
		if (SIZE(p)>SIZE(mp))
			mp=p;
	}
	sort(GS.begin(),GS.end());
	reverse(GS.begin(),GS.end());
	printf("top10 : \n");
	for (int i=0;i<SIZE(GS) && i<10;i++)
		printf("%d ",GS[i]);
	printf("\n");
	printf("SCC size = %d\n",SIZE(mp));
	color=new int[n];
	for (int i=0;i<n;i++) color[i]=0;
	for (int i=0;i<n;i++) visited[i]=false;
	for (int i=0;i<SIZE(mp);i++) DFS3(mp[i]);
	int c_left=0;
	for (int i=0;i<n;i++) if (visited[i]) { color[i]=1; c_left++; }
	for (int i=0;i<n;i++) visited[i]=false;
	for (int i=0;i<SIZE(mp);i++) DFS4(mp[i]);
	int c_right=0;
	for (int i=0;i<n;i++) if (visited[i]) { color[i]=2; c_right++; }
	printf("%d %d\n",c_left-SIZE(mp),c_right-SIZE(mp));
	printf("%d\n",n-c_left-c_right+SIZE(mp));
	int cnt2=0;
	for (int i=0;i<n;i++) if (color[i]==2)
		for (int j=0;j<SIZE(g[i]);j++)
		{
			int other=g[i][j];
			if (color[other]==1)
				cnt2++;
		}
	printf("cnt2 = %d\n",cnt2);
	return 0;
}