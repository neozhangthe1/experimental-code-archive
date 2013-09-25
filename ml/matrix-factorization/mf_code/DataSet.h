#pragma once
#include <vector>
#include <algorithm>
#include <cstdio>
#include "Conf.h"
using namespace std;

class DataInstance {
public:
	vector<pair<int, double> > user_feature;
	vector<pair<int, double> > item_feature;
	vector<pair<int, double> > global_feature;
	double score;	
};

class DataSet {
public:
	vector<DataInstance> instances;

	void LoadDataSet(const char *filepath);

	void Check();
};

class GlobalDataSet {
public:
	DataSet *train_data;
	DataSet *test_data;
	int num_user;
	int num_item;
	int num_feature;

	void LoadData(Conf *conf); 

	void PrintInfo();
};
