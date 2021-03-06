Info<< "Reading field Uf, velocity on the faces of the moving mesh" << endl;
surfaceVectorField Uf
(
    IOobject
    (
        "Uf", runTime.timeName(), rMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    rMesh
);

Info<< "Reading field h" << endl;
volScalarField h
(
    IOobject
    (
        "h", runTime.timeName(), rMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    rMesh
);
h.correctBoundaryConditions();

// h interpolated on to faces
surfaceScalarField hf
(
    IOobject("hf", runTime.timeName(), rMesh),
    fvc::interpolate(h)
);

// hUf with component in direction H.delta corrected
surfaceVectorField hUf("hUf", hf*Uf);

Info<< "Reading field h0 if present (the orography) on the fixed mesh\n" << endl;
volScalarField h0
(
    IOobject
    (
        "h0", runTime.timeName(), mesh, IOobject::READ_IF_PRESENT
    ),
    mesh,
    dimensionedScalar("h0", dimLength, 0.0)
);
// The gradient of the orography is assumed not to change
//const surfaceScalarField gsnGradh0 = mesh.magSf()*g*fvc::snGrad(h0);

// Volumes associated with faces
surfaceScalarField faceVol("faceVol", 1/3.*(H.delta() & rMesh.Sf()));
faceVol.oldTime();

// fixed flux boundary conditions
wordList VuBCs(rMesh.boundaryMesh().size(), "calculated");
wordList uBCs(rMesh.boundaryMesh().size(), "zeroGradient");
forAll(rMesh.boundaryMesh(), patchi)
{
    const word btype = rMesh.boundaryMesh()[patchi].type();
    const word utype = Uf.boundaryField()[patchi].type();
    const word htype = h.boundaryField()[patchi].type();
    if (btype == "wall" || btype == "symmetryPlane" || btype == "empty"
     || htype == "geostrophicZeroFlux")
    {
        VuBCs[patchi] = "fixedValue";
        uBCs[patchi] = "slip";
    }
    else if (utype == "fixedValue")
    {
        VuBCs[patchi] = "fixedValue";
        uBCs[patchi] = "fixedValue";
    }
}

Info << "Calculating the cell centre velocity, u" << endl;
volVectorField u
(
    IOobject("u", runTime.timeName(), rMesh,
             IOobject::READ_IF_PRESENT,IOobject::AUTO_WRITE),
    H.reconstruct(Uf & rMesh.Sf()),
    uBCs
);
u.write();

Info << "Calculating the mesh flux, meshPhi" << endl;
surfaceScalarField meshPhi
(
    IOobject
    (
        "meshPhi", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    fvc::meshPhi(u)
);

Info<< "Initialising the absolute face flux, U" << endl;
surfaceScalarField U
(
    IOobject
    (
        "U", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    hf * (Uf & rMesh.Sf())
);
U.write();

Info << "Calculating V mass circulation field" << endl;
surfaceScalarField V
(
    IOobject
    (
        "V", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    hf * (Uf & H.delta()),
    VuBCs
);

//Info<< "Creating field hTotal for post processing\n" << endl;
//volScalarField hTotal
//(
//   IOobject
//   (
//       "hTotal", runTime.timeName(), mesh,
//       IOobject::NO_READ_IF_PRESENT, IOobject::AUTO_WRITE
//   ),
//   h+h0
//);
//hTotal.write();


Info << "Initialising dhudt and dVdt for trapezoidal implicit time-stepping" << endl;

surfaceScalarField dVdt
(
    IOobject("dVdt", runTime.timeName(), rMesh),
    -(H.delta() & fvc::interpolate(fvc::div(U,u), "Uf"))
    -hf*(two_dxOmega & Uf)
    -g*hf*fvc::snGrad(h)*H.magd(),
    VuBCs
);

Info << "Storing the old value of V, V0 on the new mesh" << endl;
surfaceScalarField V0("V0", V);

// Flux divergence at old time-step for time-stepping
volScalarField divUold("divUold", fvc::div(U));

// fix no flow boundaries
forAll(V.boundaryField(), patchi)
{
    const word btype = rMesh.boundaryMesh()[patchi].type();
    const word utype = Uf.boundaryField()[patchi].type();
    const word htype = h.boundaryField()[patchi].type();
    if
    (
        btype == "wall" || btype == "symmetryPlane" || btype == "empty"
     || utype == "slip" || htype == "geostrophicZeroFlux"
    )
    {
        V.boundaryFieldRef()[patchi] == 0;
        V0.boundaryFieldRef()[patchi] == 0;
        dVdt.boundaryFieldRef()[patchi] == 0;
    }
    else if(utype == "fixedValue")
    {
        u.boundaryFieldRef()[patchi] == Uf.boundaryField()[patchi];
    }
}

surfaceVectorField dhudt
(
    IOobject("dhudt", runTime.timeName(), rMesh),
    fvc::interpolate(H.reconstructd(dVdt))
);
dhudt += (dVdt - (dhudt & H.delta()))*H.delta()/sqr(H.magd());
dhudt.oldTime();

