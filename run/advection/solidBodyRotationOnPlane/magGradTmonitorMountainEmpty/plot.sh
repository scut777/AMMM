#!/bin/bash -e

time=0.5
field=T
gmtFoam ${field}under -region pMesh  -time $time
cat $time/${field}under.ps ../fixedMountain/0/mountainOver.ps \
    > $time/${field}.ps
ps2pdf $time/${field}.ps $time/${field}.pdf.pdf
pdfcrop $time/${field}.pdf.pdf $time/${field}.pdf
rm $time/${field}under.ps $time/${field}.ps $time/${field}.pdf.pdf
gv $time/${field}.pdf &

# Plot results
for field in T A AT UT mesh monitor uniT; do
    gmtFoam ${field}under -region pMesh
    for time in [0-9]*; do
        cat $time/${field}under.ps ../fixedMountain/0/mountainOver.ps \
            > $time/${field}.ps
        ps2pdf $time/${field}.ps $time/${field}.pdf.pdf
        pdfcrop $time/${field}.pdf.pdf $time/${field}.pdf
        rm $time/${field}under.ps $time/${field}.ps $time/${field}.pdf.pdf
        gv $time/${field}.pdf &
    done
done

# Conservation of T
globalSum T -region pMesh

# Make links for animategraphics
mkdir -p animategraphics
for field in T A AT UT mesh monitor uniT; do
    for time in [0-9]*; do
        t=`echo $time | awk {'print $1/0.5'}`
        ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
