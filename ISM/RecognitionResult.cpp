#include "RecognitionResult.hpp"

namespace ISM{
    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResult &r) {
        return strm<<"recognition result: ["<<r.patternName<<";"<<r.referencePose<<";"<<r.recognizedSet<<";"<<r.confidence<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResultPtr &r) {
        return strm<<(*r);
    }
}
