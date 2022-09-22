include <openscad-bits/lib.scad>
use <openscad-bits/lib.scad>

border_radius = 2;
top_thickness = 1.5;
wall_thickness = 1.5;
bottom_plate_thickness = 1.5;

// See https://matt3o.com/anatomy-of-a-keyboard/
switch_tolerance = 0.1;
switch_hole_size = 14 + switch_tolerance;
switch_distance_between_holes = 5.05;
switch_columns_count = 2;
switch_rows_count = 2;

kb2040_offset = 2;

magnet_height = 1.7;
magnet_radius = 4 / 2;
magnet_tolerance = 0.2;

internal_height = 15;
external_height = internal_height + top_thickness + bottom_plate_thickness;

internal_width = switch_distance_between_holes * switch_rows_count % 1 +
                 (switch_distance_between_holes + switch_hole_size) * switch_rows_count; // dep to internal vitamins
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

internal_length = switch_distance_between_holes * switch_columns_count % 1 +
                  (switch_hole_size + switch_distance_between_holes) * switch_columns_count + kb2040_width +
                  kb2040_offset + knob_base_length_on_body; // dep to switches, internal vitamins and rotary
external_length = internal_length + wall_thickness * 2;

module kb2040_transformation()
{
    translate([
        knob_base_length_on_body - kb2040_width + kb2040_offset, internal_width / 2, bottom_plate_thickness +
        kb2040_bottom_clearance
    ]) children();
}

module drv2605l_transformation()
{
    translate([
        knob_base_length_on_body - drv2605l_width - 2, internal_width / 2,
        bottom_plate_thickness + drv2605l_bottom_clearance + 5.5
    ]) children();
}

module ERM_motor_transformation()
{
    translate([
        knob_base_length_on_body - ERM_motor_radius * 2 - 5, 0, external_height - top_thickness + .5 -
        ERM_motor_thickness
    ]) children();
}

module hall_effect_sensor()
{
    cube([ 4, 4, 1.5 ]);
}

module knob_stl()
{
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

module body()
{
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

                translate([ external_length / 2, 0, bottom_plate_thickness ]) linear_extrude(internal_height)
                    offset(delta = -wall_thickness) projection()
                        rounded_cube([ external_length, external_width, thin ]);
            }

            // top plate
            translate([ external_length / 2, 0, bottom_plate_thickness + internal_height ])
                rounded_cube([ external_length, external_width, top_thickness ]);
        }

        // switches holes
        // translate([
        //     external_length - switch_hole_size / 2 - wall_thickness - switch_distance_between_holes, 0,
        //     internal_height
        // ])
        // {
        //     translate([ 0, +switch_hole_size / 2 + switch_distance_between_holes / 2, 0 ]) cube(switch_hole_size,
        //     true); translate([ 0, -switch_hole_size / 2 - switch_distance_between_holes / 2, 0 ])
        //     cube(switch_hole_size, true); translate([
        //         -switch_hole_size - switch_distance_between_holes,
        //         +switch_hole_size / 2 + switch_distance_between_holes / 2, 0
        //     ]) cube(switch_hole_size, true);
        //     translate([
        //         -switch_hole_size - switch_distance_between_holes,
        //         -switch_hole_size / 2 - switch_distance_between_holes / 2, 0
        //     ]) cube(switch_hole_size, true);
        // }

        // rotary cutout
        clip(zmin = bottom_plate_thickness) knob_transformation() translate([ 0, 0, knob_base_length ])
            rotary_encoder_cutout();

        // rotary access hole
        difference()
        {
            translate([ -knob_radius, -internal_width / 2, bottom_plate_thickness ]) cube([
                knob_base_length_on_body + knob_radius, internal_width, bottom_plate_thickness + internal_height -
                top_thickness
            ]);

            knob_transformation()
            {
                translate([ -knob_radius, -knob_radius, knob_base_length - knob_top_thickness ]) cube(knob_radius * 2);
                translate([ -knob_radius * 3 + wall_thickness, -knob_radius, 0 ]) cube(knob_radius * 2);
                translate([ -wall_thickness / 2, 0, 0 ]) difference()
                {
                    translate([ -knob_radius - 10, -knob_radius - 10, 0 ]) cube(knob_radius * 2 + 20);

                    cylinder(r = knob_radius - wall_thickness * 3, h = knob_base_length);
                    translate([ 0, -(knob_radius - wall_thickness * 3), 0 ])
                        cube((knob_radius - wall_thickness * 3) * 2);
                }
            }
        }

        // erm motor cutout
        ERM_motor_transformation() ERM_motor_cutout();

        // usb-c hole
        kb2040_transformation() kb2040_vitamin(true);
    }

    // kb2040 supports
    kb2040_transformation() kb2040_supports();
}

module bottom_plate_cutout()
{
    intersection()
    {
        body();

        union()
        {
            linear_extrude(bottom_plate_thickness) offset(delta = -wall_thickness) projection() clip(zmax = thin)
                body();
            length = knob_base_length_on_body + kb2040_offset + 1 + 5;
            height = internal_height - (ERM_motor_thickness + top_thickness * 2) + bottom_plate_thickness;
            difference()
            {
                translate([ 0, -internal_width / 2 + wall_thickness, 0 ]) cube([ length, internal_width, height ]);
                translate([ length, -internal_width / 2 + wall_thickness, 0 ]) rotate(knob_rotation)
                    cube(internal_width);
            }
        }
    }
}

module bottom_plate_stl()
{

    difference()
    {
        intersection()
        {
            body();
            bottom_plate_cutout();
        }

        // corner cutouts
        translate([ external_length, external_width / 2, 0 ]) rotate([ 0, 0, 45 ]) cube(10, true);
        mirror([ 0, 1, 0 ]) translate([ external_length, external_width / 2, 0 ]) rotate([ 0, 0, 45 ]) cube(10, true);

        // disassembly hole
        cylinder(r = 7, h = bottom_plate_thickness);
    }

    // pilar
    translate([
        external_length - switch_hole_size - wall_thickness - switch_distance_between_holes -
            switch_distance_between_holes / 2,
        0,
        bottom_plate_thickness
    ])
    {
        cylinder(r = magnet_radius, h = internal_height);
        width = switch_hole_size + switch_distance_between_holes;
        translate([ 0, 0, internal_height / 2 ]) cube([ wall_thickness, width, internal_height ], true);
        translate([ 0, 0, internal_height / 2 ]) cube([ width, wall_thickness, internal_height ], true);
    }
}

module cover_stl()
{
    difference()
    {
        body();
        bottom_plate_cutout();
    }
}

if ($preview)
{
    // kb2040_transformation() kb2040_vitamin();
    // drv2605l_transformation() drv2605l_vitamin();
    // ERM_motor_transformation() ERM_motor_vitamin();
    // knob_transformation() translate([ 0, 0, knob_base_length + knob_elevation ]) rotary_encoder_vitamin();

    render() // clip(ymin = 0)
    {
        knob_transformation() translate([ 0, 0, 0.5 + knob_base_length + knob_elevation ]) knob_stl();
        bottom_plate_stl();
        cover_stl();
    }

    color("red", 0.5)
    {
        // body();
    }
}