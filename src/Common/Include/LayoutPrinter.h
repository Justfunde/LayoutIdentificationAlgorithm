#ifndef __LAYOUT_PRINTER_H__
#define __LAYOUT_PRINTER_H__

#include "Include/LayoutData.hpp"

namespace LayoutPrinter
{
    void
    PrintGeometry(
        const Geometry* Geom,
        size_t Depth = 0);
}


#endif //!__LAYOUT_PRINTER_H__