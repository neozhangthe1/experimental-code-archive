#include "MF.h"
#include "Conf.h"
#include "Utils.h"
#include <cstring>
using namespace std;

void MF::InitTrain(Conf *conf, GlobalDataSet *g_data) {
	num_user = g_data->num_user;
	num_item = g_data->num_item;
	num_feature = g_data->num_feature;
	dim = conf->dim;
	gradient_step = conf->gradient_step;
	penalty_square = conf->penalty_square;

	this->g_data = g_data;
	this->conf = conf;

	uv = new double*[num_user];
	for (int i = 0; i < num_user; i++) uv[i] = new double[dim];
	iv = new double*[num_item];
	for (int i = 0; i < num_item; i++) iv[i] = new double[dim];
	w = new double[num_feature];

	if (conf->src_model_file.size() == 0) {
		InitParameter();
	} else {
		LoadModel(conf->src_model_file.c_str());
	}
}

void MF::InitParameter() {	
	for (int i = 0; i < num_user; i++) memset(uv[i], 0, sizeof(double) * dim);
	for (int i = 0; i < num_item; i++) memset(iv[i], 0, sizeof(double) * dim);
	memset(w, 0, sizeof(double) * num_feature);
	base = 0;

	if (g_data->train_data->instances.size() == 0) return;
	//if (conf->optimization_func == SIGMOID) return;

	double num_term = 0;
	double pred_sum = 0;
	for (int i = 0; i < g_data->train_data->instances.size(); i++) {
		DataInstance &instance = g_data->train_data->instances[i];
		double user_sum = 0;
		double item_sum = 0;
		double global_sum = 0;
		for (int j = 0; j < instance.user_feature.size(); j++) user_sum += instance.user_feature[j].second;
		for (int j = 0; j < instance.item_feature.size(); j++) item_sum += instance.item_feature[j].second;
		for (int j = 0; j < instance.global_feature.size(); j++) global_sum += instance.global_feature[j].second;
		num_term += user_sum + item_sum + global_sum + user_sum * item_sum;
		pred_sum += instance.score;
	}
	num_term /= g_data->train_data->instances.size();
	if (num_term == 0) return;
	pred_sum /= g_data->train_data->instances.size();
	pred_sum /= num_term;	
	for (int i = 0; i < num_feature; i++) w[i] = pred_sum * 2 * rand() / RAND_MAX;
	double factor_limit = sqrt(abs(pred_sum / dim));
	if (pred_sum < 0) factor_limit *= -1;
	for (int i = 0; i < num_user; i++)
		for (int j = 0; j < dim; j++)
			uv[i][j] = factor_limit * 2 * rand() / RAND_MAX;
	for (int i = 0; i < num_item; i++)
		for (int j = 0; j < dim; j++)
			iv[i][j] = factor_limit * 2 * rand() / RAND_MAX;
}

void MF::LoadModel(const char *model_path) {
	FILE *fin = fopen(model_path, "rb");
	if (fin == NULL) {
		printf("[ERROR] Source model file does not exist.\n");
		InitParameter();
		return;
	}
	int nu, ni, nf, ndim;
	fread(&nu, sizeof(int), 1, fin);
	fread(&ni, sizeof(int), 1, fin);
	fread(&nf, sizeof(int), 1, fin);
	fread(&ndim, sizeof(int), 1, fin);
	if (nu != num_user || ni != num_item || nf != num_feature || ndim != dim) {
		printf("[ERROR] Source model file does not match the configuration.\n");
		InitParameter();
		fclose(fin);
	}

	fread(&base, sizeof(double), 1, fin);

	for (int i = 0; i < num_user; i++) {
		fread(uv[i], sizeof(double), dim, fin);
	}
	for (int i = 0; i < num_item; i++) {
		fread(iv[i], sizeof(double), dim, fin);
	}	
	
	fread(w, sizeof(double), num_feature, fin);	

	fclose(fin);
}

void MF::SaveModel(const char *model_path) {
	FILE *fout = fopen(model_path, "wb");
	fwrite(&num_user, sizeof(int), 1, fout);
	fwrite(&num_item, sizeof(int), 1, fout);
	fwrite(&num_feature, sizeof(int), 1, fout);
	fwrite(&dim, sizeof(int), 1, fout);

	fwrite(&base, sizeof(double), 1, fout);

	for (int i = 0; i < num_user; i++) {
		fwrite(uv[i], sizeof(double), dim, fout);
	}
	for (int i = 0; i < num_item; i++) {
		fwrite(iv[i], sizeof(double), dim, fout);
	}	
	
	fwrite(w, sizeof(double), num_feature, fout);
	
	fclose(fout);
}

void MF::Train() {
	vector<DataInstance> &instances = g_data->train_data->instances;
	int *pm = new int[instances.size()];
	for (int i = 0; i < instances.size(); i++) pm[i] = i;

	double *p, *q;
	double *tuv = new double[dim];
	double *tiv = new double[dim];
	double tmp_result;
	double *pred_val = new double[g_data->test_data->instances.size()];
	
	for (int iter = 0; iter < conf->max_iter; iter++) {
		double train_rmse = 0;
		Shuffle(pm, instances.size());
		for (int ii = 0; ii < instances.size(); ii++) {			
			DataInstance &instance = instances[pm[ii]];
			memset(tuv, 0, sizeof(double) * dim);
			memset(tiv, 0, sizeof(double) * dim);

			for (int i = 0; i < instance.user_feature.size(); i++) {
				p = uv[instance.user_feature[i].first];
				for (int j = 0; j < dim; j++) 
					tuv[j] += p[j] * instance.user_feature[i].second;
			}

			for (int i = 0; i < instance.item_feature.size(); i++) {
				q = iv[instance.item_feature[i].first];
				for (int j = 0; j < dim; j++)
					tiv[j] += q[j] * instance.item_feature[i].second;
			}

			double est_val = base;
			for (int i = 0; i < dim; i++) est_val += tuv[i] * tiv[i];
			for (int i = 0; i < instance.user_feature.size(); i++) 
				est_val += instance.user_feature[i].second * w[instance.user_feature[i].first];
			for (int i = 0; i < instance.item_feature.size(); i++) 
				est_val += instance.item_feature[i].second * w[instance.item_feature[i].first + num_user];
			for (int i = 0; i < instance.global_feature.size(); i++) 
				est_val += instance.global_feature[i].second * w[instance.global_feature[i].first + num_user + num_item];
			
			if (conf->optimization_func == SIGMOID) {
				tmp_result = est_val;
				est_val = Sigmoid(est_val);
			}

			double e = est_val - instance.score;
			train_rmse += Sqr(e);
			if (conf->optimization_func == SIGMOID) {
				e = e * est_val * est_val * exp(-tmp_result);
			}
			e = e * 2;			
			
			base -= gradient_step * e;

			for (int i = 0; i < instance.user_feature.size(); i++) {
				p = uv[instance.user_feature[i].first];
				for (int j = 0; j < dim; j++)
					p[j] -= gradient_step * (e * tiv[j] * instance.user_feature[i].second + 2 * penalty_square * p[j]);
				w[instance.user_feature[i].first] -= gradient_step * e * instance.user_feature[i].second;
			}

			for (int i = 0; i < instance.item_feature.size(); i++) {
				q = iv[instance.item_feature[i].first];
				for (int j = 0; j < dim; j++)
					q[j] -= gradient_step * (e * tuv[j] * instance.item_feature[i].second + 2 * penalty_square * q[j]);
				w[instance.item_feature[i].first + num_user] -= gradient_step * e * instance.item_feature[i].second;
			}

			for (int i = 0; i < instance.global_feature.size(); i++) {
				w[instance.global_feature[i].first + num_user + num_item] -= gradient_step * e * instance.global_feature[i].second;
			}
		}

		if (instances.size() > 0) train_rmse /= instances.size();
		train_rmse = sqrt(train_rmse);
		printf("[ITER %d] Train RMSE: %.9lf.", iter + 1, train_rmse);
		if (conf->eval_interval > 0 && (iter + 1) % conf->eval_interval == 0) {			
			double test_rmse = Predict(pred_val);
			printf(" Test RMSE: %.9lf.", test_rmse);
		}
		printf("\n");
	}

	delete pm;
	delete tuv;
	delete tiv;
	delete pred_val;
}

double MF::Predict(double *pred_val) {
	double rmse = 0;
	vector<DataInstance> &instances = g_data->test_data->instances;
	double *p, *q;
	double *tuv = new double[dim];
	double *tiv = new double[dim];

	for (int ii = 0; ii < instances.size(); ii++) {			
		DataInstance &instance = instances[ii];
		memset(tuv, 0, sizeof(double) * dim);
		memset(tiv, 0, sizeof(double) * dim);

		for (int i = 0; i < instance.user_feature.size(); i++) {
			p = uv[instance.user_feature[i].first];
			for (int j = 0; j < dim; j++) 
				tuv[j] += p[j] * instance.user_feature[i].second;
		}

		for (int i = 0; i < instance.item_feature.size(); i++) {
			q = iv[instance.item_feature[i].first];
			for (int j = 0; j < dim; j++)
				tiv[j] += q[j] * instance.item_feature[i].second;
		}

		double est_val = base;
		for (int i = 0; i < dim; i++) est_val += tuv[i] * tiv[i];
		for (int i = 0; i < instance.user_feature.size(); i++) 
			est_val += instance.user_feature[i].second * w[instance.user_feature[i].first];
		for (int i = 0; i < instance.item_feature.size(); i++) 
			est_val += instance.item_feature[i].second * w[instance.item_feature[i].first + num_user];
		for (int i = 0; i < instance.global_feature.size(); i++) 
			est_val += instance.global_feature[i].second * w[instance.global_feature[i].first + num_user + num_item];

		if (conf->optimization_func == SIGMOID) {			
			est_val = Sigmoid(est_val);
		}

		rmse += Sqr(est_val - instance.score);
		pred_val[ii] = est_val;
	}
	if (instances.size() > 0) rmse /= instances.size();
	rmse = sqrt(rmse);

	delete tuv;
	delete tiv;

	return rmse;
}

void MF::GeneratePredictionFile() {
	double *pred_val = new double[g_data->test_data->instances.size()];
	double rmse = Predict(pred_val);
	printf("Test RMSE: %.9lf\n", rmse);
	if (conf->pred_file.size() > 0) {
		FILE *fout = fopen(conf->pred_file.c_str(), "w");
		for (int i = 0; i < g_data->test_data->instances.size(); i++)
			fprintf(fout, "%.9lf\n", pred_val[i]);
		fclose(fout);
	}
	delete pred_val;
}