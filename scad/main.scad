//! Project description in Markdown format before the first include.
$pp1_colour = "grey"; // Override any global defaults here if required, see NopSCADlib/global_defs.scad.
include <NopSCADlib/lib.scad>
use <NopSCADlib/vitamins/potentiometer.scad>

thin = 0.00001;
border_radius = 2;
// See https://matt3o.com/anatomy-of-a-keyboard/
switch_hole_size = 14;
switch_distance_between_holes = 5.05;
keycap_placeholder_space = switch_hole_size + switch_distance_between_holes;
switch_tolerance = 0.1;

top_thickness = 1.5;
wall_thickness = 1.5;

size_offset = 7;
x_size = keycap_placeholder_space * 5 + size_offset;
y_size = keycap_placeholder_space + size_offset;
h_size = 15;
body_base_x = x_size + wall_thickness * 2;
body_base_y = y_size + wall_thickness * 2;

knob_chamfer = 1;
knob_oversized_by = (1 / 5) * body_base_y;
knob_radius = (body_base_y / 2) + knob_oversized_by / 2;
knob_height = 17;
knob_elevation = 5;
knob_rotation = [ 0, -15, 0 ];

shaft_height = 10.20;
shaft_radius = 3;
shaft_base_radius = 7 / 2;
shaft_tolerance = 0.075;

module kb2040_pcb(cutout = false)
{
    x = 18;
    y = 21;
    thickness = 1.6;
    translate([ 22, body_base_y / 2 - wall_thickness, 2 ]) translate([ x / 2, -y / 2, 0 ])
    {
        color("black") translate([ 0, 0, thickness / 2 ]) cube([ x, y, thickness ], center = true);
        translate([ 0, (y / 2) - (7.35 / 2) + 1, thickness ]) rotate([ 0, 0, 90 ]) usb_C(cutout);
    }
}

module drv2605l_pcb()
{
    x = 18;
    y = 26;
    thickness = 1.6;
    translate([ 22, body_base_y / 2 - wall_thickness, 8 ]) translate([ x / 2, -y / 2, thickness / 2 ])
    {
        color("black") cube([ x, y, thickness ], center = true);
    }
}

module vibrator()
{
    r = 10 / 2;
    thickness = 3.5;
    translate([ 27, 0, h_size - top_thickness - thickness ]) translate([ r, r, 0 ])
    {
        color("silver") cylinder(h = thickness, r = r);
    }
}

module knob_transformation()
{
    translate([ knob_radius / sqrt(2), 0, knob_radius / sqrt(2) ]) rotate(knob_rotation) children();
}

module rotary_encoder()
{
    thickness = top_thickness + 2;
    translate([ 0, 0, knob_elevation - thickness ]) rotate([ 0, 180, 0 ])
    {
        translate([ 0, 0, -thickness ]) pot_nut(KY_040_encoder, false);
        potentiometer(KY_040_encoder);
    }
}

module knob_stl()
{
    stl("knob");
    translate([ 0, 0, 0.5 + knob_elevation ])
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

            translate([ 0, 0, (knob_height - knob_chamfer) - shaft_height ])
            {
                difference()
                {
                    cylinder(h = shaft_height, r = shaft_radius + shaft_tolerance);
                    translate([ -shaft_radius, shaft_radius / 1.7, 0 ]) cube([ shaft_radius * 2, 10, shaft_height ]);
                }
            }

            cylinder(h = knob_height - knob_chamfer - shaft_height, r = knob_radius - 2);
        }
    }
}

module body_stl()
{
    stl("body");

    difference()
    {
        union()
        {
            difference()
            {
                body_base_x_corrected = body_base_x - (border_radius * 2);
                body_base_y_corrected = body_base_y - (border_radius * 2);
                linear_extrude(height = h_size) offset(r = border_radius) translate([ body_base_x / 2, 0, 0 ])
                    square([ body_base_x_corrected, body_base_y_corrected ], center = true);

                knob_transformation()
                {
                    h = 50 + knob_elevation;
                    translate([ -knob_radius / 2, 0, h / 2 - 25 ]) cube([ knob_radius, knob_radius * 2, h ], true);
                    translate([ 0, 0, -25 ]) cylinder(h = h, r = knob_radius);
                }
            }

            clip(zmin = 0) knob_transformation() translate([ 0, 0, -40 ])
                cylinder(h = 40 + knob_elevation, r = knob_radius);
        }

        union()
        {
            difference()
            {
                body_base_x_corrected = body_base_x - wall_thickness - (border_radius * 2);
                body_base_y_corrected = body_base_y - wall_thickness - (border_radius * 2);
                linear_extrude(height = h_size - top_thickness) offset(r = border_radius)
                    translate([ (body_base_x) / 2, 0, 0 ])
                        square([ body_base_x_corrected, body_base_y_corrected ], center = true);

                knob_transformation()
                {
                    h = 50 + knob_elevation;
                    translate([ -knob_radius / 2, 0, h / 2 - 25 ]) cube([ knob_radius, knob_radius * 2, h ], true);
                    translate([ 0, 0, -25 ]) cylinder(h = h, r = knob_radius - wall_thickness);
                }
            }

            // knob base hollow
            knob_transformation()
            {
                h = 40 + knob_elevation;
                translate([ 0, 0, -40 - 6.5 ]) cylinder(h = h - top_thickness, r = knob_radius - wall_thickness / 2);
                scale([1.1,1.1,1.1]) rotary_encoder();
            }

            // switch holes
            for (column = [0:3 - 1])
            {

                translate([
                    x_size - size_offset / 2 - column * keycap_placeholder_space - keycap_placeholder_space / 2, 0,
                    h_size
                ]) cube(switch_hole_size + switch_tolerance, center = true);
            }

            // usbc hole
            kb2040_pcb(true);
        }
    }

    // internal supports
    for (column = [0:4 - 1])
    {
        translate([ x_size - size_offset / 2 - column * keycap_placeholder_space, 0, (h_size - top_thickness) / 2 ])
        {
            difference()
            {
                cube([ wall_thickness, body_base_y, h_size - top_thickness ], center = true);
                translate([ 0, 0, -body_base_y / 4 - 2 ]) rotate([ 0, 90, 0 ])
                    cylinder(h = wall_thickness, r = (body_base_y - wall_thickness) / 2, center = true);
            }
        }
    }
}

//! Assembly instructions in Markdown format in front of each module that makes an assembly.
module main_assembly() assembly("main")
{
    kb2040_pcb();
    drv2605l_pcb();
    vibrator();
    knob_transformation() rotary_encoder();

    stl_colour(pp1_colour) render() clip(ymin = 0)
    {
        // knob_transformation() knob_stl();
        body_stl();
    }
}

if ($preview)
    main_assembly();