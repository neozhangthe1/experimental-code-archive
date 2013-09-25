#pragma once

#include <string>
using namespace std;

const int LINEAR = 0; 
const int SIGMOID = 1;
const int MAX_BUF = 65536;

class Conf {
public:
	string task;
	string train_file;
	string test_file;
	string pred_file;
	string src_model_file;
	string dst_model_file;

	int max_iter;

	double gradient_step;

	int eval_interval;

	double penalty_square;

	int optimization_func;

	int dim;

	Conf() { SetDefault(); }
	void SetDefault();

	bool LoadConf(int argc, char *argv[]);
	static void ShowUsage();
};