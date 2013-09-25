#include "Conf.h"
#include <cstring>
using namespace std;

void Conf::SetDefault() {
	task = "-est";
	train_file = "train.txt";
	test_file = "test.txt";
	pred_file = "";
	src_model_file = "";
	dst_model_file = "";

	max_iter = 50;

	gradient_step = 0.002;

	eval_interval = 0;

	penalty_square = 0.0001;

	optimization_func = LINEAR;

	dim = 10;
}

bool Conf::LoadConf(int argc, char *argv[]) {	
	bool flag = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-est") == 0 || strcmp(argv[i], "-inf") == 0) {
			task = argv[i];
			flag = true;
		} else if (strcmp(argv[i], "-niter") == 0) {
			max_iter = atoi(argv[i + 1]);
		} else if (strcmp(argv[i], "-trainfile") == 0) {
			train_file = argv[i + 1];
		} else if (strcmp(argv[i], "-testfile") == 0) {
			test_file = argv[i + 1];
		} else if (strcmp(argv[i], "-predfile") == 0) {
			pred_file = argv[i + 1];
		} else if (strcmp(argv[i], "-srcmodel") == 0) {
			src_model_file = argv[i + 1];
		} else if (strcmp(argv[i], "-dstmodel") == 0) {
			dst_model_file = argv[i + 1];
		} else if (strcmp(argv[i], "-gradientstep") == 0) {
			gradient_step = atof(argv[i + 1]);
		} else if (strcmp(argv[i], "-penalty") == 0) {
			penalty_square = atof(argv[i + 1]);
		} else if (strcmp(argv[i], "-dim") == 0) {
			dim = atoi(argv[i + 1]);
		} else if (strcmp(argv[i], "-eval") == 0) {
			eval_interval = atoi(argv[i + 1]);
		} else if (strcmp(argv[i], "-func") == 0) {
			if (argv[i + 1][0] == 's')
				optimization_func = SIGMOID;
			else optimization_func = LINEAR;
		}
	}
	return flag;
}

void Conf::ShowUsage() {
	printf("MatrixFactorization                                          \n");
    printf("     by Zhanpeng Fang, Tsinghua University                   \n");
    printf("                                                             \n");
    printf("Usage: mf <task> [options]	                                 \n");
    printf(" Options:                                                    \n");
    printf("   task: -est,-inf                                           \n");    
    printf("\n");
    printf("   -niter int           : number of iterations                                            \n");
    printf("   -srcmodel string     : the model to load                                               \n");
    printf("   -dstmodel string     : model file to save                                              \n");
    printf("   -func string         : optimization functions (linear/sigmoid), default: linear        \n");
    printf("   -gradientstep double : learning rate                                                   \n");        
    printf("   -penalty double      : penalty parameter                                               \n");        
	printf("   -trainfile string    : train file                                                      \n");
    printf("   -testfile string     : test file                                                       \n");
	printf("   -predfile string     : prediction result file, default: NULL                           \n");
	printf("   -eval int            : evaluation interval(0 for no evaluation), default: 0            \n");
	printf("   -dim int             : hidden vector dimension, default: 10                            \n");

}