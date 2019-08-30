#ifndef _BCSRBENCHMARK_H_
#define _BCSRBENCHMARK_H_

#include "Benchmark.h"

//<Include>
#include "sparse_matrix.h"      // C implementation of BCSR Matrix
#include "smv.h"
#include "matrix_io.h"
//#include "rose_main.cpp"

using namespace std;

class BCSRBenchmark : public Benchmark {
public:
    BCSRBenchmark() {}

    BCSRBenchmark(int argc, char *argv[]) {
        _config.addParamString("matrix", 'm', "", "-m, path to the CSR matrix file");
        _config.addParamInt("blocksize",'b', 8, "-b, the block size");
        _config.addParamInt("num_threads", 'p', 1, "-p <num_threads>, number of cores");
        _config.parse(argc, (char **) argv);
    }

    virtual ~BCSRBenchmark() {}

    void init() {
        int i;
        _status = true;

        string matrix = _config.getString("matrix");
        cerr << matrix << endl;
        char *filename = (char *) matrix.c_str();
        load_sparse_matrix(filename, &_A);

        _x = ((float *)(malloc(sizeof(float ) * _A.ncols)));
        _b = ((float *)(malloc(sizeof(float ) * _A.nrows)));
        _b2 = ((float *)(malloc(sizeof(float ) * _A.nrows)));

        for (i = 0; i < _A.ncols; i++)
            _x[i] = 1.0;
        for (i = 0; i < _A.nrows; i++) {
            _b[i] = 0;
            _b2[i] = 0;
        }
    }

    void run() {
        init();     // Initialize data...

        if (valid()) {
            // Set number of threads...
            setNumThreads(_config.getInt("num_threads"));

            // Invoke execution function...
            start();    // Start timer...
            spmv(_A.nrows,_A.rows,_A.vals,_b2,_x,_A.cols);
            stop();     // Stop timer...

            // If execution function did not calculate its own runtime, compute it now...
            if (_runTime == 0.0) {
                _runTime = _stopTime - _startTime;
            }

            _meas.setField("RunTime", _runTime);

            // If verification on, invoke evaluation function...
            if (_verify) {
                start();
                //_evalFxn(_inputData, _verifyData, &_evalTime, _config);
                smv(&_A,_x,_b);
                stop();

                // If evaluation function did not calculate its own runtime, compute it now...
                if (_evalTime == 0.0) {
                    _evalTime = _stopTime = _startTime;
                }

                // Compare _outputData and _verifyData...
                _status = spmv_comp(_A.nrows, _b, _b2);
                //_status = _compFxn(_outputData, _verifyData, _config, _errorLoc);

                if (_status) {
                    _meas.setField("verification", "SUCCESS");
                } else {
                    //cout << "Writing to CSV...\n";
                    //toCSV();
                    _meas.setField("verification", "FAILURE");
                }
            }

            finish();   // Perform cleanup, etc...
        } else {
            cerr << "Benchmark initialization failed!\n";
        }
    }

    void finish() {
        free(_x);
        free(_b);
        free(_b2);
        free(_A.rows);
        free(_A.cols);
        free(_A.vals);
    }

    string error() {
        return "";
    }

private:
    struct sparse_matrix _A;
    float *_x;
    float *_b;
    float *_b2;

    // TODO: Update these to match BSR prototypes...
    //function<void(int, int*, float*, float*, float*, int*)> _execFxn;
    //function<void(sparse_matrix*, REAL*, REAL*)> _evalFxn;
    //function<bool(int, float*, float*)> _compFxn;
};

#endif // _BCSRBENCHMARK_H_
