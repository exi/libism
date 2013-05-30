#include "RecognitionResult.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResult &r) {
        strm << "recognition result: [" << r.patternName << ";" << r.confidence << ";" << r.referencePose << ";"
                << r.recognizedSet;
        size_t s = r.subPatterns.size();
        if (s > 0) {
            strm << ";sub patterns: [";
            size_t c = 1;
            for (auto& subPattern : r.subPatterns) {
                strm << subPattern;
                if (c < s) {
                    strm << ";";
                }
                strm<<"\n";
                c++;
            }
            strm << "]";
        }
        return strm << "]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResultPtr &r) {
        return strm << (*r);
    }
}
