#include "CNS_derive.H"
#include "CNS_parm.H"

using namespace amrex;

void cns_derpres (const Box& bx, FArrayBox& pfab, int dcomp, int /*ncomp*/,
                  const FArrayBox& rhoefab, const Geometry& /*geomdata*/,
                  Real /*time*/, const int* /*bcrec*/, int /*level*/)
{
    auto const rhoe = rhoefab.array();
    auto       p    = pfab.array();
    AMREX_FOR_3D ( bx, i, j, k,
    {
        p(i,j,k,dcomp) = (Parm::eos_gamma-1.)*rhoe(i,j,k);
    });
}

void cns_dervel (const Box& bx, FArrayBox& velfab, int dcomp, int /*ncomp*/,
                  const FArrayBox& datfab, const Geometry& /*geomdata*/,
                  Real /*time*/, const int* /*bcrec*/, int /*level*/)
{
    auto const dat = datfab.array();
    auto       vel = velfab.array();
    AMREX_FOR_3D ( bx, i, j, k,
    {
        vel(i,j,k,dcomp) = vel(i,j,k,1)/vel(i,j,k,0);
    });
}