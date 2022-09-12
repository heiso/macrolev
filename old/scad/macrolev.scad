$fn = 32;
thin = 0.00001;
border_radius = 2;
// See https://matt3o.com/anatomy-of-a-keyboard/
switch_hole_size = 14;
switch_distance_between_holes = 5.05;
keycap_placeholder_space = switch_hole_size + switch_distance_between_holes;
switch_tolerance = 0.1;

wall_thickness = 1.5;

x_size = keycap_placeholder_space * 5;
y_size = keycap_placeholder_space + 5;
h_size = 15;
body_base_x = x_size + wall_thickness * 2;
body_base_y = y_size + wall_thickness * 2;

knob_chamfer = 1;
knob_oversized_by = (1 / 5) * body_base_y;
knob_radius = (body_base_y / 2) + knob_oversized_by / 2;
knob_height = 11;
knob_elevation = 5;
knob_rotation = [ 0, -45, 0 ];

shaft_height = 10.20;
shaft_radius = 3;
shaft_tolerance = 0.075;

module cut()
{
    h = 100;
    x = 200;
    y = 50;
    translate([ 0, -y / 2, 0 ]) cube([ x, y, h ], true);
}

module table()
{
    h = 25;
    x = 100;
    y = 100;
    translate([ 0, 0, -h / 2 ]) cube([ x, y, h ], true);
}

module rotary_encoder()
{
    translate([ 0, 0, -10 ])
    {
        translate([ 0, 0, 5 ]) cube([ 12, 12, 10 ], center = true);
        translate([ 0, 0, 10 ]) cylinder(7, r = 7 / 2);
        translate([ 0, 0, 10 + 7 ]) cylinder(12.5, r = 6 / 2);
    }
}

module knob()
{
    translate([ 0, 0, 0.5 + knob_elevation ])
    {
        difference()
        {
            union()
            {
                cylinder(h = knob_height - knob_chamfer, r = knob_radius);

                translate([ 0, 0, knob_height - knob_chamfer ]) hull()
                {
                    linear_extrude(height = thin) circle(r = knob_radius);
                    translate([ 0, 0, knob_chamfer ]) linear_extrude(height = thin) offset(delta = -knob_chamfer)
                        circle(r = knob_radius);
                }
            }

            difference()
            {
                cylinder(h = knob_height - wall_thickness, r = shaft_radius + shaft_tolerance);
                translate([ -shaft_radius, shaft_radius / 1.7, 0 ]) cube([ shaft_radius * 2, 10, shaft_height ]);
            }
        }
    }
}

module knob_base()
{
    h = 25;

    translate([ 0, 0, -h ])
    {
        translate([ 0, 0, h + knob_elevation - wall_thickness ]) linear_extrude(height = wall_thickness) difference()
        {
            circle(r = knob_radius);
            circle(r = shaft_radius);
        }
        linear_extrude(height = knob_elevation + h) difference()
        {
            circle(r = knob_radius);
            offset(delta = -wall_thickness) circle(r = knob_radius);
        }
    }
}

module knob_assembly()
{
    difference()
    {
        translate([ 0, 0, knob_radius / sqrt(2) ]) rotate(knob_rotation)
        {
            // translate([ 0, 0, knob_elevation - wall_thickness ]) color([ 0, 0, 1, 0.8 ]) rotary_encoder();
            // knob();
            knob_base();
        }

        // hole for body

        color("red") translate([ knob_radius / sqrt(2), 0, 0 ]) rotate([ 0, 90, 0 ])
            cylinder(h = x_size / 2, r = y_size / 2);
        table();
    }
}

module body()
{
    difference()
    {
        translate([ body_base_x / 2, 0, 0 ])
        {
            body_base_x_corrected = body_base_x - (border_radius * 2);
            body_base_y_corrected = body_base_y - (border_radius * 2);

            color("blue") translate([ 0, 0, h_size - wall_thickness ]) linear_extrude(height = wall_thickness)
                offset(r = border_radius) square([ body_base_x_corrected, body_base_y_corrected ], center = true);

            linear_extrude(height = h_size - wall_thickness) difference()
            {
                offset(r = border_radius) square([ body_base_x_corrected, body_base_y_corrected ], center = true);
                offset(r = border_radius) offset(delta = -wall_thickness)
                    square([ body_base_x_corrected, body_base_y_corrected ], center = true);
            }
        }

        // hole for knob_base
        h = 50;
        translate([ 0, 0, knob_radius / sqrt(2) ]) rotate(knob_rotation)
        {
            cylinder(h = h, r = knob_radius, center = true);
            translate([ -knob_radius / 2, 0, 0 ]) cube(size = [ knob_radius, knob_radius * 2, h ], center = true);
        }

        // switch holes
        for (column = [0:3 - 1])
        {
            translate([ x_size - column * keycap_placeholder_space - keycap_placeholder_space / 2, 0, h_size ])
                cube(switch_hole_size + switch_tolerance, center = true);
        }
    }
}

module main()
{
    difference()
    {
        union()
        {
            knob_assembly();
            body();
        }

        // difference()
        // {
        // translate([ body_base_x / 2, 0, (h_size - wall_thickness) / 2 ])
        //     cube([ body_base_x - wall_thickness * 2, body_base_y - wall_thickness * 2, h_size - wall_thickness ],
        //          center = true);
        // }
        // cut();
    }
}

main();