include <./void_switch/void_switch.scad>
include <openscad-bits/lib.scad>
use <openscad-bits/lib.scad>

IS_PRESSED = false;
SWITCH_COLUMNS_COUNT = 3;
SWITCH_ROWS_COUNT = 2;
SWITCH_PLATE_TILT = 6;
KEYCAP_SKIRT_HEIGHT = -3.41;
// KEYCAP_SKIRT_HEIGHT = 5;

/* [Hidden] */

$fn = 32;
border_radius = 2;

// See https://matt3o.com/anatomy-of-a-keyboard/
switch_hole_tolerance = 0.1;
switch_lip_tolerance = 0.2;
switch_hole_size = 14;
switch_distance_between_holes = 5.05;
switch_columns_count = SWITCH_COLUMNS_COUNT;
switch_rows_count = SWITCH_ROWS_COUNT;
switch_travel = 4;
switch_lip_height = .5;
switch_lip_width = .6;
switch_magnet_under_plate_height = 9.7; // 9.7 // 9.4
switch_under_plate_height = 10;
switch_placeholder_width = switch_distance_between_holes + switch_hole_size;

keycap_min_height = 7;
keycap_width = 19.5;
keycap_skirt_height = KEYCAP_SKIRT_HEIGHT;

switch_plate_thickness = 1.5;
switch_plate_tolerance = 0.05;
switch_plate_border_radius = .5;
switch_plate_x = switch_plate_border_radius * 2 + switch_columns_count * switch_placeholder_width;
switch_plate_y = switch_plate_border_radius * 2 + switch_rows_count * switch_placeholder_width;

hall_effect_sensor_x = 4;
hall_effect_sensor_y = 3;
hall_effect_sensor_z = 1.5;
hall_effect_sensor_tolerance = 0.1;

diode_r = 2.7 / 2;
diode_h = 5;
diode_tolerance = 0.1;

wedge_height = hall_effect_sensor_z + 1;
wedge_support_width = 2.5;
wedge_min_height = .5;

kb2040_support_width = 1;

body_modules_wires_gap = 2;
body_wall_thickness = 1.5;
body_modules_space_width = kb2040_width + kb2040_support_width * 2;
body_x = body_wall_thickness + body_modules_space_width + body_wall_thickness + switch_plate_tolerance +
         switch_plate_x + switch_plate_tolerance + body_wall_thickness;
body_y = switch_plate_y + (body_wall_thickness + switch_plate_tolerance) * 2;
body_bottom_plate_min_thickness = .5;
body_bottom_plate_thickness = body_y * sin(SWITCH_PLATE_TILT) + body_bottom_plate_min_thickness;
body_bottom_plate_x = body_x - body_wall_thickness - body_modules_space_width - body_wall_thickness;
body_top_plate_thickness = 2;
body_z =
    body_bottom_plate_thickness + wedge_height + switch_under_plate_height + switch_lip_height + keycap_skirt_height;

module kb2040_origin()
{
    translate([
        -body_x / 2 + body_wall_thickness + body_modules_space_width / 2, body_y / 2 - body_wall_thickness - 2,
        +kb2040_thickness + kb2040_bottom_clearance + 7
    ]) rotate([ 0, 180, 180 ]) children();
}

module adafruit_drv2605L_origin()
{
    switch_plate_tilt() translate(
        [ -body_x / 2 + adafruit_drv2605L_width / 2 + body_wall_thickness, 15, body_z - body_top_plate_thickness ])
        rotate([ 180, 0, 0 ]) children();
}

module ERM_motor_origin()
{
    translate([ 0, 0, 0 ]) switch_plate_origin() translate([ 0, ERM_motor_height / 2, ERM_motor_radius ])
        rotate([ 90, 0, 0 ]) children();
}

module hall_effect_sensor(cutout = false)
{
    if (cutout)
    {
        translate([ 0, 10 / 2, (hall_effect_sensor_z + hall_effect_sensor_tolerance) / 2 ]) cube(
            [
                hall_effect_sensor_x + hall_effect_sensor_tolerance,
                hall_effect_sensor_y + hall_effect_sensor_tolerance + 10, hall_effect_sensor_z +
                hall_effect_sensor_tolerance
            ],
            true);
    }
    else
    {
        color("black") translate([ 0, 0, hall_effect_sensor_z / 2 ])
            cube([ hall_effect_sensor_x, hall_effect_sensor_y, hall_effect_sensor_z ], true);
    }
}

module diode(cutout = false)
{
    if (cutout)
    {
        cylinder(r = diode_r + diode_tolerance / 2, h = diode_h + diode_tolerance, true);
    }
    else
    {
        color("black") cylinder(r = diode_r, h = diode_h, true);
    }
}

module hall_effect_sensor_support()
{
    difference()
    {
        translate([ 0, 0, hall_effect_sensor_z / 2 ])
            cube([ hall_effect_sensor_x + 2, hall_effect_sensor_y + 2, hall_effect_sensor_z ], true);

        hall_effect_sensor(true);
    }
}

module hall_effect_sensor_origin()
{
    translate([ 0, 0, (diode_r * 2) - hall_effect_sensor_z ]) hall_effect_sensor_xy_origin() children();
}

module diode_origin()
{
    translate([ -4, 2, diode_r ]) rotate([ 45, 90, 0 ]) children();
}

module hall_effect_sensor_xy_origin()
{
    translate([ 4, -4, 0 ]) rotate([ 0, 0, 45 ]) children();
}

module void_switch_support()
{
    difference()
    {
        translate([ 0, 0, -switch_under_plate_height ])
            linear_extrude(switch_under_plate_height + switch_plate_thickness + switch_lip_height) square(
                [ switch_distance_between_holes + switch_hole_size, switch_distance_between_holes + switch_hole_size ],
                true);

        void_switch(cutout = true);
    }
}

module switch_origin()
{
    translate([ 0, 0, switch_under_plate_height ]) children();
}

module switch_xy_origins()
{
    translate([
        switch_placeholder_width / 2 - (switch_columns_count * switch_placeholder_width) / 2,
        switch_placeholder_width / 2 - (switch_rows_count * switch_placeholder_width) / 2, 0
    ])
    {
        for (column = [0:switch_columns_count - 1], row = [0:switch_rows_count - 1])
        {
            translate([ column * switch_placeholder_width, row * switch_placeholder_width, 0 ]) children();
        }
    }
}

module switch_plate_tilt()
{
    translate([ 0, 0, -body_y / 2 * sin(SWITCH_PLATE_TILT) ]) rotate([ SWITCH_PLATE_TILT, 0, 0 ]) children();
}

module switch_plate()
{
    difference()
    {
        union()
        {
            switch_xy_origins() switch_origin() void_switch_support();

            difference()
            {
                chamfer = 1;
                translate([ 0, 0, switch_under_plate_height - switch_plate_thickness ])
                {

                    linear_extrude(switch_plate_thickness + switch_lip_height) offset(switch_plate_border_radius)
                        offset(-switch_plate_border_radius) square([ switch_plate_x, switch_plate_y ], true);

                    hull()
                    {
                        linear_extrude(thin) offset(switch_plate_border_radius) offset(-switch_plate_border_radius)
                            square([ switch_plate_x, switch_plate_y ], true);

                        translate([ 0, 0, -chamfer ]) linear_extrude(thin) offset(-switch_plate_border_radius)
                            square([ switch_plate_x, switch_plate_y ], true);
                    }
                }

                translate([ 0, 0, switch_under_plate_height - switch_plate_thickness - chamfer ])
                    linear_extrude(switch_plate_thickness + switch_lip_height + chamfer)
                        offset(-switch_plate_border_radius) square([ switch_plate_x, switch_plate_y ], true);
            }
        }

        // translate([ 0, ERM_motor_height / 2, ERM_motor_radius ]) rotate([ 90, 180, 0 ]) ERM_motor(cutout = true);
    }
}

module switch_plate_z_origin()
{
    translate([ 0, 0, 4 ]) children();
}

module switch_plate_xy_origin()
{
    translate([ body_modules_space_width / 2 + body_wall_thickness / 2, 0, 0 ]) children();
}

module switch_plate_origin()
{
    // switch_plate_tilt()
    switch_plate_z_origin() switch_plate_xy_origin() children();
}

module body()
{
    module wires_gap_support()
    {
        hull()
        {
            linear_extrude(thin) square([ body_wall_thickness, 2 ], true);

            translate([ 0, 0, body_modules_wires_gap ]) linear_extrude(thin)
                square([ body_wall_thickness, body_modules_wires_gap * 2 ], true);
        }
    }

    difference()
    {
        union()
        {

            clip(zmin = 0) switch_plate_tilt() difference()
            {
                union()
                {
                    // Bottom plate
                    translate([ body_modules_space_width / 2, 0, 0 ]) linear_extrude(body_bottom_plate_thickness)
                        square([ body_bottom_plate_x, body_y - body_wall_thickness * 2 ], true);

                    // External walls
                    linear_extrude(body_z) difference()
                    {
                        offset(body_wall_thickness) offset(-body_wall_thickness) square([ body_x, body_y ], true);
                        offset(-body_wall_thickness) square([ body_x, body_y ], true);
                    }

                    // Inner wall
                    translate([
                        -body_x / 2 + +body_wall_thickness + body_modules_space_width + body_wall_thickness / 2, 0,
                        body_bottom_plate_thickness
                    ])
                    {

                        translate([ 0, 0, body_modules_wires_gap ])
                            linear_extrude(body_z - body_bottom_plate_thickness - body_modules_wires_gap)
                                square([ body_wall_thickness, body_y - body_wall_thickness * 2 ], true);

                        translate([ 0, -body_y / 2, 0 ])
                        {
                            translate([ 0, body_wall_thickness + 1, 0 ]) wires_gap_support();
                            translate([ 0, body_y - body_wall_thickness - 1, 0 ]) wires_gap_support();

                            for (row = [1:switch_rows_count * 2 - 1])
                            {
                                translate([ 0, body_wall_thickness + (switch_placeholder_width / 2) * row, 0 ])
                                {
                                    wires_gap_support();
                                }
                            }
                        }
                    }

                    // Hall effect sensor supports
                    switch_plate_xy_origin() switch_xy_origins() hall_effect_sensor_origin() translate([ 0, 0, -thin ])
                        hall_effect_sensor_support();

                    // Top
                    translate([
                        -body_x / 2 + body_wall_thickness + body_modules_space_width / 2, 0,
                        -body_top_plate_thickness / 2 +
                        body_z
                    ])
                        cube(
                            [
                                body_modules_space_width + body_wall_thickness, body_y - body_wall_thickness * 2,
                                body_top_plate_thickness
                            ],
                            true);
                }

                // Holes to remove plate
                switch_plate_xy_origin() translate([
                    switch_placeholder_width / 2 - (switch_columns_count * switch_placeholder_width) / 2,
                    switch_placeholder_width / 2 - (switch_rows_count * switch_placeholder_width) / 2, 0
                ])
                {
                    if (switch_rows_count > 1 && switch_columns_count > 1)
                    {
                        for (column = [0:switch_columns_count - 2], row = [0:switch_rows_count - 2])
                        {
                            translate([
                                (switch_placeholder_width / 2) + switch_placeholder_width * column,
                                (switch_placeholder_width / 2) + switch_placeholder_width * row, 0
                            ]) cylinder(h = body_z, r = 2);
                        }
                    }
                    else
                    {
                        translate([ 0, (switch_placeholder_width / 2), 0 ]) cylinder(h = body_z, r = 2);
                    }
                }
            }

            // kb2040 support
            kb2040_origin() kb2040_supports();
            kb2040_origin() translate([ 0, kb2040_length + 0.1, 0 ]) linear_extrude(2) translate([ 0, 2 / 2, 0 ])
                square([ body_modules_space_width, 2 ], true);
        }

        // Bottom plate recess
        linear_extrude(.5) translate([ body_x / 2 - body_bottom_plate_x / 2, 0, 0 ]) offset(body_wall_thickness)
            offset(-body_wall_thickness * 2) square([ body_bottom_plate_x, body_y ], true);

        // kb2040 cutout
        kb2040_origin() kb2040(cutout = true);
    }
}

module test_body()
{
    difference()
    {
        union()
        {
            // Bottom plate
            translate([ body_modules_space_width / 2, 0, 0 ]) linear_extrude(1)
                square([ body_bottom_plate_x, body_y - body_wall_thickness * 2 ], true);

            // External walls
            translate([ body_modules_space_width / 2 + body_wall_thickness / 2, 0, 0 ]) linear_extrude(body_z)
                difference()
            {
                offset(body_wall_thickness) offset(-body_wall_thickness)
                    square([ body_x - body_modules_space_width - body_wall_thickness, body_y ], true);
                offset(-body_wall_thickness)
                    square([ body_x - body_modules_space_width - body_wall_thickness, body_y ], true);
            }
        }

        // Cable hole
        translate([ 0, 13, 0 ]) rotate([ 0, 90, 0 ]) cylinder(r = 3, h = body_x, center = true);
        translate([ 0, -6, 0 ]) rotate([ 0, 90, 0 ]) cylinder(r = 3, h = body_x, center = true);
    }
}

module fake_pcb()
{
    height = diode_r * 2 + .2;
    translate([ body_modules_space_width / 2 + body_wall_thickness / 2, 0, 0 ]) difference()
    {
        union()
        {

            difference()
            {
                // Plate
                switch_xy_origins() linear_extrude(height)
                    square(switch_distance_between_holes + switch_hole_size, true);

                // Hall effect sensor supports
                translate([ 0, 0, -.1 ]) switch_xy_origins() hall_effect_sensor_origin()
                    hall_effect_sensor(cutout = true);

                // Channels
                switch_xy_origins()
                {
                    translate([ -2, 2, diode_r ]) cube([ 12, 12, diode_r * 2 ], true);
                    translate([ -16.5, 0, 0 ]) cube([ switch_placeholder_width + 1, 8, diode_r * 2 ]);
                }
            }

            translate([
                -(switch_plate_x / 2 - switch_placeholder_width / 8.5),
                switch_plate_y / 2 - switch_placeholder_width / 8.5, 0
            ]) linear_extrude(height = height) circle(r = switch_placeholder_width / 8.5);
            translate([
                switch_plate_x / 2 - switch_placeholder_width / 8.5,
                switch_plate_y / 2 - switch_placeholder_width / 8.5, 0
            ]) linear_extrude(height = height) circle(r = switch_placeholder_width / 8.5);
            translate([
                switch_plate_x / 2 - switch_placeholder_width / 8.5,
                -(switch_plate_y / 2 - switch_placeholder_width / 8.5), 0
            ]) linear_extrude(height = height) circle(r = switch_placeholder_width / 8.5);
            translate([
                -(switch_plate_x / 2 - switch_placeholder_width / 8.5),
                -(switch_plate_y / 2 - switch_placeholder_width / 8.5), 0
            ]) linear_extrude(height = height) circle(r = switch_placeholder_width / 8.5);
        }
    }
}

module wedge()
{
    difference()
    {
        union()
        {
            switch_xy_origins() linear_extrude(wedge_height)
                square(switch_distance_between_holes + switch_hole_size, true);

            translate([
                -(switch_plate_x / 2 - switch_placeholder_width / 8.5),
                switch_plate_y / 2 - switch_placeholder_width / 8.5, 0
            ]) linear_extrude(height = wedge_height) circle(r = switch_placeholder_width / 8.5);
            translate([
                switch_plate_x / 2 - switch_placeholder_width / 8.5,
                switch_plate_y / 2 - switch_placeholder_width / 8.5, 0
            ]) linear_extrude(height = wedge_height) circle(r = switch_placeholder_width / 8.5);
            translate([
                switch_plate_x / 2 - switch_placeholder_width / 8.5,
                -(switch_plate_y / 2 - switch_placeholder_width / 8.5), 0
            ]) linear_extrude(height = wedge_height) circle(r = switch_placeholder_width / 8.5);
            translate([
                -(switch_plate_x / 2 - switch_placeholder_width / 8.5),
                -(switch_plate_y / 2 - switch_placeholder_width / 8.5), 0
            ]) linear_extrude(height = wedge_height) circle(r = switch_placeholder_width / 8.5);
        }

        translate([ 0, 0, wedge_height / 2 ]) switch_xy_origins() cube(
            [
                switch_placeholder_width - wedge_support_width, switch_placeholder_width - wedge_support_width,
                wedge_height
            ],
            true);

        translate([ 0, 0, -wedge_min_height ]) switch_xy_origins() linear_extrude(height = wedge_height)
        {
            square([ switch_placeholder_width + .1, switch_placeholder_width / 1.5 ], true);
            square([ switch_placeholder_width / 1.5, switch_placeholder_width + .1 ], true);
        }
    }
}

module wedge_origin()
{
    // switch_plate_tilt()
    switch_plate_xy_origin() translate([ 0, 0, body_bottom_plate_thickness ]) children();
}

// - resolve hall effect sensor range problem
// - resolve hall effect sensor range problem
// - resolve hall effect sensor range problem
// - resolve hall effect sensor range problem
// - resolve hall effect sensor range problem
// - usb-c link to main pcb with flex cable
// - printed proto plate (ppb haha)
// - link ppb and switch plate together with magnets
// - add rotary encoder
// - kicad
// - esp32 for BLE

if ($preview)
{
    // render()
    // {
    //     body();
    // }
    render() clip(xmin = 0)
    {
        test_body();
    }

    color("blue") render() // clip(xmax = 13)
    {
        translate([ 0, 0, 1 ]) fake_pcb();
    }

    // color("blue") render()
    // {
    //     wedge_origin() wedge();
    // }

    color("grey") render() // clip(xmax = 10)
    {
        switch_plate_origin() switch_plate();
    }

    color("red")
    {
        switch_plate_origin() switch_xy_origins() switch_origin() void_switch(down = IS_PRESSED, keycap = true);
    }

    // switch_plate_tilt()
    switch_plate_xy_origin() switch_xy_origins() hall_effect_sensor_origin() hall_effect_sensor();

    // switch_plate_tilt()
    switch_plate_xy_origin() switch_xy_origins() diode_origin() diode();

    kb2040_origin() kb2040();

    adafruit_drv2605L_origin() adafruit_drv2605L();
}
else
{
    translate([ 0, +body_y + 10, 0 ]) rotate([ 0, 180, 0 ]) fake_pcb();
    switch_plate();
    translate([ 0, -body_y - 10, 0 ]) test_body();
    // rotate([ -SWITCH_PLATE_TILT, 0, 0 ]) translate([ 0, -body_y - 10, 0 ]) body();
}