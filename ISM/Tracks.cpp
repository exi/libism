#include "Tracks.hpp"

namespace ISM {
    Tracks::Tracks(std::vector<ObjectSetPtr> sets) {
        size_t setIdx = 0;
        for (auto& set : sets) {
            for (auto& object : set->objects) {
                TrackPtr track = getTrackByTypeAndId(object->type, object->observedId);
                if (!track) {
                    track = TrackPtr(new Track(object->type, object->observedId));
                    tracks.push_back(track);

                    // fill up with empty object pointers
                    for (size_t i = 0; i < setIdx; i++) {
                        track->objects.push_back(ObjectPtr());
                    }
                }

                track->objects.push_back(object);
            }
            // fill up tracks that had no object in this frame
            for (auto& track : tracks) {
                for (size_t i = track->objects.size(); i <= setIdx; i++) {
                    track->objects.push_back(ObjectPtr());
                }
            }
            setIdx++;
        }
    }

    Tracks::Tracks(std::vector<TrackPtr> tracks) {
        this->tracks = tracks;
    }

    TrackPtr Tracks::getTrackByTypeAndId(std::string type, std::string observedId) {
        for (auto& track : tracks) {
            if (track->type == type && track->observedId == observedId) {
                return track;
            }
        }

        return TrackPtr();
    }

    std::vector<ObjectSetPtr> Tracks::toObjectSetVector() {
        std::vector<ObjectSetPtr> ret;
        if (tracks.size() > 0) {
            for (size_t i = 0; i < tracks[0]->objects.size(); i++) {
                ObjectSetPtr set(new ObjectSet());
                for (auto& track : tracks) {
                    if (track->objects[i]) {
                        set->insert(track->objects[i]);
                    }
                }
                ret.push_back(set);
            }
        }

        return ret;
    }

    void Tracks::replace(std::vector<TrackPtr> eraseTracks, TrackPtr newTrack) {
        for (auto& eraseTrack : eraseTracks) {
            for (size_t i = 0; i < tracks.size(); i++) {
                auto track = tracks[i];
                if (eraseTrack == track) {
                    tracks.erase(tracks.begin() + i);
                    break;
                }
            }
        }

        tracks.push_back(newTrack);
    }
}
