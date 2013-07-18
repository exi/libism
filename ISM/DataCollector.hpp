#pragma once

#include <boost/shared_ptr.hpp>
#include "Tracks.hpp"
#include "Track.hpp"
#include <vector>

namespace ISM {
    struct TracksWithRef {
            TracksPtr tracks;
            TrackPtr refTrack;
            TracksWithRef(TracksPtr tracks, TrackPtr refTrack) : tracks(tracks), refTrack(refTrack) {};
    };

    struct CollectedData {
            bool shouldCollect;
            std::vector<TracksWithRef> tracksWithRef;
    };

    typedef boost::shared_ptr<CollectedData> CollectedDataPtr;

    class DataCollector {
        private:
            DataCollector() {};
            ~DataCollector() {};
        public:
            static CollectedDataPtr getData() {
                static CollectedDataPtr data;
                if (!data) {
                    data = CollectedDataPtr(new CollectedData());
                    data->shouldCollect = false;
                }
                return data;
            }

            static void setCollect(bool newCollect) {
                getData()->shouldCollect = newCollect;
            }

            static bool shouldCollect() {
                return getData()->shouldCollect;
            }

            static void release() {
                bool shouldCollect = getData()->shouldCollect;
                getData().reset();
                getData()->shouldCollect = shouldCollect;
            }
    };
}
