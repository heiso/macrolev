module void_switch(down = false, cutout = false, keycap = true)
{
    module keycap()
    {
        if (keycap)
        {
            rotate([ 0, 180, 0 ]) import("./keycap.3mf");
        }
    }

    if (cutout)
    {
        // under plate
        translate([ 0, 0, -(switch_under_plate_height - switch_plate_thickness) / 2 - switch_plate_thickness - 10 / 2 ])
            cube(
                [
                    switch_hole_size + switch_hole_tolerance * 2, switch_placeholder_width - 2,
                    switch_under_plate_height - switch_plate_thickness + 10
                ],
                true);

        // plate
        translate([ 0, 0, -switch_plate_thickness / 2 ]) cube(
            [
                switch_hole_size + switch_hole_tolerance * 2, switch_hole_size + switch_hole_tolerance * 2,
                switch_plate_thickness
            ],
            true);

        // lip
        translate([ 0, 0, 0 ]) linear_extrude(switch_lip_height) offset(0.5) offset(-0.5)
            square(switch_hole_size + switch_lip_width * 2 + switch_lip_tolerance * 2, true);

        // keycap
        translate([ 0, 0, switch_lip_height ]) linear_extrude(20)
            square(switch_placeholder_width + switch_hole_tolerance, true);
    }
    else
    {
        translate([ 0, 0, switch_lip_height ]) rotate([ 0, 180, 0 ])
        {
            if (down)
            {
                import("./void_switch_keydown.3mf");
                keycap();
            }
            else
            {
                translate([ 0, 0, -switch_travel ]) keycap();
                import("./void_switch_keyup.3mf");
            }
        }
    }
}