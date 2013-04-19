#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include "ObjectSet.hpp"
#include "Object.hpp"
#include "Pose.hpp"
#include "Track.hpp"

namespace ISM {
    class Tracks {
        public:
            Tracks(std::vector<ObjectSetPtr> sets);
            Tracks(std::vector<TrackPtr> tracks);

            std::vector<TrackPtr> tracks;
            TrackPtr getTrackByTypeAndId(std::string type, std::string observedId);
            std::vector<ObjectSetPtr> toObjectSetVector();
            void replace(std::vector<TrackPtr> tracks, TrackPtr newTrack);
    };

    typedef boost::shared_ptr<Tracks> TracksPtr;
}
