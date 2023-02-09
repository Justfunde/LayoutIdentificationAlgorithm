/**
 * @file LayoutConverter.h
 * @author Mikhail Kotlyarov  (m.kotlyarov@elvis.ru)
 * @brief Function declaration for layout object convertions
 * @version 0.1
 * @date 2023-02-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __LAYOUT_CONVERTER_H__
#define __LAYOUT_CONVERTER_H__

#include "Include/LayoutData.hpp"

#include <memory>
#include <list>

using GeometryPtr  = std::shared_ptr<Geometry>;
using LegacyGeometryPtr = Geometry*;
using GeometryList = std::list<std::shared_ptr<Geometry>>;
using LegacyGeometryList = std::list<LegacyGeometryPtr>;


/*! Namespace for geometri convertions functions*/
namespace GeometryConverter
{
    /**
     * @brief Converting path into list of boxes
     * 
     * @param Path Path to convert
     * @return GeometryList 
     */
    GeometryList
    PathToRectList(
        const Geometry* Path);

    /**
     * @brief Splitting sref into list of boxes and polygons
     * 
     * @param Sref 
     * @param Min 
     * @return GeometryList 
     */
    GeometryList
    SplitSref(
        const Geometry* Sref,
        Coord Min = {0,0});
}

#endif //!__LAYOUT_CONVERTER_H__