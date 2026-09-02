#ifndef slic3r_FillSeededConcentric_hpp_
#define slic3r_FillSeededConcentric_hpp_

#include "FillBase.hpp"

namespace Slic3r {

class FillSeededConcentric : public Fill
{
public:
    ~FillSeededConcentric() override = default;
    bool is_self_crossing() override { return false; }

protected:
    Fill* clone() const override { return new FillSeededConcentric(*this); }

    void _fill_surface_single(
        const FillParams& params,
        unsigned int thickness_layers,
        const std::pair<float, Point>& direction,
        ExPolygon expolygon,
        Polylines& polylines_out) override;

    bool no_sort() const override { return true; }
};

} // namespace Slic3r

#endif // slic3r_FillSeededConcentric_hpp_
