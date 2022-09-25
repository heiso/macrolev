include <openscad-bits/lib.scad>
use <openscad-bits/lib.scad>

IS_PRESSED = false;

/* [Hidden] */
thickness_floor = 0.5;
thickness_wall = 1;

stem_x = 7;
stem_y = 5;
stem_travel = 4;
stem_tolerance = 0.5;

sensor_x = 4;
sensor_y = 3;
sensor_z = 1.5;
sensor_tolerance = 0.3;

magnet_height = 1.7;
magnet_radius = 4 / 2;
magnet_tolerance = 0.2;

nail_radius = 1.6 / 2;
nail_tolerance = .2;

body_x = stem_x + thickness_wall * 2;
body_y = stem_y + thickness_wall * 2;
body_height = stem_travel * 2 + sensor_z + thickness_floor + magnet_height + thickness_floor;
$fn = 32;

module sensor(cutout = false)
{
    if (cutout)
    {
        translate([ 0, -100 / 2, sensor_z / 2 ])
            cube([ sensor_x + sensor_tolerance, sensor_y + 100, sensor_z + sensor_tolerance ], true);
    }
    else
    {
        color("black") translate([ 0, 0, sensor_z / 2 ]) cube([ sensor_x, sensor_y, sensor_z ], true);
    }
}

module nail(cutout = false)
{
    translate([ 0, 0, nail_radius ])
    {
        if (cutout)
        {
            hull()
            {
                rotate([ 0, 90, 0 ]) cylinder(h = body_x, r = nail_radius + nail_tolerance, center = true);
                translate([ 0, 0, stem_travel ]) rotate([ 0, 90, 0 ])
                    cylinder(h = body_x, r = nail_radius + nail_tolerance, center = true);
            }
        }
        else
        {
            color("silver") rotate([ 0, 90, 0 ]) cylinder(h = body_x, r = nail_radius, center = true);
        }
    }
}

///

module stem_magnet_origin()
{
    stem_origin() translate([ 0, 0, 0 ]) children();
}

module sensor_origin()
{
    translate([ 0, 0, -thickness_floor ]) translate([ 0, 0, -sensor_z ]) stem_origin_offset_pressed() stem_origin()
        children();
}

module body_magnet_origin()
{
    translate([ 0, 0, -thickness_floor ]) translate([ 0, 0, -magnet_height ]) sensor_origin() children();
}

module stem_origin()
{
    translate([ 0, 0, body_height - stem_travel ]) children();
}

module stem_origin_offset_pressed()
{
    translate([ 0, 0, -stem_travel ]) children();
}

module nail_origin()
{
    translate([ 0, 0, body_height - stem_travel + magnet_height ]) children();
}

module stem_movement()
{
    if (IS_PRESSED)
    {
        stem_origin_offset_pressed() children();
    }
    else
    {
        children();
    }
}

///
module stem_cross()
{
    cube([ 4, 1, stem_travel ], true);
    cube([ 1, 4, stem_travel ], true);
}

module stem()
{
    color("green")
    {
        difference()
        {
            union()
            {
                translate([ 0, 0, stem_travel ]) rounded_cube([ stem_x, stem_y, stem_travel * 2 ], 1);
                translate([ 0, 0, stem_travel / 2 + stem_travel * 2 ]) stem_cross();
            }

            magnet(h = magnet_height, r = magnet_radius, cutout = true);

            translate([ 0, 0, magnet_height ]) nail(cutout = true);
        }
    }
}

module stem_cutout()
{
    cube([ stem_x + stem_tolerance, stem_y + stem_tolerance, stem_travel * 2 ], true);
}

module body()
{
    difference()
    {
        translate([ 0, 0, body_height / 2 ]) cube([ body_x, body_y, body_height ], true);

        stem_origin() stem_cutout();

        sensor_origin() sensor(cutout = true);

        body_magnet_origin()
        {
            translate([ 0, 0, -magnet_tolerance ])
                magnet(h = magnet_height, r = magnet_radius, r_tolerance = magnet_tolerance, cutout = true);
            translate([ 0, -100 / 2, -10 / 2 + magnet_height ])
                cube([ magnet_radius * 2 + magnet_tolerance * 2, magnet_radius * 2 + .2 + 100, 10 ], true);
        }

        nail_origin() nail();
    }
}

///

// ~55g
// good feeling, almost like void_switch
// but not small enough :/ might find a way to remove one stem_travel from height

if ($preview)
{
    stem_movement()
    {
        stem_origin() stem();
        stem_magnet_origin() magnet(h = magnet_height, r = magnet_radius);
    }

    // #% cylinder(h = 20, r = 5.5 / 2, $fn = 360);

    body_magnet_origin() magnet(h = magnet_height, r = magnet_radius);
    sensor_origin() sensor();
    nail_origin() nail();

    % body();
}
else
{

    translate([ body_x + 5, 0, stem_y / 2 ]) rotate([ -90, 0, 0 ]) stem();
    translate([ 0, 0, body_y / 2 ]) rotate([ -90, 0, 0 ]) body();
}