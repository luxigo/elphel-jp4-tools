/*
  Copyright 2010 Paulo Henrique Silva <ph.silva@gmail.com>

  This file is part of jp4-tools.

  jp4-tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  jp4-tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with jp4-tools.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <libjp4/jp4.h>

#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s image.jp4\n", argv[0]);
    exit(1);
  }

  JP4 jp4;
  if (!jp4.open(argv[1])) {
		fprintf(stderr, "Cannot open %s.\n", argv[1]);
		exit(1);
	}

  const ElphelMakerNote& elphel = jp4.makerNote();
  unsigned int makerNoteLength = jp4.makerNoteLength();

	if (makerNoteLength == 0) {
		fprintf(stderr, "%s does not have Elphel MakerNote entry.\n", argv[1]);
		exit(1);
	}

  printf("gain       : %5.4lf\t%5.4lf\t%5.4lf\t%5.4lf\n", elphel.gain[0],
         elphel.gain[1], elphel.gain[2], elphel.gain[3]);
  printf("gamma      : %5.4lf\t%5.4lf\t%5.4lf\t%5.4lf\n", elphel.gamma[0],
         elphel.gamma[1], elphel.gamma[2], elphel.gamma[3]);
  printf("gamma_scale: %5.4lf\t%5.4lf\t%5.4lf\t%5.4lf\n",
         elphel.gamma_scale[0], elphel.gamma_scale[1],
         elphel.gamma_scale[2], elphel.gamma_scale[3]);
  printf("black      : %5.4lf\t%5.4lf\t%5.4lf\t%5.4lf\n", elphel.black[0],
         elphel.black[1], elphel.black[2], elphel.black[3]);

  if (makerNoteLength >= 12) {
    printf("woi_left: %d\n", elphel.woi_left);
    printf("woi_width: %d\n", elphel.woi_width);
    printf("woi_top: %d\n", elphel.woi_top);
    printf("woi_height: %d\n", elphel.woi_height);
    printf("flip_hor: %d\n", elphel.flip_hor);
    printf("flip_ver: %d\n", elphel.flip_ver);
    printf("bayer_mode: %d\n", elphel.bayer_mode);
    printf("color_mode: %d\n", elphel.color_mode);
    printf("decim_hor: %d\n", elphel.decim_hor);
    printf("decim_ver: %d\n", elphel.decim_ver);
    printf("bin_hor: %d\n", elphel.bin_hor);
    printf("bin_ver: %d\n", elphel.bin_ver);
  }

  printf("orientation: %s\n", elphel.portrait? "PORTRAIT": "LANDSCAPE");


  if (makerNoteLength >= 14) {
    printf("composite: %d\n", elphel.composite);

    printf("height1  : %d\n", elphel.height1);
    printf("blank1   : %d\n", elphel.blank1);
    printf("flip_h1  : %d\n", elphel.flip_h1);
    printf("flip_v1  : %d\n", elphel.flip_v1);

    printf("height2  : %d\n", elphel.height2);
    printf("blank2   : %d\n", elphel.blank2);
    printf("flip_h2  : %d\n", elphel.flip_h2);
    printf("flip_v2  : %d\n", elphel.flip_v2);

    printf("height3  : %d\n", elphel.height3);
    printf("flip_h3  : %d\n", elphel.flip_h3);
    printf("flip_v3  : %d\n", elphel.flip_v3);
  }

  return 0;
}

