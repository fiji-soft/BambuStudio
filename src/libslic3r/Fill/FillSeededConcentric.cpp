#include "FillSeededConcentric.hpp"

namespace Slic3r {

void FillSeededConcentric::_fill_surface_single(
    const FillParams&,
    unsigned int,
    const std::pair<float, Point>&,
    ExPolygon,
    Polylines&)
{
    // Experimental pattern registration. The toolpath generator is added separately.
}

} // namespace Slic3r
