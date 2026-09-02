#ifndef slic3r_FillSeededConcentric_hpp_
#define slic3r_FillSeededConcentric_hpp_

#include "FillBase.hpp"

namespace Slic3r {

class FillSeededConcentric : public Fill
{
public:
    ~FillSeededConcentric() override = default;
    bool is_self_crossing() override { return false; }

    void set_seed_point(const Vec2d& seed_point_mm) {
        m_seed_point = Point::new_scale(seed_point_mm.x(), seed_point_mm.y());
    }

protected:
    Fill* clone() const override { return new FillSeededConcentric(*this); }

    void _fill_surface_single(
        const FillParams& params,
        unsigned int thickness_layers,
        const std::pair<float, Point>& direction,
        ExPolygon expolygon,
        Polylines& polylines_out) override;

    bool no_sort() const override { return true; }

private:
    Point m_seed_point{0, 0};
};

} // namespace Slic3r

#endif // slic3r_FillSeededConcentric_hpp_
