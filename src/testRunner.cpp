#include <ISM/TableHelper.hpp>
#include <ISM/RecordedPattern.hpp>
#include <ISM/Recognizer.hpp>
#include <ISM/Trainer.hpp>
#include <boost/program_options.hpp>
#include <string>
#include <set>
#include <stack>
#include <queue>
#include <utility>
#include <boost/filesystem.hpp>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

using namespace ISM;
using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct TestSpec {
        int setCount;
        bool useClustering;
        int objectCount;
        double sensitivity;
        bool useId;
        bool useType;
};

struct TestResult {
        TestSpec spec;
        double avgRuntime;
        double avgConfidence;
};

void removeFileIfExists(const string& filename) {
    if (fs::exists(filename)) {
        fs::remove(filename);
    }
}

void printTest(const TestSpec& t) {
    cerr << t.setCount << "," << t.objectCount << "," << t.sensitivity << "," << t.useClustering
            << "," << t.useType << "," << t.useId << endl;
}

void printTestResult(const TestResult& r) {
    cerr << r.spec.setCount << "," << r.spec.objectCount << "," << r.spec.sensitivity << "," << r.spec.useClustering
            << "," << r.spec.useType << "," << r.spec.useId << "," << r.avgRuntime << "," << r.avgConfidence << endl;
}

void writeTestResult(const TestResult& r, ofstream& csvFile) {
        csvFile << r.spec.setCount << "," << r.spec.objectCount << "," << r.spec.sensitivity << "," << r.spec.useClustering
                << "," << r.spec.useType << "," << r.spec.useId << "," << r.avgRuntime << "," << r.avgConfidence
                << endl;
}

int main(int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")("database-file,d",
            po::value<string>()->default_value("record.sqlite"), "database file to use")("test-database-file,t",
            po::value<string>()->default_value("test.sqlite"), "database file write tests to")("csv-file,c",
            po::value<string>()->default_value("results.csv"), "csv file to write results");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cerr << desc << "\n";
        return 0;
    }

    vector<int> setCounts = { 10, 50, 100, 150, 200, 250, 300, 350, 400 };
    vector<bool> useClusterings = { true, false };
    vector<int> objectCounts = { 1, 2, 3, 4, 5, 6 };
    vector<double> sensitivities = {
            0.5, 0.4, 0.3, 0.2, 0.1,
            0.09, 0.08, 0.07, 0.06, 0.05, 0.04, 0.03, 0.02, 0.01
    };
    vector<bool> useIds = { true };
    vector<bool> useTypes = { true };

    queue<TestSpec> tests;
    stack<TestResult> testResults;

    for (auto setCount : setCounts) {
        for (auto useClustering : useClusterings) {
            for (auto objectCount : objectCounts) {
                for (auto sensitivity : sensitivities) {
                    for (auto useId : useIds) {
                        for (auto useType : useTypes) {
                            TestSpec t = { setCount, useClustering, objectCount, sensitivity, useId, useType };
                            tests.push(t);
                        }
                    }
                }
            }
        }
    }


    auto sourceFile = vm["database-file"].as<string>();
    auto testFile = vm["test-database-file"].as<string>();

    auto csvFilename = vm["csv-file"].as<string>();
    removeFileIfExists(csvFilename);
    ofstream csvFile;
    csvFile.open(csvFilename.c_str());
    csvFile << "setCount,objectCount,sensitivity,useClustering,useType,useId,runtime,confidence" << endl;

    while (!tests.empty()) {
        removeFileIfExists(testFile);
        auto test = tests.front();
        tests.pop();

        printTest(test);

        auto source = TableHelper(sourceFile);
        auto target = new TableHelper(testFile);
        set<pair<string, string> > typesAndIds;
        auto patternName = source.getRecordedPatternNames()[0];

        auto pattern = source.getRecordedPattern(patternName);
        for (auto& set : pattern->objectSets) {
            for (auto& obj : set->objects) {
                auto p = make_pair(obj->type, obj->observedId);
                if (typesAndIds.find(p) == typesAndIds.end()) {
                    typesAndIds.insert(p);
                }
            }
        }

        set<pair<string, string> > typesAndIdsToUse;
        while (typesAndIdsToUse.size() < (size_t)test.objectCount) {
            auto first = typesAndIds.begin();
            typesAndIdsToUse.insert(*first);
            typesAndIds.erase(first);
        }

        cerr << "creating test database";
        int insertedSets = 0;
        string patternNameToUse;
        for (auto& set : pattern->objectSets) {
            ObjectSetPtr newSet(new ObjectSet());
            for (auto& obj : set->objects) {
                auto p = make_pair(obj->type, obj->observedId);
                if (typesAndIdsToUse.find(p) != typesAndIdsToUse.end()) {
                    newSet->insert(obj);
                }
            }
            target->insertRecordedObjectSet(newSet, patternName);
            insertedSets++;
            cerr << ".";
            cerr.flush();
            if (insertedSets >= test.setCount) {
                break;
            }
        }
        cerr << " done" << endl;

        delete (target);

        auto trainer = new Trainer(testFile);
        if (!test.useClustering) {
            trainer->setUseClustering(false);
        }
        trainer->trainPattern();
        delete trainer;

        TestResult testResult = { test, 0, 0 };
        target = new TableHelper(testFile);
        cerr <<"recognizing";
        for (int i = 0; i < insertedSets; i++) {
            auto testSet = target->getRecordedObjectSet(i);

            for (auto& obj : testSet->objects) {
                if (!test.useType) {
                    obj->type = "";
                }
                if (!test.useId) {
                    obj->observedId = "";
                }
            }

            Recognizer recognizer(testFile, test.sensitivity);

            struct timeval start, end;

            gettimeofday(&start, NULL);

            auto results = recognizer.recognizePattern(testSet, 0, 1);

            gettimeofday(&end, NULL);

            long mtime, seconds, useconds;
            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;

            mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

            testResult.avgRuntime += mtime;

            for (auto& result : results) {
                if (result->patternName == patternName) {
                    testResult.avgConfidence += result->confidence;
                }
            }
            cerr<<".";
            cerr.flush();
        }

        cerr<<"done"<<endl;
        delete (target);

        testResult.avgConfidence /= (double) insertedSets;
        testResult.avgRuntime /= (double) insertedSets;

        testResults.push(testResult);
        printTestResult(testResult);
        writeTestResult(testResult, csvFile);
    }

    csvFile.close();

    return 0;
}
