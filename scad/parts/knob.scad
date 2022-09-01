$fn = 64;

KNOB_RADIUS = 13;
SKIRT_HEIGHT = 6;
CHAMFER = 0.2;

FACETTES = 180;

SHAFT_HEIGHT = 10.20;
SHAFT_RADIUS = 3;
SHAFT_DISTANCE_TO_TOP = 2;
SHAFT_TOLERANCE = 0.075;

/*
 * hole for the shaft
 */
module shaft_hole()
{
    translate([ 0, 0, SHAFT_DISTANCE_TO_TOP ])
    {
        difference()
        {
            cylinder(h = SHAFT_HEIGHT + SKIRT_HEIGHT, r = SHAFT_RADIUS + SHAFT_TOLERANCE, center = false);

            translate([ -SHAFT_RADIUS, SHAFT_RADIUS / 1.7, 0 ])
                cube([ SHAFT_RADIUS * 2, 10, SHAFT_HEIGHT + SKIRT_HEIGHT ]);
        }
    }
}

module skirt()
{
    translate([ 0, 0, SHAFT_HEIGHT + SHAFT_DISTANCE_TO_TOP ])
    {
        cylinder(h = SKIRT_HEIGHT, r1 = KNOB_RADIUS - 2, r2 = KNOB_RADIUS - 2);
    }
}

/*
 * main body of the knob
 */
module mainbody()
{
    cylinder($fn = FACETTES, h = SHAFT_HEIGHT + SHAFT_DISTANCE_TO_TOP + SKIRT_HEIGHT, r = KNOB_RADIUS, center = false);
}

/*
 * helper for the chamfer
 */
module chamfer()
{
    rotate_extrude($fn = FACETTES) translate([ -(KNOB_RADIUS - 5 - CHAMFER), 6 + CHAMFER, 0 ])
    {
        polygon(points = [ [ 0, -10 ], [ -10, -10 ], [ -10, 0 ] ]);
    }
}

module main()
{
    difference()
    {
        mainbody();

        chamfer();

        skirt();

        shaft_hole();
    }
}

main();