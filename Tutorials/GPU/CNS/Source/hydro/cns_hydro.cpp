
#include "CNS_K.H"
#include "CNS_index_macros.H"

using namespace amrex;

AMREX_GPU_DEVICE
void cns_ctoprim (Box const& bx, FArrayBox const& ufab, FArrayBox & qfab)
{
    const auto len = bx.length3d();
    const auto lo  = bx.loVect3d();
    const auto u = ufab.view(lo);
    const auto q = qfab.view(lo);
    const Real smallr = 1.e-19;
    const Real smallp = 1.e-10;
    const Real gamma = 1.4;

    for         (int k = 0; k < len[2]; ++k) {
        for     (int j = 0; j < len[1]; ++j) {
            AMREX_PRAGMA_SIMD
            for (int i = 0; i < len[0]; ++i) {
                Real rho = u(i,j,k,URHO);
                rho = (rho > smallr) ? rho : smallr;
                Real rhoinv = 1.0/rho;
                Real ux = u(i,j,k,UMX)*rhoinv;
                Real uy = u(i,j,k,UMY)*rhoinv;
                Real uz = u(i,j,k,UMZ)*rhoinv;
                Real kineng = 0.5*rho*(ux*ux+uy*uy+uz*uz);
                Real ei = u(i,j,k,UEDEN) - kineng;
                if (ei <= 0.0) ei = u(i,j,k,UEINT);
                Real p = (gamma-1.0)*ei;
                p = (p > smallp) ? p : smallp;
                ei *= rhoinv;

                q(i,j,k,QRHO) = rho;
                q(i,j,k,QU) = ux;
                q(i,j,k,QV) = uy;
                q(i,j,k,QW) = uz;
                q(i,j,k,QEINT) = ei;
                q(i,j,k,QPRES) = p;
                q(i,j,k,QCS) = std::sqrt(gamma*p*rhoinv);
                q(i,j,k,QTEMP) = 0.0;
            }
        }
    }
}

#if (AMREX_SPACEDIM == 3)
AMREX_GPU_DEVICE
void cns_flux_to_dudt (Box const& bx, FArrayBox& dudtfab,
                       FArrayBox const& fxfab, FArrayBox const& fyfab, FArrayBox const& fzfab,
                       GpuArray<Real,AMREX_SPACEDIM> const& dxinv)
{
    const auto len = bx.length3d();
    const auto lo  = bx.loVect3d();
    const int ncomp = dudtfab.nComp();
    const auto dudt = dudtfab.view(lo);
    const auto fx   =   fxfab.view(lo);
    const auto fy   =   fyfab.view(lo);
    const auto fz   =   fzfab.view(lo);

    for (int n = 0; n < NCONS; ++n) {
        for         (int k = 0; k < len[2]; ++k) {
            for     (int j = 0; j < len[1]; ++j) {
                AMREX_PRAGMA_SIMD
                for (int i = 0; i < len[0]; ++i) {
                    dudt(i,j,k,n) = dxinv[0] * (fx(i,j,k,n) - fx(i+1,j,k,n))
                        +           dxinv[1] * (fy(i,j,k,n) - fy(i,j+1,k,n))
                        +           dxinv[2] * (fz(i,j,k,n) - fz(i,j,k+1,n));
                }
            }
        }
    }
}
#endif
