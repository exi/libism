#include <boost/shared_ptr.hpp>
#include <vector>
#include "ObjectSet.hpp"
#include "Pose.hpp"
#include "Point.hpp"

namespace ISM {
    struct VotingBinResult {
        ObjectSetPtr objects;
        PosePtr referencePose;
        double confidence;
        std::vector<PointPtr> idealPoints;

        VotingBinResult(const VotingBinResult& other): confidence(confidence) {
            objects = ObjectSetPtr(new ObjectSet(*(other.objects)));
            referencePose = PosePtr(new Pose(*(other.referencePose)));
        }

        VotingBinResult(ObjectSetPtr& os, PosePtr& p, double c): objects(os), referencePose(p), confidence(c) {};
        VotingBinResult(ObjectSetPtr& os, PosePtr& p, double c, std::vector<PointPtr> i): objects(os), referencePose(p), confidence(c), idealPoints(i) {};
    };

    typedef boost::shared_ptr<VotingBinResult> VotingBinResultPtr;
} /* VotingBinResult */
