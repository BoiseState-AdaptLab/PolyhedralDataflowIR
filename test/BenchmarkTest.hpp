#ifndef _BENCHMARKTEST_HPP_
#define _BENCHMARKTEST_HPP_

#include <initializer_list>
using std::initializer_list;
#include <map>
using std::map;
#include <string>
using std::string;
using std::to_string;
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <vector>
using std::vector;
#include <sys/time.h>
#include <gtest/gtest.h>
using namespace testing;
#include <util/Strings.hpp>

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

#define GTEST_COUT cout << "[   INFO   ] "
#define GTEST_CERR cerr << "[  ERROR   ] "

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
            _startTime = Now();
        }

        void Stop() noexcept {
            _stopTime = Now();
            _runTime = (_stopTime - _startTime);
        }

        double Speedup() const {
            double p = _runTime;
            double s = _evalTime;
            double r = 0.0;
            //double n = (double) GetNumThreads();

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

        virtual void SetUp(initializer_list<string> args) {
            unsigned argc = args.size();
            vector<string> argv(args.begin(), args.end());

            string key, val;
            for (unsigned i = 0; i < argc; i++) {
                string arg = argv[i];
                if (arg[0] == '-') {
                    key = Strings::ltrim(arg);
                } else {
                    val = arg;
                }
                if (!key.empty()) {
                    _args[key] = val;
                    key = "";
                }
            }
        }

        virtual void Evaluate() = 0;

        virtual void Verify() {
            _evalTime = 0.0;
            double runTime = _runTime;

            Start();
            Evaluate();
            Stop();

            _evalTime = _runTime;
            _runTime = runTime;

            GTEST_COUT << "RunTime = " << _runTime << ", EvalTime = " << _evalTime
                       << ", Ratio = " << _evalTime / _runTime << endl;
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
                    GTEST_CERR << "Found " << testData[i] << ", Expected " << refData[i] << ", Position " << i << endl;
                }
            }
            return index;
        }

        template <typename T>
        static inline void Write(const vector<T>& elems, const char delim = ',') {
            Write(cerr, elems, delim);
        }

        template <typename T>
        static inline void Write(const string& file, const vector<T>& elems, const char delim = ',') {
            ofstream ofs(file.c_str());
            Write(ofs, elems, delim);
        }

        template <typename T>
        static inline void Write(ostream& os, const vector<T>& elems, const char delim = ',') {
            unsigned endpos = elems.size() - 1;
            for (unsigned i = 0; i <= endpos; i++) {
                os << elems[i];
                if (i < endpos) {
                    os << delim;
                }
            }
            os << endl;
        }

        string _name = "";

        double _startTime;
        double _stopTime;
        double _runTime;
        double _evalTime;

        map<string, string> _args;

    private:
        double Now() {
            struct timeval tval;
            gettimeofday(&tval, NULL);
            return (double) tval.tv_sec + (((double) tval.tv_usec) / 1000000);
        }
    };
}


#endif
