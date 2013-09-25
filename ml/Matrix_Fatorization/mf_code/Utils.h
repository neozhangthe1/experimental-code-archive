#pragma once
#include <cstdlib>
#include <algorithm>
using namespace std;

pair<int, double> GetPairFromStr(char *str);

int GetRand();

void Shuffle(int *pm, int n);

double Sigmoid(double val);

double Sqr(const double &val);