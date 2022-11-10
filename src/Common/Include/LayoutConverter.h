#ifndef __LAYOUT_CONVERTER_H__
#define __LAYOUT_CONVERTER_H__

#include "Include/LayoutData.hpp"

#include <memory>
#include <list>

using GeometryPtr  = std::shared_ptr<Geometry>;
using GeometryList = std::list<std::shared_ptr<Geometry>>;


namespace GeometryConverter
{
    GeometryList
    PathToRectList(const Geometry* Path);

    GeometryList
    SplitSref(
        const Geometry* Sref,
        Coord Min);
}

#endif //!__LAYOUT_CONVERTER_H__