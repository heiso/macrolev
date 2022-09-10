$fn = 32;

x_size = (14 + 5.05) * 4;
y_size = (14 + 5.05);
knob_base_radius = y_size / 2;
knob_radius = knob_base_radius + 5;
h_size = (knob_radius / sqrt(2)) * 2;
knob_height = 16;
thin = 0.00001;

module knob()
{
    chamfer = knob_radius - knob_base_radius;

    hull()
    {
        translate([ 0, 0, chamfer ]) linear_extrude(height = thin) circle(r = knob_radius);
        linear_extrude(height = thin) offset(delta = -chamfer) circle(r = knob_radius);
    }

    translate([ 0, 0, chamfer ]) cylinder(h = knob_height - chamfer, r = knob_radius);

    translate([ 0, 0, knob_height ]) hull()
    {
        linear_extrude(height = thin) circle(r = knob_radius);
        translate([ 0, 0, 1 ]) linear_extrude(height = thin) offset(delta = -1) circle(r = knob_radius);
    }
}

module main_body()
{
    difference()
    {
        union()
        {
            translate([ (knob_base_radius * 2) / sqrt(2), 0, 0 ]) cube(size = [ x_size, y_size, h_size ]);

            hull()
            {
                linear_extrude(height = thin) square(size = [ (knob_base_radius * 2) / sqrt(2), y_size ]);

                translate([ (knob_base_radius * 2) / sqrt(2), 0, (knob_base_radius * 2) / sqrt(2) ])
                    linear_extrude(height = thin) square(size = [ thin, y_size ]);
            }
        }

        // hull()
        // {
        //     translate([ (knob_base_radius * 2) / sqrt(2), 0, h_size ]) linear_extrude(height = thin)
        //         square(size = [ x_size, (y_size / 4) * 3 ]);

        //     translate([ (knob_base_radius * 2) / sqrt(2), 0, (knob_base_radius * 2) / sqrt(2) ])
        //         linear_extrude(height = thin) square(size = [ x_size, thin ]);
        // }
    }

    // {
    //     knob_distance = (knob_radius / sqrt(2));
    //     // cube(size = [ x_size, y_size, knob_distance * 2 ]);

    //     // rotate([ 0, -45, 0 ]) cube(knob_base_radius * 2);

    //     mask_size = (knob_base_radius * 2) / sqrt(2);
    //     translate([ 0, 0, 4 + knob_distance ]) cube(size = [ mask_size, knob_base_radius * 2, mask_size * 2 ]);

    //     // hull()
    //     // {
    //     //     linear_extrude(height = thin) square([ x_size, y_size ])
    //     // }
    // }
}

module main()
{
    translate([ knob_base_radius / sqrt(2), knob_base_radius, knob_base_radius / sqrt(2) ]) rotate([ 0, -45, 0 ])
        translate([ 0, 0, 0.5 ]) knob();

    main_body();
}

main();