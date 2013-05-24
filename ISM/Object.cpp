#include "Object.hpp"
#include "JsonStream.hpp"

namespace ISM{
    std::ostream& operator<<(std::ostream &strm, const ISM::Object &o) {
        return strm<<"object: ["<<o.type<<";"<<o.observedId<<";"<<o.weight<<";"<<o.confidence<<";"<<o.pose<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectPtr &o) {
        return strm<<(*o);
    }

    void Object::serialize(std::ostream& strm) const {
        strm<<"{"<<std::endl
            <<"\"type\": \""<<this->type<<"\", "<<std::endl
            <<"\"observedId\": \""<<this->observedId<<"\", "<<std::endl
            <<"\"pose\": "<<json(this->pose)<<std::endl
            <<"}"<<std::endl;
    }
}
