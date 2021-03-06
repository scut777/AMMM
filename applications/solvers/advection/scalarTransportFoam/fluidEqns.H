#include "CourantNo.H"

// Make the flux relative
fvc::makeRelative(phi, U);

if (colinParameter)
{
    fvScalarMatrix AEqn
    (
        fvm::ddt(A)
      - fvc::div(pMesh.phi())
    );
    AEqn.solve();

    // Solve equations iterativelly
    for(int icorr = 0; icorr < nRKstages; icorr++)
    {
        fvScalarMatrix AuniTEqn
        (
            fvm::ddt(A,uniT)
          + fvc::div(phi, uniT, "div(phi,T)")
        );
        AuniTEqn.solve();

        fvScalarMatrix ATEqn
        (
            fvm::ddt(A,T)
          + fvc::div(phi, T, "div(phi,T)")
        );
        ATEqn.solve();
    }
    AT == A*T;
}
else
{
    // Solve equations iterativelly
    for(int icorr = 0; icorr < nRKstages; icorr++)
    {
        fvScalarMatrix TEqn
        (
            fvm::ddt(T)
          + fvc::div(phi, T)
        );
        TEqn.solve();

        fvScalarMatrix uniTEqn
        (
            fvm::ddt(uniT)
          + fvc::div(phi, uniT, "div(phi,T)")
        );
        uniTEqn.solve();
    }
}

// Recover the absolute flux
fvc::makeAbsolute(phi, U);
