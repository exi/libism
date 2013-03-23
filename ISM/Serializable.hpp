#pragma once

#include <ostream>
#include <boost/shared_ptr.hpp>

namespace ISM {
    class Serializable {
        public:
            virtual void serialize(std::ostream& strm) const = 0;
    };
    typedef boost::shared_ptr<Serializable> SerializablePtr;
}
