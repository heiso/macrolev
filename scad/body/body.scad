ROWS = 2;
COLUMNS = 4;
SWITCH_HEIGHT = 11;
SKIRT_HEIGHT = 5;
TOLERANCE = 0.1;
PLATE_THICKNESS = 1.5;      // Official spec for Cherry MX is 1.5 ± 0.1 mm
BOTTOM_PLATE_THICKNESS = 1; // Official spec for Cherry MX is 1.5 ± 0.1 mm
WALL_THICKNESS = 1.5;
CORNER_RADIUS = 2;

// See https://matt3o.com/anatomy-of-a-keyboard/
SWITCH_HOLE_SIZE = 14;
SWITCH_DISTANCE_BETWEEN_HOLES = 5.05;

$fn = 32;

keycap_placeholder_space = SWITCH_HOLE_SIZE + SWITCH_DISTANCE_BETWEEN_HOLES;
x_size = keycap_placeholder_space * COLUMNS + WALL_THICKNESS;
y_size = keycap_placeholder_space * ROWS + WALL_THICKNESS;
x_size_corrected = x_size - CORNER_RADIUS * 2;
y_size_corrected = y_size - CORNER_RADIUS * 2;
x_inner_size_corrected = x_size_corrected - WALL_THICKNESS * 2;
y_inner_size_corrected = y_size_corrected - WALL_THICKNESS * 2;

module hole()
{
    translate([ keycap_placeholder_space / 2, keycap_placeholder_space / 2, 0 ])
    {
        // Keycap Mock
        // translate([ 0, 0, 8 ]) color("red") linear_extrude(height = 8) square(keycap_placeholder_space, center =
        // true);

        // Plate hole
        color("blue") linear_extrude(height = PLATE_THICKNESS) square(SWITCH_HOLE_SIZE + TOLERANCE, center = true);

        // Structural hole
        translate([ 0, 0, PLATE_THICKNESS ]) linear_extrude(height = SWITCH_HEIGHT - PLATE_THICKNESS)
            offset(r = CORNER_RADIUS)
                square(keycap_placeholder_space - CORNER_RADIUS * 2 - WALL_THICKNESS, center = true);
    }
}

module main_body()
{
    difference()
    {
        // Body
        translate([ x_size / 2, y_size / 2, 0 ]) difference()
        {
            linear_extrude(height = SWITCH_HEIGHT) offset(r = CORNER_RADIUS)
                square([ x_size_corrected, y_size_corrected ], center = true);

            // hull()
            // {
            //     translate([ 0, 0, SWITCH_HEIGHT / 4 ]) linear_extrude(height = 0.0001)
            //         offset(delta = -WALL_THICKNESS - keycap_placeholder_space / 3)
            //             square([ x_size, y_size ], center = true);
            //     translate([ 0, 0, SWITCH_HEIGHT ]) linear_extrude(height = 0.0001) offset(delta = -WALL_THICKNESS *
            //     2)
            //         square([ x_size, y_size ], center = true);
            // }
        }

        // Holes
        translate([ WALL_THICKNESS / 2, WALL_THICKNESS / 2, 0 ])
        {
            for (column = [0:COLUMNS - 1])
            {
                for (row = [0:ROWS - 1])
                {
                    translate([ column * keycap_placeholder_space, row * keycap_placeholder_space, 0 ]) hole();
                }
            }
        }
    }

    // Skirt
    translate([ x_size / 2, y_size / 2, SWITCH_HEIGHT ]) difference()
    {
        linear_extrude(height = SKIRT_HEIGHT) offset(r = CORNER_RADIUS)
            square([ x_size_corrected, y_size_corrected ], center = true);

        linear_extrude(height = SKIRT_HEIGHT) offset(r = CORNER_RADIUS)
            square([ x_inner_size_corrected, y_inner_size_corrected ], center = true);
    }
}

// remove it and at only support for hall effect sensors
// remove it and at only support for hall effect sensors
// remove it and at only support for hall effect sensors
// remove it and at only support for hall effect sensors
// remove it and at only support for hall effect sensors
// remove it and at only support for hall effect sensors
module bottom_plate()
{
    chamfer_height = 1 / 3;
    chamfer_width = 1 / 3;

    translate([ x_size / 2, y_size / 2, 0 ])
    {
        color("blue") linear_extrude(height = BOTTOM_PLATE_THICKNESS) offset(r = CORNER_RADIUS)
            square([ x_size_corrected, y_size_corrected ], center = true);

        translate([ 0, 0, BOTTOM_PLATE_THICKNESS ]) linear_extrude(height = 1 - chamfer_height)
            offset(r = CORNER_RADIUS) square([ x_inner_size_corrected, y_inner_size_corrected ], center = true);

        translate([ 0, 0, BOTTOM_PLATE_THICKNESS + 1 - chamfer_height ]) hull()
        {
            translate([ 0, 0, 0 ]) linear_extrude(height = 0.0001) offset(r = CORNER_RADIUS)
                square([ x_inner_size_corrected, y_inner_size_corrected ], center = true);
            translate([ 0, 0, chamfer_height ]) linear_extrude(height = 0.0001) offset(r = CORNER_RADIUS)
                square([ x_inner_size_corrected - chamfer_width * 2, y_inner_size_corrected - chamfer_width * 2 ],
                       center = true);
        }
    }
}

module main()
{
    main_body();

    translate([ 0, y_size + 10, 0 ]) bottom_plate();
}

main();