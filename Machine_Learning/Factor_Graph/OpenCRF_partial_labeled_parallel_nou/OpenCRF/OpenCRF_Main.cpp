#include "mpi.h"

#include "Config.h"
#include "DataSet.h"
#include "Transmitter.h"
#include "CRFModel.h"

/*
void DEBUG__AddLinearEdge(DataSet* dataset)
{
    int n = dataset->num_sample;
    for (int i = 0; i < n; i ++)
    {
        DataSample *sample = dataset->sample[i];        
        sample->edge.clear();
        for (int j = 1; j < sample->num_node; j ++)
        {
            {
                DataEdge *e = new DataEdge();
                e->a = j - 1; e->b = j; e->edge_type = 0;
                sample->edge.push_back( e );
            }
        }
        sample->num_edge = sample->edge.size();
    }
    dataset->num_edge_type = 1;
}
*/

void MakeEvaluate(Config* conf, GlobalDataSet* g_dataset, CRFModel* model)
{
    if (conf->my_rank == 0)
    {
        GlobalDataSet* g_testdata = new GlobalDataSet();
        DataSet* testdata = new DataSet();        

        g_testdata->LoadDataWithDict(conf->test_file.c_str(), conf, g_dataset->label_dict, g_dataset->attrib_dict, g_dataset->edge_type_dict);
        testdata->sample = g_testdata->sample;
        testdata->num_sample = g_testdata->sample.size();
        testdata->num_edge_type = g_testdata->num_edge_type;
        testdata->num_attrib_type = g_testdata->num_attrib_type;
        testdata->num_label = g_testdata->num_label;

        for (int i = 0; i < testdata->num_sample; i ++)
        {        
            testdata->sample[i]->num_node = testdata->sample[i]->node.size();
            testdata->sample[i]->num_edge = testdata->sample[i]->edge.size();        

            for (int t = 0; t < testdata->sample[i]->num_node; t ++)
                testdata->sample[i]->node[t]->num_attrib = testdata->sample[i]->node[t]->attrib.size();
        }

        //DEBUG__AddLinearEdge(testdata);

        model->InitEvaluate(conf, testdata);
        model->Evalute();
    }
}

void Estimate(Config* conf)
{
    GlobalDataSet* g_dataset;
    DataSet* dataset;

    if (conf->my_rank == 0) // master
    {
        g_dataset = new GlobalDataSet();
        g_dataset->LoadData(conf->train_file.c_str(), conf);
        g_dataset->label_dict.SaveMappingDict(conf->dict_file.c_str());

        // Assign jobs
        dataset = Transmitter::AssignJobs(g_dataset, conf->num_procs);
    }
    else
    {
        // Get jobs
        dataset = Transmitter::GetJobs();
    }

    printf("num_label = %d\n", dataset->num_label);
    printf("num_sample = %d\n", dataset->num_sample);
    printf("num_edge_type = %d\n", dataset->num_edge_type);
    printf("num_attrib_type = %d\n", dataset->num_attrib_type);

    CRFModel *model = new CRFModel();
    
    model->InitTrain(conf, dataset);    

	printf("Start Training...\n");
	fflush(stdout);
    model->Train();
    
    if (conf->my_rank == 0)
        model->SaveModel(conf->dst_model_file.c_str());
      
    //MakeEvaluate(conf, g_dataset, model);
}

void EstimateContinue(Config* conf)
{
    GlobalDataSet* g_dataset;
    DataSet* dataset;

    if (conf->my_rank == 0) // master
    {
        g_dataset = new GlobalDataSet();
        g_dataset->LoadData(conf->train_file.c_str(), conf);
        g_dataset->label_dict.SaveMappingDict(conf->dict_file.c_str());

        // Assign jobs
        dataset = Transmitter::AssignJobs(g_dataset, conf->num_procs);
    }
    else
    {
        // Get jobs
        dataset = Transmitter::GetJobs();
    }

    //DEBUG__AddLinearEdge(dataset);

    printf("num_label = %d\n", dataset->num_label);
    printf("num_sample = %d\n", dataset->num_sample);
    printf("num_edge_type = %d\n", dataset->num_edge_type);
    printf("num_attrib_type = %d\n", dataset->num_attrib_type);

    CRFModel *model = new CRFModel();
    
    model->InitTrain(conf, dataset);

    if (conf->my_rank == 0)
        model->LoadModel(conf->src_model_file.c_str());

    model->Train();

    if (conf->my_rank == 0)

        model->SaveModel(conf->dst_model_file.c_str());
    
    //MakeEvaluate(conf, g_dataset, model);
}

void Inference(Config* conf)
{
    GlobalDataSet* g_dataset;
    DataSet* dataset;

    if (conf->my_rank == 0) // master
    {
        g_dataset = new GlobalDataSet();
        g_dataset->LoadData(conf->train_file.c_str(), conf);
        g_dataset->label_dict.SaveMappingDict(conf->dict_file.c_str());

        // Assign jobs
        dataset = Transmitter::AssignJobs(g_dataset, conf->num_procs);
    }
    else
    {
        // Get jobs
        dataset = Transmitter::GetJobs();
    }

    //DEBUG__AddLinearEdge(dataset);

    printf("num_label = %d\n", dataset->num_label);
    printf("num_sample = %d\n", dataset->num_sample);
    printf("num_edge_type = %d\n", dataset->num_edge_type);
    printf("num_attrib_type = %d\n", dataset->num_attrib_type);

    CRFModel *model = new CRFModel();
    
    model->InitTrain(conf, dataset);
    if (conf->my_rank == 0)
        model->LoadModel(conf->src_model_file.c_str());
    
    MakeEvaluate(conf, g_dataset, model);
}

int main(int argc, char* argv[])
{
    int         my_rank;
    int         num_procs;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    // Load Configuartion
    Config* conf = new Config();
    if (! conf->LoadConfig(my_rank, num_procs, argc, argv))
    {
        conf->ShowUsage();
        MPI_Finalize();
        exit( 0 );
    }

    if (conf->task == "-est")
    {
        Estimate(conf);
    }
    else if (conf->task == "-estc")
    {
        EstimateContinue(conf);
    }
    else if (conf->task == "-inf")
    {
        Inference(conf);
    }
    else
    {
        Config::ShowUsage();
    }

    MPI_Finalize();
}