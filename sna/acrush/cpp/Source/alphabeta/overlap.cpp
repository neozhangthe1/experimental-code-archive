#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;

#define SIZE(A) ((int)A.size())

char s[1 << 20];
map<string, int> M;
string *ps;

int get_id(string s)
{
	if (M.find(s) != M.end())
		return M[s];
	int size = SIZE(M);
	return M[s] = size;
}

int main()
{
	FILE *f = fopen("..\\socialgraph.data", "r");
	while (fscanf(f, "%s", s) != -1)
		get_id(s);
	fclose(f);
	int n = SIZE(M);
	ps = new string[n];
	for (map<string, int>::iterator it = M.begin(); it != M.end(); ++it)
		ps[it->second] = it->first + " #";
	FILE *f3 = fopen("..\\userinfo.data", "r");
	while (1)
	{
		s[0] = 0;
		fgets(s, 1 << 20, f3);
		if (s[0] == 0)
			break;
		istringstream sin(s);
		string str;
		sin >> str;
		if (M.find(str) == M.end())
			continue;
		int pos = M[str];
		ps[pos] = s;
		ps[pos] = ps[pos].substr(0, ps[pos].length() - 1);
	}
	fclose(f3);
	for (int sz = 250; sz <= 650; sz += 50)
	{
		printf("sz = %d\n", sz);
		char s1[1024], s2[1024];
		sprintf(s1, "twl_core%d.txt", sz);
		sprintf(s2, "twl_core%d_idx.txt", sz);
		FILE *f1 = fopen(s1, "r");
		FILE *f2 = fopen(s2, "w");
		int size;
		while (fscanf(f1, "%d", &size) != -1)
		{
			fprintf(f2, "%d\n", size);
			for (int i = 0; i < size; ++i)
			{
				int idx;
				fscanf(f1, "%d", &idx);
				fprintf(f2, "%9d : %s\n", idx, ps[idx].c_str());
			}
			fprintf(f2, "\n");
		}
		fclose(f1);
		fclose(f2);
	}
	return 0;
}