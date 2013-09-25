#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>
#include <fstream>
#include <map>

using namespace std;

#define SIZE(A) ((int)A.size())
typedef pair<int,int> ipair;
#define MP(A,B) make_pair(A,B)
typedef vector<int> VI;

int n;
map<string,int> M;

int get_id(string key)
{
	if (M.find(key)!=M.end()) return M[key];
	int size=SIZE(M);
	return M[key]=size;
}
void powerlaw(int n,int d[])
{
	int md=0;
	for (int i=0;i<n;i++) if (d[i]>md) md=d[i];
	int *cd=new int[md+1];
	for (int i=0;i<=md;i++) cd[i]=0;
	for (int i=0;i<n;i++) cd[d[i]]++;
/*
	for (int i=0;i<=md;i++)
		printf("%d %d\n",i,cd[i]);
	printf("\n");
*/
	double x[50],y[50];
	for (int i=0;i<50;i++)
	{
		x[i]=log((double)max(1,i));
		y[i]=log((double)max(1,cd[i]));
	}
	double A=0,B=0,C=0,D=0;
	int s = 20;
	for (int i=0;i<s;i++)
	{
		A+=x[i]*x[i];
		B+=x[i];
		C+=x[i]*y[i];
		D+=y[i];
	}
	double slope=(C*s-B*D)/(A*s-B*B);
	double height=(A*D-C*B)/(A*s-B*B);
	printf("slope = %.6lf\n",slope);
	printf("height = %.6lf\n",height);
/*
	for (int i=1;i<50;i++)
		printf("%.6lf\n",log((double)i));
	printf("\n");
	for (int i=1;i<50;i++)
		printf("%.6lf\n",log((double)cd[i]));
	printf("\n");
*/
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
	int c1=0,c2=0;
	for (int i=0;i<SIZE(p);i+=2)
		if (p[i]<p[i+1])
			c1++;
		else
			c2++;
	printf("%d %d\n",c1,c2);
//	freopen("degree_log.txt","w",stdout);
	int n=SIZE(M);
	int *d1=new int[n];
	int *d2=new int[n];
	printf("n = %d\n",n);
	for (int i=0;i<n;i++) d1[i]=d2[i]=0;
	for (int i=0;i<SIZE(p);i+=2)
	{
		int a=get_id(p[i]);
		int b=get_id(p[i+1]);
		d1[a]++;
		d2[b]++;
	}
	powerlaw(n,d1);
	powerlaw(n,d2);
	return 0;
}