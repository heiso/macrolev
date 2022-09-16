//! Project description in Markdown format before the first include.
$pp1_colour = "grey"; // Override any global defaults here if required, see NopSCADlib/global_defs.scad.
include <NopSCADlib/lib.scad>
use <NopSCADlib/vitamins/potentiometer.scad>

thin = 0.00001;
border_radius = 2;
top_thickness = 1.5;
wall_thickness = 1.5;
bottom_plate_thickness = 1.5;

// See https://matt3o.com/anatomy-of-a-keyboard/
switch_tolerance = 0.1;
switch_hole_size = 14 + switch_tolerance;
switch_distance_between_holes = 5.05;
switch_count = 3;

magnet_height = 1.7;
magnet_radius = 4 / 2;
magnet_tolerance = 0.2;

kb2040_width = 18;
kb2040_length = 21;
kb2040_thickness = 1.6;
kb2040_bottom_clearance = 1;
kb2040_offset = 2;

drv2605l_width = 18;
drv2605l_length = 26;
drv2605l_thickness = 1.6;
drv2605l_bottom_clearance = 1;

ERM_motor_radius = 10 / 2;
ERM_motor_thickness = 3.5;

internal_height = 15;
external_height = internal_height + top_thickness + bottom_plate_thickness;

internal_width =
    switch_distance_between_holes + (switch_distance_between_holes + switch_hole_size) * 2; // dep to internal vitamins
external_width = internal_width + wall_thickness * 2;

knob_chamfer = 1;
knob_oversized_by = (1 / 5) * external_width;
knob_radius = (external_width / 2) + knob_oversized_by / 2;
knob_height = 17;
knob_elevation = 0;
knob_angle = 15;
knob_top_thickness = top_thickness + 2;
knob_rotation = [ 0, -knob_angle, 0 ];
knob_base_length = (external_height / cos(knob_angle)) + tan(knob_angle) * knob_radius;
knob_base_cutout_length = internal_height / cos(knob_angle);
knob_base_length_on_body = sin(knob_angle) * knob_base_length + cos(knob_angle) * knob_radius;

internal_length = (switch_distance_between_holes + switch_hole_size + switch_distance_between_holes) + kb2040_width +
                  kb2040_offset + knob_base_length_on_body; // dep to switches, internal vitamins and rotary
external_length = internal_length + wall_thickness * 2;

rotary_shaft_height = 10.20;
rotary_shaft_radius = 3;
rotary_shaft_base_radius = 7 / 2;
rotary_shaft_base_height = 7 / 2;
rotary_shaft_tolerance = 0.075;
rotary_base_tolerance = 0.3;
rotary_base_width = 12;

module rounded_cube(v)
{
    x_corrected = v[0] - border_radius * 2;
    y_corrected = v[1] - border_radius * 2;
    linear_extrude(height = v[2]) offset(r = border_radius) square([ x_corrected, y_corrected ], center = true);
}

module kb2040_transformation()
{
    translate([
        knob_base_length_on_body - kb2040_width + kb2040_offset, internal_width / 2, bottom_plate_thickness +
        kb2040_bottom_clearance
    ]) children();
}

module kb2040_vitamin(cutout = false)
{
    translate([ kb2040_width / 2, -kb2040_length / 2, 0 ])
    {
        color("black") translate([ 0, 0, kb2040_thickness / 2 ])
            cube([ kb2040_width, kb2040_length, kb2040_thickness ], center = true);
        translate([ 0, (kb2040_length / 2) - (7.35 / 2) + 1, kb2040_thickness ]) rotate([ 0, 0, 90 ]) usb_C(cutout);
    }
}

module drv2605l_transformation()
{
    translate([
        knob_base_length_on_body - drv2605l_width - 2, internal_width / 2,
        bottom_plate_thickness + drv2605l_bottom_clearance + 5.5
    ]) children();
}

module drv2605l_vitamin()
{
    translate([ drv2605l_width / 2, -drv2605l_length / 2, drv2605l_thickness / 2 ])
    {
        color("black") cube([ drv2605l_width, drv2605l_length, drv2605l_thickness ], center = true);
    }
}

module ERM_motor_transformation()
{
    translate([
        knob_base_length_on_body - ERM_motor_radius * 2 - 5, 0,
        bottom_plate_thickness + internal_height - ERM_motor_thickness -
        top_thickness
    ]) children();
}

module ERM_motor_vitamin()
{
    translate([ ERM_motor_radius, 0, 0 ])
    {
        color("silver") cylinder(h = ERM_motor_thickness, r = ERM_motor_radius);
    }
}

module ERM_motor_cutout()
{
    height = ERM_motor_thickness + switch_tolerance;
    radius = ERM_motor_radius + switch_tolerance / 2;
    hole = radius + 5;
    translate([ radius - switch_tolerance, 0, 0 ]) cylinder(h = height, r = radius);
    translate([ hole / 2 + radius, 0, height / 2 ]) cube([ hole, radius * 2, height ], center = true);
}

module rotary_encoder_vitamin()
{
    translate([ 0, 0, -knob_top_thickness ]) rotate([ 0, 180, 0 ])
    {
        translate([ 0, 0, -knob_top_thickness ]) pot_nut(KY_040_encoder, false);
        potentiometer(KY_040_encoder);
    }
}

module rotary_encoder_cutout()
{
    translate([ 0, 0, -knob_top_thickness ])
    {
        cylinder(h = rotary_shaft_base_height + 1, r = rotary_shaft_base_radius);
        translate([ 0, 0, 1 / 2 ])
            cube([ rotary_base_width + rotary_base_tolerance, 2 + rotary_base_tolerance, 1 ], true);
        translate([ 0, 0, -100 / 2 ])
            cube([ rotary_base_width + rotary_base_tolerance, rotary_base_width + rotary_base_tolerance, 100 ], true);
    }
}

module magnet_vitamin()
{
    color("silver") cylinder(h = magnet_height, r = magnet_radius);
}

module magnet_cutout()
{
    cylinder(h = magnet_height + magnet_tolerance * 2, r = magnet_radius + magnet_tolerance / 2);
}

module hall_effect_sensor()
{
    cube([ 4, 4, 1.5 ]);
}

module knob_stl()
{
    stl("knob");
    difference()
    {
        union()
        {
            translate([ 0, 0, knob_height - knob_chamfer ]) hull()
            {
                linear_extrude(height = thin) circle(r = knob_radius);
                translate([ 0, 0, knob_chamfer ]) linear_extrude(height = thin) offset(delta = -knob_chamfer)
                    circle(r = knob_radius);
            }

            cylinder(h = knob_height - knob_chamfer, r = knob_radius);
        }

        translate([ 0, 0, (knob_height - knob_chamfer) - rotary_shaft_height - .1 ])
        {
            difference()
            {
                cylinder(h = rotary_shaft_height + .1, r = rotary_shaft_radius + rotary_shaft_tolerance);
                translate([ -rotary_shaft_radius, rotary_shaft_radius / 1.7, 0 ])
                    cube([ rotary_shaft_radius * 2, 10, rotary_shaft_height + .1 ]);
            }
        }

        cylinder(h = knob_height - knob_chamfer - rotary_shaft_height, r = knob_radius - 2);
    }
}

module knob_transformation()
{
    z_translation = sin(knob_angle) * knob_radius;
    x_translation = sin(knob_angle) * knob_base_length;
    translate([ x_translation, 0, 0 ]) translate([ 0, 0, -z_translation ]) rotate(knob_rotation) children();
}

module magnets_transformations()
{
    translate([
        external_length - magnet_radius - wall_thickness - wall_thickness / 2,
        internal_width / 2 - magnet_radius - wall_thickness / 2, 0
    ]) children();

    translate([
        external_length - magnet_radius - wall_thickness - wall_thickness / 2 - switch_hole_size -
            switch_distance_between_holes,
        internal_width / 2 - magnet_radius - wall_thickness / 2, 0
    ]) children();

    translate([ knob_base_length_on_body, internal_width / 2 - magnet_radius - wall_thickness / 2, 0 ]) children();
}

module kb2040_supports()
{
    radius = 0.5;
    width = 5;
    translate([ -1, -kb2040_length, 0 ])
    {
        translate([ 0, 0, -kb2040_bottom_clearance ]) linear_extrude(kb2040_bottom_clearance) difference()
        {
            square([ kb2040_width + 2, kb2040_length ]);
            translate([ 2, 0, 0 ]) square([ kb2040_width - 2, kb2040_length ]);
            translate([ 0, kb2040_length / 4, 0 ]) square([ kb2040_width + 2, kb2040_length / 2 ]);
        }

        linear_extrude(kb2040_thickness) difference()
        {
            square([ kb2040_width + 2, kb2040_length ]);
            translate([ 1, 0, 0 ]) square([ kb2040_width, kb2040_length ]);
            translate([ 0, kb2040_length / 4, 0 ]) square([ kb2040_width + 2, kb2040_length / 2 ]);
        }

        translate([ 0, -2, -kb2040_bottom_clearance ])
            linear_extrude(kb2040_bottom_clearance + kb2040_thickness + radius + .3)
        {
            square([ width, 2 ]);
            translate([ kb2040_width - width + 2, 0, 0 ]) square([ width, 2 ]);
        }

        translate([ 0, -0.3, kb2040_bottom_clearance + kb2040_thickness - radius + 0.2 ]) rotate([ 0, 90, 0 ])
            cylinder(r = radius, h = width);
        translate([ kb2040_width - width + 2, -0.3, kb2040_bottom_clearance + kb2040_thickness - radius + 0.2 ])
            rotate([ 0, 90, 0 ]) cylinder(r = radius, h = width);
    }

    clip(ymax = 0) translate([ -1, 0, kb2040_thickness + radius ]) rotate([ 0, 90, 0 ]) cylinder(r = radius, h = 3);
    clip(ymax = 0) translate([ kb2040_width - 2, 0, kb2040_thickness + radius ]) rotate([ 0, 90, 0 ])
        cylinder(r = radius, h = 3);
}

module body_magnet_holder()
{
    width = (magnet_radius + wall_thickness / 2) * 2;
    height = width + magnet_height;
    difference()
    {
        translate([ -width / 2, width / 2, -height ]) mirror([ 1, 0, 0 ]) rotate([ 90, 0, -90 ]) linear_extrude(width)
            polygon([ [ 0, 0 ], [ width, width ], [ width, height ], [ 0, height ] ]);

        mirror([ 0, 0, 1 ]) magnet_cutout();
    }
}

module body_base_stl()
{
    stl("body_base");

    difference()
    {
        union()
        {
            // knob base
            clip(zmin = 0) knob_transformation() cylinder(h = knob_elevation + knob_base_length, r = knob_radius);

            // base plate
            difference()
            {
                translate([ external_length / 2, 0, 0 ])
                    rounded_cube([ external_length, external_width, bottom_plate_thickness ]);

                knob_transformation() linear_extrude(knob_elevation + knob_base_length) hull()
                {
                    translate([ -knob_radius, 0, 0 ]) square([ knob_radius, knob_radius * 2 ], true);
                    circle(knob_radius);
                }
            }

            // walls
            difference()
            {
                translate([ external_length / 2, 0, bottom_plate_thickness ])
                    rounded_cube([ external_length, external_width, internal_height ]);

                translate([ external_length / 2, 0, bottom_plate_thickness ])
                    rounded_cube([ internal_length, internal_width, internal_height ]);
            }
        }

        // rotary cutout
        knob_transformation() translate([ 0, 0, knob_base_length ]) rotary_encoder_cutout();

        // rotary access hole
        difference()
        {
            height = internal_height - (ERM_motor_thickness + top_thickness * 2);
            length = knob_base_length_on_body;
            translate([ length / 2, 0, height / 2 + bottom_plate_thickness ])
                cube([ length, internal_width, height ], center = true);
            knob_transformation() translate([ 0, 0, 50 + knob_base_length - knob_top_thickness ])
                cube(100, center = true);
        }

        // erm motor cutout
        ERM_motor_transformation() ERM_motor_cutout();

        // usb-c hole
        kb2040_transformation() kb2040_vitamin(true);
    }

    // kb2040 supports
    kb2040_transformation() kb2040_supports();

    // magnet holders
    translate(
        [ 0, 0, bottom_plate_thickness + internal_height - (magnet_height + magnet_tolerance * 2) - magnet_tolerance ])
    {
        magnets_transformations() body_magnet_holder();
        mirror([ 0, 1, 0 ]) magnets_transformations() body_magnet_holder();
    }
}

module top_plate_magnet_holder()
{
    difference()
    {
        cylinder(r = magnet_radius + wall_thickness / 2, h = magnet_height + magnet_tolerance * 2);
        magnet_cutout();
    }
}

module top_plate_stl()
{
    stl("top_plate");

    difference()
    {
        union()
        {
            // plate
            translate([ external_length / 2, 0, bottom_plate_thickness + internal_height ])
                rounded_cube([ external_length, external_width, top_thickness ]);

            // magnet holders
            translate([ 0, 0, bottom_plate_thickness + internal_height ]) mirror([ 0, 0, 1 ])
            {
                magnets_transformations() top_plate_magnet_holder();
                mirror([ 0, 1, 0 ]) magnets_transformations() top_plate_magnet_holder();
            }

            // pilar
            translate([
                external_length - switch_hole_size - wall_thickness - switch_distance_between_holes -
                    switch_distance_between_holes / 2,
                0,
                bottom_plate_thickness
            ]) cylinder(r = magnet_radius, h = internal_height);
        }

        // knob base cutout
        knob_transformation() cylinder(h = knob_elevation + knob_base_length, r = knob_radius);

        // switches holes
        translate([
            external_length - switch_hole_size / 2 - wall_thickness - switch_distance_between_holes, 0,
            internal_height
        ])
        {
            translate([ 0, +switch_hole_size / 2 + switch_distance_between_holes / 2, 0 ]) cube(switch_hole_size, true);
            translate([ 0, -switch_hole_size / 2 - switch_distance_between_holes / 2, 0 ]) cube(switch_hole_size, true);
            translate([
                -switch_hole_size - switch_distance_between_holes,
                +switch_hole_size / 2 + switch_distance_between_holes / 2, 0
            ]) cube(switch_hole_size, true);
            translate([
                -switch_hole_size - switch_distance_between_holes,
                -switch_hole_size / 2 - switch_distance_between_holes / 2, 0
            ]) cube(switch_hole_size, true);
        }
    }
}

module test_stl()
{
    stl("test");

    intersection()
    {
        union()
        {

            top_plate_stl();
            // body_base_stl();
        }

        translate([ 54.65, -external_width / 2, 0 ]) cube([ 5.52, external_width, external_height * 2 ]);
    }
    // translate([ 54.65+5.52, 0, 0 ]) translate([0,-external_width/2,0])
    // cube([wall_thickness,external_width,internal_height+bottom_plate_thickness]);
}

//! Assembly instructions in Markdown format in front of each module that makes an assembly.
module main_assembly() assembly("main")
{
    kb2040_transformation() kb2040_vitamin();
    drv2605l_transformation() drv2605l_vitamin();
    ERM_motor_transformation() ERM_motor_vitamin();
    knob_transformation() translate([ 0, 0, knob_base_length ]) rotary_encoder_vitamin();

    render() // clip(ymin = 0)
    {
        knob_transformation() translate([ 0, 0, 0.5 + knob_base_length ]) knob_stl();
        body_base_stl();
    }

    color("grey", 0.8) render() // clip(ymin = 0)
    {
        top_plate_stl();
    }

    color("red", 0.5)
    {

        // top_plate_stl();
        // magnets_transformations() magnet_vitamin();
        // test_stl();
    }
}

if ($preview)
    main_assembly();