#ifndef _BENCHMARKTEST_HPP_
#define _BENCHMARKTEST_HPP_

#include <initializer_list>
using std::initializer_list;
#include <string>
using std::string;
using std::to_string;
#include <iostream>
using std::cerr;
using std::endl;
//#include <chrono>
//using std::chrono::duration;
//using std::chrono::time_point;
//using std::chrono::system_clock;
#include <sys/time.h>
#include <gtest/gtest.h>
using namespace testing;

#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#define omp_get_max_threads() 1
#define omp_get_num_threads() 1
#define omp_set_num_threads(n) n=n
#endif

#ifndef EPSILON
#define EPSILON 0.001
#endif

namespace test {
    class BenchmarkTest : public ::testing::Test {
    public:
        double EvalTime() const {
            return _evalTime;
        }

        double RunTime() const {
            return _runTime;
        }

        void Start() noexcept {
            _runTime = 0.0;
            _startTime = Now(); //system_clock::now();
        }

        void Stop() noexcept {
            _stopTime = Now(); //system_clock::now();
            _runTime = (_stopTime - _startTime); //.count();
        }

        double Speedup() const {
            double p = _runTime;
            double s = _evalTime;
            double r = 0.0;
            //double n = (double) _config.getInt("num_threads");

            // Amdahl's Law:
            if (s != 0.0) {
                r = (1.0 / (1.0 - p + (p/s)));
            }

            return r;
        }

        virtual int GetMaxThreads() const {
            return omp_get_max_threads();
        }

        virtual int GetNumThreads() const {
            return omp_get_num_threads();
        }

        virtual bool SetNumThreads(int nThreads) {
            bool result = false;
            if (nThreads > omp_get_max_threads()) {
                cerr << "--num_threads cannot be more than \n" << omp_get_max_threads();
            } else if (nThreads < 1) {
                cerr << "--num_threads cannot be less than 1\n";
            } else {
                omp_set_num_threads(nThreads);
                result = true;
            }
            return result;
        }

    protected:
        BenchmarkTest(const string& name = "") : _name(name) {}

        virtual ~BenchmarkTest() {}

        virtual void SetUp(initializer_list<string> args) = 0;

        virtual void Evaluate() = 0;

        virtual void Verify() {
            _evalTime = 0.0;
            double runTime = _runTime;

            Start();
            Evaluate();
            Stop();

            _evalTime = _runTime;
            _runTime = runTime;

            fprintf(stderr, "RunTime = %lg, EvalTime = %lg, Ratio = %lg\n", _runTime, _evalTime, _evalTime / _runTime);
        }

        virtual void Execute() = 0;

        virtual void Run() {
            Start();
            Execute();
            Stop();
        }

        virtual void Assert() {};

        virtual int Compare(const double* testData, const double* refData, unsigned size, double eps = EPSILON) {
            int index = -1;
            for (unsigned i = 0; i < size && index < 0; i++) {
                //if (abs((testData[i] - refData[i])/refData[i]) >= eps) {
                if (abs(testData[i] - refData[i]) >= eps) {
                    index = i;
                    cerr << "Found " << testData[i] << ", Expected " << refData[i] << ", Position " << i << endl;
                }
            }
            return index;
        }

        double Now() {
            struct timeval tval;
            gettimeofday(&tval, NULL);
            return (double) tval.tv_sec + (((double) tval.tv_usec) / 1000000);
        }

        string _name = "";

//        time_point<system_clock> _startTime;
//        time_point<system_clock> _stopTime;
        double _startTime;
        double _stopTime;
        double _runTime;
        double _evalTime;
    };
}


#endif
