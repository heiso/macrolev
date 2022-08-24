use <blank.scad>
use <utils.scad>
// 2-4mm of travel is traditional for mechanical key switches
TOTAL_TRAVEL = 4; // [1:0.1:20]

/* [Advanced Stuff] */

BODY_LENGTH = 14.2;
BODY_WIDTH = 14.2;
BODY_HEIGHT = 7;
BODY_CORNER_RADIUS = 0.5;
WALL_THICKNESS = 0.8;
COVER_THICKNESS = 0.5; // [0:0.1:1.5]
COVER_OVERHANG = 0.6;  // [0:0.1:1.5]

CLIP_BRIDGE_THICKNESS = 0.3;
SWITCH_PLATE_THICKNESS = 1.5;
SWITCH_PLATE_TOLERANCE = 0.2;

BODY_TAPER = 1.05;

ROTARY_ENCODER_BODY_HEIGHT = 6.6;
INTERNAL_HEIGHT = 3;

// OpenSCAD's rendering resolution (64 worked well for me but feel free to go higher if you're patient--unlike me)
$fn = 64;

difference()
{
    union()
    {
        switch_body(BODY_LENGTH, BODY_WIDTH, TOTAL_TRAVEL, cover_overhang = COVER_OVERHANG, taper = BODY_TAPER,
                    wall_thickness = WALL_THICKNESS, cover_thickness = COVER_THICKNESS,
                    plate_thickness = SWITCH_PLATE_THICKNESS, plate_tolerance = SWITCH_PLATE_TOLERANCE,
                    corner_radius = BODY_CORNER_RADIUS, bridge_thickness = CLIP_BRIDGE_THICKNESS, height = BODY_HEIGHT);

        translate([ -6.5, -5, 0 ])
        {
            cube([ 13, 10, INTERNAL_HEIGHT ]);
        }
    }

    translate([ 5, -1, INTERNAL_HEIGHT - 1.4 ])
    {
        cube([ 1.3, 2, 1.5 ]);
    }

    cylinder(BODY_HEIGHT, r = 7 / 2);
}