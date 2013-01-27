#include "VotingBin.hpp"
#include "MinMaxFinder.hpp"
#include "MathHelper.hpp"

namespace ISM {
    void VotingBin::insert(const VotedPosePtr& vote) {
        if (this->votes.find(vote->source) == this->votes.end()) {
            this->votes.insert(std::make_pair(vote->source, vote));
            this->value += vote->confidence;
        } else if (this->votes[vote->source]->confidence < vote->confidence) {
            this->value -= this->votes[vote->source]->confidence;
            this->votes[vote->source] = vote;
            this->value += vote->confidence;
        }
    }

    const PosePtr VotingBin::getReferencePose() const {
        MinMaxFinder m;

        std::vector<QuaternionPtr> quats;

        for (auto& mapItem : this->votes) {
            ObjectPtr o = mapItem.first;
            VotedPosePtr v = mapItem.second;
            m.add(v->pose->point->x, v->pose->point->y, v->pose->point->z);
            quats.push_back(v->pose->quat);
        }

        return PosePtr(
            new Pose(
                m.getMiddlePoint(),
                MathHelper::getAveragePose(quats)
            )
        );
    }

    const ObjectSetPtr VotingBin::getObjectSet() const {
        ObjectSetPtr os(new ObjectSet());

        for (auto& item : this->votes) {
            os->insert(item.first);
        }

        return os;
    }
}
