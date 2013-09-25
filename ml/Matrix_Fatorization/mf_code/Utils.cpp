#include "Utils.h"
#include <algorithm>
#include <cstdlib>
using namespace std;

pair<int, double> GetPairFromStr(char *str) {
	int p;
	for (p = 0; str[p]; p++)
		if (str[p] == ':')
			break;
	pair<int, double> ret = make_pair(-1, 0);
	if (str[p] == ':') {
		str[p] = 0;
		ret = make_pair(atoi(str), atof(str + p + 1));
		str[p] = ':';		
	}
	return ret;
}

int GetRand() {
	return ((rand() & 32767) << 15) + (rand() & 32767);
}

void Shuffle(int *pm, int n) {
	for (int i = 1; i < n; i++) swap(pm[i], pm[GetRand() % (i + 1)]);
}

double Sigmoid(double val) {
	return 1 / (1 + exp(-val));
}

double Sqr(const double &val) {
	return val * val;
}