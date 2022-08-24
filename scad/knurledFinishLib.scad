/*
 * knurledFinishLib.scad
 *
 * Written by aubenc @ Thingiverse
 *
 * This script is licensed under the Public Domain license.
 *
 * http://www.thingiverse.com/thing:9095
 *
 * Usage:
 *
 *    knurled_cyl( Knurled cylinder height,
 *                 Knurled cylinder outer diameter,
 *                 Knurl polyhedron width,
 *                 Knurl polyhedron height,
 *                 Knurl polyhedron depth,
 *                 Cylinder ends smoothed height,
 *                 Knurled surface smoothing amount );
 */
module knurled_cyl(chg, cod, cwd, csh, cdp, fsh, smt)
{
    cord = (cod + cdp + cdp * smt / 100) / 2;
    cird = cord - cdp;
    cfn = round(2 * cird * PI / cwd);
    clf = 360 / cfn;
    crn = ceil(chg / csh);

    intersection()
    {
        shape(fsh, cird, cord - cdp * smt / 100, cfn * 4, chg);

        translate([ 0, 0, -(crn * csh - chg) / 2 ]) knurled_finish(cord, cird, clf, csh, cfn, crn);
    }
}

module shape(hsh, ird, ord, fn4, hg)
{
    union()
    {
        cylinder(h = hsh, r1 = ird, r2 = ord, $fn = fn4, center = false);

        translate([ 0, 0, hsh - 0.002 ]) cylinder(h = hg - 2 * hsh + 0.004, r = ord, $fn = fn4, center = false);

        translate([ 0, 0, hg - hsh ]) cylinder(h = hsh, r1 = ord, r2 = ird, $fn = fn4, center = false);
    }
}

module knurled_finish(ord, ird, lf, sh, fn, rn)
{
    for (j = [0:rn - 1])
    {
        for (i = [0:fn - 1])
        {
            h0 = sh * j;
            h1 = sh * (j + 1 / 2);
            h2 = sh * (j + 1);

            lf0 = lf * i;
            lf1 = lf * (i + 1 / 2);
            lf2 = lf * (i + 1);
            polyhedron(points =
                           [
                               [ 0, 0, h0 ], [ ord * cos(lf0), ord * sin(lf0), h0 ],
                               [ ird * cos(lf1), ird * sin(lf1), h0 ], [ ord * cos(lf2), ord * sin(lf2), h0 ],

                               [ ird * cos(lf0), ird * sin(lf0), h1 ], [ ord * cos(lf1), ord * sin(lf1), h1 ],
                               [ ird * cos(lf2), ird * sin(lf2), h1 ],

                               [ 0, 0, h2 ], [ ord * cos(lf0), ord * sin(lf0), h2 ],
                               [ ird * cos(lf1), ird * sin(lf1), h2 ], [ ord * cos(lf2), ord * sin(lf2), h2 ]
                           ],
                       triangles =
                           [
                               [ 0, 1, 2 ], [ 2, 3, 0 ], [ 1, 0, 4 ], [ 4, 0, 7 ], [ 7, 8, 4 ], [ 8, 7, 9 ],
                               [ 10, 9, 7 ], [ 10, 7, 6 ], [ 6, 7, 0 ], [ 3, 6, 0 ], [ 2, 1, 4 ], [ 3, 2, 6 ],
                               [ 10, 6, 9 ], [ 8, 9, 4 ], [ 4, 5, 2 ], [ 2, 5, 6 ], [ 6, 5, 9 ], [ 9, 5, 4 ]
                           ],
                       convexity = 5);
        }
    }
}