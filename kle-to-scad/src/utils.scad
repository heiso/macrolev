SWITCH_HOLE_WIDTH = 14;
SWITCH_HOLE_TOLERANCE = 0.1;

PLATE_THICKNESS = 1.7;
PLATE_BORDER_WIDTH = 3;

SWITCH_HEIGHT_UNDER_PLATE = 5 - PLATE_THICKNESS;
SWITCH_PLACEHOLDER_WIDTH = 19.05;

STAB_2u_DISTANCE_BETWEEN_HOLES_CENTER = 23.8;
STAB_3u_DISTANCE_BETWEEN_HOLES_CENTER = 38.1;
STAB_5u_DISTANCE_BETWEEN_HOLES_CENTER = 76.2;
STAB_HOLE_WIDHT = 6.75;
STAB_HOLE_HEIGHT = 14;
STAB_HOLE_DISTANCE_TO_SWITCH_TOP = 1;

KEYCAP_WIDTH = 18.10;
KEYCAP_HEIGHT = 11.60;
KEYCAP_HEIGHT_ABOVE_PLATE = 6.80;

/* [Hidden] */

$fn = 32;

module hole(x = 0, y = 0, w = 0, h = 0, is_support = false) {
  real_x = x * SWITCH_PLACEHOLDER_WIDTH;
  real_y = y * SWITCH_PLACEHOLDER_WIDTH;
  real_w = w * SWITCH_PLACEHOLDER_WIDTH;
  real_h = h * SWITCH_PLACEHOLDER_WIDTH;

  center_x = real_x + real_w/2;
  center_y = real_y + real_h/2;
  translate([center_x, center_y, 0]) {
    if (is_support) {
      r = 1.5;
      // Switch under plate
      offset(r = r) offset(r = -r) square(SWITCH_PLACEHOLDER_WIDTH - 1.2, true);

      // Stab under plate
      offset(r = r) offset(r = -r) {
        // 2u, 2.25u, 2.75u stab
        if (w == 2 || w == 2.25 || w == 2.75) {
          square([STAB_HOLE_WIDHT + STAB_2u_DISTANCE_BETWEEN_HOLES_CENTER + 3, SWITCH_PLACEHOLDER_WIDTH - 1.2], true);
        }
        
        // 3u stab
        if (w == 3) {
          square([STAB_HOLE_WIDHT + STAB_3u_DISTANCE_BETWEEN_HOLES_CENTER + 3, SWITCH_PLACEHOLDER_WIDTH - 1.2], true);
        }

        // ISO enter key
        if (h == 2) {
          square([SWITCH_PLACEHOLDER_WIDTH - 1.2, STAB_HOLE_WIDHT + STAB_2u_DISTANCE_BETWEEN_HOLES_CENTER + 3], true);
        }
      }
    } else {
      // Switch plate hole
      square(SWITCH_HOLE_WIDTH + SWITCH_HOLE_TOLERANCE, true);

      // Stab plate hole
      stab_distance_to_top = y == 0 ? STAB_HOLE_DISTANCE_TO_SWITCH_TOP : -STAB_HOLE_DISTANCE_TO_SWITCH_TOP;
      translate([0, stab_distance_to_top, 0]) {
        // 2u, 2.25u, 2.75u stab
        if (w == 2 || w == 2.25 || w == 2.75) {
          translate([STAB_2u_DISTANCE_BETWEEN_HOLES_CENTER/2, 0, 0])  square([STAB_HOLE_WIDHT, STAB_HOLE_HEIGHT], true);
          translate([-STAB_2u_DISTANCE_BETWEEN_HOLES_CENTER/2, 0, 0])  square([STAB_HOLE_WIDHT, STAB_HOLE_HEIGHT], true);
        }
        
        // 3u stab
        if (w == 3) {
          translate([STAB_3u_DISTANCE_BETWEEN_HOLES_CENTER/2, 0, 0])  square([STAB_HOLE_WIDHT, STAB_HOLE_HEIGHT], true);
          translate([-STAB_3u_DISTANCE_BETWEEN_HOLES_CENTER/2, 0, 0])  square([STAB_HOLE_WIDHT, STAB_HOLE_HEIGHT], true);
        }
      } 

      // ISO enter key
      if (h == 2) {
        translate([-STAB_HOLE_DISTANCE_TO_SWITCH_TOP, 0, 0]) {
          translate([0, STAB_2u_DISTANCE_BETWEEN_HOLES_CENTER/2, 0])  square([STAB_HOLE_HEIGHT, STAB_HOLE_WIDHT], true);
          translate([0, -STAB_2u_DISTANCE_BETWEEN_HOLES_CENTER/2, 0])  square([STAB_HOLE_HEIGHT, STAB_HOLE_WIDHT], true);
        }
      }
    }
  }
}

module plate(x = 0, y = 0) {
  difference() {
    square([x * SWITCH_PLACEHOLDER_WIDTH, y * SWITCH_PLACEHOLDER_WIDTH]);
    children();
  }
}