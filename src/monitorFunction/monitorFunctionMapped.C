/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "monitorFunctionMapped.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "meshToMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionMapped, 0);
addToRunTimeSelectionTable(monitorFunction, monitorFunctionMapped, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

monitorFunctionMapped::monitorFunctionMapped
(
    const IOdictionary& dict
)
:
    monitorFunction(dict),
    name_(dict.lookup("monitorFieldName"))
{}

// * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * * //

tmp<volScalarField> monitorFunctionMapped::map
(
    const fvMesh& newMesh,
    const volScalarField& oldMonitor
) const
{
    meshToMesh meshMap
    (
        oldMonitor.mesh(), newMesh, meshToMesh::imCellVolumeWeight, true
    );
    
    tmp<volScalarField> tMon
    (
        new volScalarField
        (
            IOobject(name_, newMesh.time().timeName(), newMesh,
                     IOobject::NO_READ, IOobject::AUTO_WRITE),
            meshMap.mapSrcToTgt(oldMonitor)
        )
    );
    
    return tMon;
}


tmp<surfaceVectorField> monitorFunctionMapped::grad
(
    const fvMesh& newMesh,
    const surfaceVectorField& oldMonitor
) const
{
    // meshToMesh meshMap
    // (
    //     oldMonitor.mesh(), newMesh, meshToMesh::imCellVolumeWeight, false
    // );
    
    // tmp<volVectorField> tMon
    // (
    //     new volVectorField
    //     (
    //         IOobject("monitor", newMesh.time().timeName(), newMesh,
    //                  IOobject::NO_READ, IOobject::AUTO_WRITE),
    //         meshMap.mapSrcToTgt(oldMonitor)
    //     )
    // );
    
    // return tMon;
    NotImplemented;
    return surfaceVectorField::null();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //



} // End namespace Foam

// ************************************************************************* //
