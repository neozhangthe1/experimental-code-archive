#include "DataSet.h"
#include "Utils.h"
#include "Conf.h"
#include <iostream>
#include <cmath>
using namespace std;

void DataSet::LoadDataSet(const char *filepath) {
	FILE *fin = fopen(filepath, "r");
	if (fin == NULL) return;
	int nu, ni, ng;
	double score;
	char *buf = new char[MAX_BUF];
	while (fscanf(fin, "%lf %d %d %d", &score, &nu, &ni, &ng) > 0) {
		DataInstance instance;
		instance.score = score;
		for (int i = 0; i < nu; i++) {
			fscanf(fin, "%s", buf);
			pair<int, double> f = GetPairFromStr(buf);			
			instance.user_feature.push_back(f);
		}

		for (int i = 0; i < ni; i++) {
			fscanf(fin, "%s", buf);
			pair<int, double> f = GetPairFromStr(buf);			
			instance.item_feature.push_back(f);
		}

		for (int i = 0; i < ng; i++) {
			fscanf(fin, "%s", buf);
			pair<int, double> f = GetPairFromStr(buf);			
			instance.global_feature.push_back(f);
		}
		instances.push_back(instance);
	}
	fclose(fin);
	delete buf;
}

void DataSet::Check() {
	for (int i = 0; i < instances.size(); i++) {
		DataInstance &instance = instances[i];
		printf("%d %d %d", instance.user_feature.size(), instance.item_feature.size(), instance.global_feature.size());

		for (int j = 0; j < instance.user_feature.size(); j++)
			printf(" %d:%lf", instance.user_feature[j].first, instance.user_feature[j].second);
		for (int j = 0; j < instance.item_feature.size(); j++)
			printf(" %d:%lf", instance.item_feature[j].first, instance.item_feature[j].second);
		for (int j = 0; j < instance.global_feature.size(); j++)
			printf(" %d:%lf", instance.global_feature[j].first, instance.global_feature[j].second);
		printf("\n");
	}
}

void GlobalDataSet::LoadData(Conf *conf) {
	train_data = new DataSet();
	test_data = new DataSet();
	train_data->LoadDataSet(conf->train_file.c_str());
	test_data->LoadDataSet(conf->test_file.c_str());

	num_user = 0;
	num_item = 0;
	num_feature = 0;
	for (int i = 0; i < train_data->instances.size(); i++) {
		DataInstance &instance = train_data->instances[i];
		for (int j = 0; j < instance.user_feature.size(); j++)
			num_user = max(num_user, instance.user_feature[j].first);
		for (int j = 0; j < instance.item_feature.size(); j++)
			num_item = max(num_item, instance.item_feature[j].first);
		for (int j = 0; j < instance.global_feature.size(); j++)
			num_feature = max(num_feature, instance.global_feature[j].first);
	}

	for (int i = 0; i < test_data->instances.size(); i++) {
		DataInstance &instance = test_data->instances[i];
		for (int j = 0; j < instance.user_feature.size(); j++)
			num_user = max(num_user, instance.user_feature[j].first);
		for (int j = 0; j < instance.item_feature.size(); j++)
			num_item = max(num_item, instance.item_feature[j].first);
		for (int j = 0; j < instance.global_feature.size(); j++)
			num_feature = max(num_feature, instance.global_feature[j].first);
	}
	num_user += 1;
	num_item += 1;
	num_feature += num_user + num_item + 1;
	PrintInfo();
}

void GlobalDataSet::PrintInfo() {
	printf("# of train instances: %d\n", train_data->instances.size());
	printf("# of test instances: %d\n", test_data->instances.size());
	printf("# of users: %d\n", num_user);
	printf("# of items: %d\n", num_item);
	printf("# of global features: %d\n", num_feature - num_user - num_item);
}