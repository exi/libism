#pragma once

#include <ostream>
#include "Serializable.hpp"

namespace ISM {
    class json {
        public:
            json(const Serializable* s) : s(s) {};
            json(const SerializablePtr sp) {
                s = sp.get();
            };
            const Serializable* s;
    };
    typedef boost::shared_ptr<json> jsonPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::json &p);
    std::ostream& operator<<(std::ostream &strm, const ISM::jsonPtr &p);
}
