$fn = 64;

KNOB_RADIUS = 13;
SKIRT_HEIGHT = 6;
CHAMFER = 0.2;

FACETTES = 180;

SHAFT_HEIGHT = 10.20;
SHAFT_RADIUS = 3;
SHAFT_DISTANCE_TO_TOP = 2;
SHAFT_TOLERANCE = 0.075;

module main(knob_radius = KNOB_RADIUS, skirt_height = SKIRT_HEIGHT, chamfer = CHAMFER, facettes = FACETTES,
            shaft_height = SHAFT_HEIGHT, shaft_radius = SHAFT_RADIUS, shaft_distance_to_top = SHAFT_DISTANCE_TO_TOP,
            shaft_tolerance = SHAFT_TOLERANCE)
{
    /*
     * hole for the shaft
     */
    module shaft_hole()
    {
        translate([ 0, 0, shaft_distance_to_top ])
        {
            difference()
            {
                cylinder(h = shaft_height + skirt_height, r = shaft_radius + SHAFT_TOLERANCE, center = false);

                translate([ -shaft_radius, shaft_radius / 1.7, 0 ])
                    cube([ shaft_radius * 2, 10, shaft_height + skirt_height ]);
            }
        }
    }

    module skirt()
    {
        translate([ 0, 0, shaft_height + shaft_distance_to_top ])
        {
            cylinder(h = skirt_height, r1 = knob_radius - 2, r2 = knob_radius - 2);
        }
    }

    /*
     * main body of the knob
     */
    module mainbody()
    {
        cylinder($fn = facettes, h = shaft_height + shaft_distance_to_top + skirt_height, r = knob_radius,
                 center = false);
    }

    /*
     * helper for the chamfer
     */
    module chamfer()
    {
        rotate_extrude($fn = facettes) translate([ -(knob_radius - 5 - chamfer), 6 + chamfer, 0 ])
        {
            polygon(points = [ [ 0, -10 ], [ -10, -10 ], [ -10, 0 ] ]);
        }
    }

    difference()
    {
        mainbody();

        chamfer();

        skirt();

        shaft_hole();
    }
}

main();