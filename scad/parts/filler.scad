use <../utils.scad>
// 2-4mm of travel is traditional for mechanical key switches
TOTAL_TRAVEL = 4; // [1:0.1:20]

/* [Advanced Stuff] */

BODY_LENGTH = 14.2;
BODY_WIDTH = 14.2;
BODY_HEIGHT = 7;
BODY_CORNER_RADIUS = 0.5;
WALL_THICKNESS = 1.35;
COVER_THICKNESS = 0.5; // [0:0.1:1.5]
COVER_OVERHANG = 0.6;  // [0:0.1:1.5]

CLIP_BRIDGE_THICKNESS = 0.3;
SWITCH_PLATE_THICKNESS = 1.5;
SWITCH_PLATE_TOLERANCE = 0.2;

BODY_TAPER = 1.1;

// OpenSCAD's rendering resolution (64 worked well for me but feel free to go higher if you're patient--unlike me)
$fn = 64;

module switch_body(length, width, travel, cover_overhang = 1, taper = 1.1, wall_thickness = 1.4, cover_thickness = 0.6,
                   plate_thickness = 1.5, plate_tolerance = 0.2, corner_radius = 0.5, bridge_thickness = 0.3,
                   height = 0)
{
    body_height = height;       // For clarity in the code
    snap_clip_protrusion = 0.2; // How much the little snap lip things stick out from the body (it gets rotate 45° so
                                // it's a little less, actually)
    snap_clip_thickness = 0.85; // How thick the clips are

    clip_width = width / 6;
    clip_side_hole_width = 0.35; // How wide the cutouts are beside each clip
    note(str("UNDER_PLATE_HEIGHT: ", body_height - cover_thickness));
    difference()
    {
        union()
        {
            // Generate the part of the body that overhangs the hole on the plate
            translate([ 0, 0, cover_thickness / 2 ]) hull()
            { // This is tapered a bit to make removing the switch easier
                squarish_rpoly(
                    xy =
                        [ length + cover_overhang - plate_tolerance / 2, width + cover_overhang - plate_tolerance / 2 ],
                    h = cover_thickness, r = corner_radius, center = true);
                translate([ 0, 0, -cover_thickness / 4 ])
                    squarish_rpoly(xy =
                                       [
                                           length + cover_overhang * 2 - plate_tolerance / 2,
                                           width + cover_overhang * 2 - plate_tolerance / 2
                                       ],
                                   h = cover_thickness / 2, r = corner_radius, center = true);
            }
            // Generate the main body of the switch
            difference()
            {
                translate([ 0, 0, cover_thickness ]) hull()
                {
                    translate([ 0, 0, plate_thickness / 2 ])
                        squarish_rpoly(xy = [ length - plate_tolerance / 2, width - plate_tolerance / 2 ],
                                       h = plate_thickness, r = corner_radius, center = true);
                    translate([ 0, 0, body_height - cover_thickness ])
                        squarish_rpoly(xy = [ length / taper, width / taper ],
                                       h = cover_thickness / 2, // Close enough
                                       r = corner_radius, center = true);
                }

                difference()
                {
                    // NOTE: Because of the body taper we need to increase the wall thickness here slightly so that it
                    // prints well (don't want the slicer cutting off the corners).  That's why we have this "interior
                    // bulge" thing...
                    translate([ 0, 0, -cover_thickness / 2 ]) hull()
                    {
                        // NOTE: The interior taper here is to give the wall-to-cover attachment a stronger bond
                        // (otherwise the top of the switch can break right off leaving you with a top and a hollow
                        // cube)
                        squarish_rpoly(xy = [ length - wall_thickness * 3.75, width - wall_thickness * 3.75 ],
                                       h = cover_thickness / 2, r = corner_radius / 1.5, center = true);
                        translate([ 0, 0, body_height / 4 ])
                            squarish_rpoly(xy = [ length - wall_thickness * 2, width - wall_thickness * 2 ],
                                           h = cover_thickness / 2, r = corner_radius / 1.5, center = true);
                        translate([ 0, 0, cover_thickness + height + wall_thickness / 2 ]) squarish_rpoly(
                            xy = [ length / taper - wall_thickness * 1.25, width / taper - wall_thickness * 1.25 ],
                            h = cover_thickness, r = corner_radius / 1.5, center = true);
                    }
                }
                // Create a gap underneath the clips so they can move freely
                gap_size = plate_thickness / 2;
                translate([ clip_width, 0, cover_thickness / 2 + gap_size / 2 ])
                    cube([ clip_width + clip_side_hole_width * 2, width * 2, gap_size ], center = true);
                translate([ -clip_width, width / 2, cover_thickness / 2 + gap_size / 2 ])
                    cube([ clip_width + clip_side_hole_width * 2, width / 2, gap_size ], center = true);
                // Cutout holes beside the clips so they can bend (slightly) easier when inserting
                translate([
                    clip_width + clip_width / 2 + clip_side_hole_width / 2, 0,
                    cover_thickness / 2 + travel / 2 + plate_thickness / 2 +
                    bridge_thickness
                ]) cube([ clip_side_hole_width, width * 2, travel ], center = true);
                translate([
                    clip_width - clip_width / 2 - clip_side_hole_width / 2, 0,
                    cover_thickness / 2 + travel / 2 + plate_thickness / 2 +
                    bridge_thickness
                ]) cube([ clip_side_hole_width, width * 2, travel ], center = true);
                translate([
                    -clip_width - clip_width / 2 - clip_side_hole_width / 2,
                    length / 2 + snap_clip_protrusion - cover_overhang / 1.15,
                    cover_thickness / 2 + travel / 2 + plate_thickness / 2 +
                    bridge_thickness
                ]) cube([ clip_side_hole_width, width / 2, travel ], center = true);
                translate([
                    -clip_width + clip_width / 2 + clip_side_hole_width / 2,
                    length / 2 + snap_clip_protrusion - cover_overhang / 1.15,
                    cover_thickness / 2 + travel / 2 + plate_thickness / 2 +
                    bridge_thickness
                ]) cube([ clip_side_hole_width, width / 2, travel ], center = true);
                // Carve out a bit of the bridge that supports the clips so it can break away easier upon first
                // insertion of the switch into the plate
                translate([ // Back side left
                    -clip_width,
                    length/2-wall_thickness/1.55,
                    cover_thickness/2+plate_thickness/2+bridge_thickness/2])
                        cube([
                            clip_width+clip_side_hole_width*2,
                            wall_thickness/3.5,
                            bridge_thickness+0.01], center=true);
                translate([ // Back side right
                    clip_width,
                    length/2-wall_thickness/1.55,
                    cover_thickness/2+plate_thickness/2+bridge_thickness/2])
                        cube([
                            clip_width+clip_side_hole_width*2,
                            wall_thickness/3.5,
                            bridge_thickness+0.01], center=true);
                translate([ // Front side right
                    clip_width,
                    -length/2+wall_thickness/1.55,
                    cover_thickness/2+plate_thickness/2+bridge_thickness/2])
                        cube([
                            clip_width+clip_side_hole_width*2,
                            wall_thickness/3.5,
                            bridge_thickness+0.01], center=true);
            }
        }
    }
    // Add little snap/clip nubs so the switch can "snap" into place (and stay there)
    if (cover_overhang)
    { // Don't need the clips if doing flush mount
        translate([
            clip_width, length / 2 + snap_clip_protrusion - cover_overhang / 1.15,
            cover_thickness + snap_clip_thickness / 1.25 +
            plate_thickness
        ]) rotate([ -60, 0, 0 ]) cube([ clip_width, snap_clip_thickness * 2, snap_clip_thickness ], center = true);
        translate([
            -clip_width, length / 2 + snap_clip_protrusion - cover_overhang / 1.15,
            cover_thickness + snap_clip_thickness / 1.25 +
            plate_thickness
        ]) rotate([ -60, 0, 0 ]) cube([ clip_width, snap_clip_thickness * 2, snap_clip_thickness ], center = true);
        translate([
            clip_width, -length / 2 - snap_clip_protrusion + cover_overhang / 1.15,
            cover_thickness + snap_clip_thickness / 1.25 +
            plate_thickness
        ]) rotate([ 60, 0, 0 ]) cube([ clip_width, snap_clip_thickness * 2, snap_clip_thickness ], center = true);
        // Same snaps/clips but other sides (probably unnecessary but leaving this here "just in case" someone wants the
        // switch body to have a death grip on the plate):
        //        translate([
        //            width/2+snap_clip_protrusion-cover_overhang/1.15,
        //            0,
        //            cover_thickness+snap_clip_thickness/2+plate_thickness/1.15])
        //                rotate([0,45,0])
        //                    cube([snap_clip_thickness, width/3, snap_clip_thickness], center=true);
        //        translate([
        //            -BODY_WIDTH/2-snap_clip_protrusion+cover_overhang/1.25,
        //            0,
        //            cover_thickness+snap_clip_thickness/2+plate_thickness/1.15])
        //                rotate([0,45,0])
        //                    cube([snap_clip_thickness, width/3, snap_clip_thickness], center=true);
    }
}

switch_body(BODY_LENGTH, BODY_WIDTH, TOTAL_TRAVEL, cover_overhang = COVER_OVERHANG, taper = BODY_TAPER,
            wall_thickness = WALL_THICKNESS, cover_thickness = COVER_THICKNESS,
            plate_thickness = SWITCH_PLATE_THICKNESS, plate_tolerance = SWITCH_PLATE_TOLERANCE,
            corner_radius = BODY_CORNER_RADIUS, bridge_thickness = CLIP_BRIDGE_THICKNESS, height = BODY_HEIGHT);