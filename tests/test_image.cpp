#include <libjp4/image.h>
#include <libjp4/clock.h>

extern "C" {
#include <stdio.h>
}

void PGM(const char* output, const ImageL8i* img) {

  FILE* pgm = fopen(output, "w");

  fprintf(pgm, "P2\n%hu %hu\n%hhu\n", img->xsize(), img->ysize(), 0xff);
  for (uint16_t i = 0; i < img->ysize(); i++) {
    for (uint16_t j = 0; j < img->xsize(); j++) {
      fprintf(pgm, "%hhu ", img->operator()(i,j));
    }   
    fprintf(pgm, "\n");
  }

  fclose(pgm);
  
}

ImageL8i* PGM(const char* input) {

  FILE* f = fopen(input, "r");
  uint16_t w = 0;
  uint16_t h = 0;
  uint8_t max = 0;

  fscanf(f, "P2");
  fscanf(f, "%hu %hu", &w, &h);
  fscanf(f, "%hhu", &max);

  ImageL8i* img = new ImageL8i(w,h);

  for (uint16_t i=0; i < h; i++) {
    for (uint16_t j=0; j < w; j++) {
      fscanf(f, "%hhu", &img->operator()(i,j));
    }
  }

  return img;

}

void flipX() {
  ImageL8i* img = PGM("test_image.pgm.template");
  uint64_t t0 = currentTimeMillis();
  img->flipX();
  uint64_t t1 = currentTimeMillis();
  PGM("__autogen__flipX.pgm", img);
  printf("flipX %llu\n", t1-t0);
  delete img;
}

void flipY() {
  ImageL8i* img = PGM("test_image.pgm.template");
  uint64_t t0 = currentTimeMillis();
  img->flipY();
  uint64_t t1 = currentTimeMillis();
  PGM("__autogen__flipY.pgm", img);
  printf("flipY: %llu\n", t1-t0);
  delete img;
}

void flipXY() {
  ImageL8i* img = PGM("test_image.pgm.template");
  uint64_t t0 = currentTimeMillis();
  img->flipXY();
  uint64_t t1 = currentTimeMillis();
  PGM("__autogen__flipXY.pgm", img);
  printf("flipXY: %llu\n", t1-t0);
  delete img;
}

void rotate90() {
  ImageL8i* img = PGM("test_image.pgm.template");
  uint64_t t0 = currentTimeMillis();
  img->rotate(ROTATE_90);
  uint64_t t1 = currentTimeMillis();
  PGM("__autogen__rotate90.pgm", img);
  printf("rotate90: %llu\n", t1-t0);
  delete img;
}

void rotate180() {
  ImageL8i* img = PGM("test_image.pgm.template");
  uint64_t t0 = currentTimeMillis();
  img->rotate(ROTATE_180);
  uint64_t t1 = currentTimeMillis();
  PGM("__autogen__rotate180.pgm", img);
  printf("rotate180: %llu\n", t1-t0);
  delete img;
}

void rotate270() {
  ImageL8i* img = PGM("test_image.pgm.template");
  uint64_t t0 = currentTimeMillis();
  img->rotate(ROTATE_270);
  uint64_t t1 = currentTimeMillis();
  PGM("__autogen__rotate270.pgm", img);
  printf("rotate270: %llu\n", t1-t0);
  delete img;
}

void crop() {
  ImageL8i* img = PGM("test_image.pgm.template");
  uint64_t t0 = currentTimeMillis();
  ImageL8i* crop = img->crop(256, 256, 1024, 1024);
  uint64_t t1 = currentTimeMillis();
  PGM("__autogen__crop-1024x1024+256+256.pgm", crop);
  printf("crop: %llu\n", t1-t0);
  delete img;
	delete crop;
}

int main() {

  flipX(); flipY(); flipXY();
  rotate90(); rotate180(); rotate270();
  crop();

  return 0;
};
