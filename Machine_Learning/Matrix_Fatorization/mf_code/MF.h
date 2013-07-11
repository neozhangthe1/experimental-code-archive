#pragma once
#include "Conf.h"
#include "DataSet.h"
using namespace std;

class MF {
public:
	int num_user;
	int num_item;
	int num_feature;
	int dim; 
	int num_iter;
	double gradient_step;
	double penalty_square;

	GlobalDataSet *g_data;
	Conf *conf;

	double **uv, **iv;
	double *w;
	double base;

	void InitTrain(Conf *conf, GlobalDataSet *g_data);

	void InitParameter();

	void Train();

	double Predict(double *pred_val);

	void GeneratePredictionFile();

	void LoadModel(const char *model_path);
	void SaveModel(const char *model_path);
};