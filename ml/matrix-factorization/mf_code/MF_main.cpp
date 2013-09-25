#include "Conf.h"
#include "DataSet.h"
#include "MF.h"
using namespace std;

void Estimate(Conf *conf) {
	GlobalDataSet *g_data = new GlobalDataSet();
	g_data->LoadData(conf);	
	MF *mf = new MF();
	mf->InitTrain(conf, g_data);
	mf->Train();
	mf->GeneratePredictionFile();
	if (conf->dst_model_file.size() > 0) mf->SaveModel(conf->dst_model_file.c_str());
}

void Inference(Conf *conf) {
	GlobalDataSet *g_data = new GlobalDataSet();
	g_data->LoadData(conf);	
	MF *mf = new MF();
	mf->InitTrain(conf, g_data);	
	mf->GeneratePredictionFile();
}

int main(int argc, char *argv[]) {
	srand(0);
	Conf *conf = new Conf();
	if (!conf->LoadConf(argc, argv)) {
		conf->ShowUsage();
		return 0;
	}

	if (conf->task == "-est") {
		Estimate(conf);
	} else if (conf->task == "-inf") {
		Inference(conf);
	} else {
		conf->ShowUsage();
	}
}