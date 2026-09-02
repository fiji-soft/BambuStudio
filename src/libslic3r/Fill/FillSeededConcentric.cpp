#include "../ClipperUtils.hpp"
#include "../ExPolygon.hpp"
#include "../Surface.hpp"
#include "FillSeededConcentric.hpp"

#include <limits>
#include <utility>

namespace Slic3r {

namespace {

void append_clipped_loop(
    const Polygon& loop,
    const ExPolygon& expolygon,
    Point& last_pos,
    Polylines& polylines_out)
{
    if (!loop.is_valid())
        return;

    Polyline path = loop.split_at_index(last_pos.nearest_point_index(loop.points));
    Polylines clipped = intersection_pl(path, expolygon);
    if (clipped.empty() && expolygon.contains(path))
        clipped.emplace_back(std::move(path));

    for (Polyline& clipped_path : clipped) {
        if (!clipped_path.is_valid())
            continue;
        last_pos = clipped_path.last_point();
        polylines_out.emplace_back(std::move(clipped_path));
    }
}

} // namespace

void FillSeededConcentric::_fill_surface_single(
    const FillParams& params,
    unsigned int,
    const std::pair<float, Point>&,
    ExPolygon expolygon,
    Polylines& polylines_out)
{
    if (params.density <= 0.0001f || expolygon.holes.empty())
        return;

    const coord_t min_spacing = scale_(this->spacing);
    if (min_spacing <= 0)
        return;

    coord_t distance = coord_t(min_spacing / params.density);
    if (params.density > 0.9999f && !params.dont_adjust) {
        distance = this->_adjust_solid_spacing(expolygon.contour.bounding_box().size()(0), distance);
        this->spacing = unscale<double>(distance);
    }
    if (distance <= 0)
        return;

    const Point seed_point = this->m_seed_point;
    size_t seed_hole_idx = size_t(-1);
    double seed_distance2 = std::numeric_limits<double>::max();
    for (size_t i = 0; i < expolygon.holes.size(); ++i) {
        const Point projected = seed_point.projection_onto(expolygon.holes[i]);
        const double distance2 = (projected - seed_point).cast<double>().squaredNorm();
        if (distance2 < seed_distance2) {
            seed_distance2 = distance2;
            seed_hole_idx = i;
        }
    }
    if (seed_hole_idx == size_t(-1))
        return;

    const size_t first_output = polylines_out.size();
    Point last_pos = seed_point;
    append_clipped_loop(expolygon.holes[seed_hole_idx], expolygon, last_pos, polylines_out);

    Polygons current{ expolygon.holes[seed_hole_idx] };
    while (!current.empty()) {
        // A hole is clockwise in an ExPolygon. In the libslic3r offset helper,
        // a negative offset grows that contour into the printable material
        // without offsetting the outer contour.
        Polygons next = offset(current, -float(distance));
        if (next.empty())
            break;

        const size_t output_size = polylines_out.size();
        for (const Polygon& loop : next)
            append_clipped_loop(loop, expolygon, last_pos, polylines_out);
        if (polylines_out.size() == output_size)
            break;

        // Keep the un-clipped frontier for the next offset. Only the emitted
        // path is clipped, so a frontier touching the outer contour can still
        // continue until no part remains inside the printable surface.
        current = std::move(next);
    }

    // Match normal Concentric's small end clip used to avoid starting exactly
    // at a closed-loop seam.
    size_t j = first_output;
    for (size_t i = first_output; i < polylines_out.size(); ++i) {
        polylines_out[i].clip_end(this->loop_clipping);
        if (polylines_out[i].is_valid()) {
            if (j < i)
                polylines_out[j] = std::move(polylines_out[i]);
            ++j;
        }
    }
    if (j < polylines_out.size())
        polylines_out.erase(polylines_out.begin() + j, polylines_out.end());
}

} // namespace Slic3r
