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

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <jpeglib.h>
#include <libexif/exif-data.h>
#include <libexif/exif-utils.h>
}

#include "jp4.h"

static long get_long(unsigned char* d) {
  return ((d[0] & 0xff) << 24) | ((d[1] & 0xff) << 16) | ((d[2] & 0xff) << 8) | (d[3] & 0xff);

}

JP4::JP4() {}

JP4::~JP4() {
  if (_raw_app1) delete[] _raw_app1;
  if (_ed) exif_data_free(_ed);
}

const string& JP4::filename() const {
  return _filename;
}

unsigned int JP4::width() const {
  return _image->xsize();
}

unsigned int JP4::height() const {
  return _image->ysize();
}

unsigned short* JP4::data() const {
  return _image->data();
}

ImageL16i* JP4::image() const {
  return _image;
}

Bayer* JP4::bayer() {
  return &_bayer;
}

const ElphelMakerNote& JP4::makerNote() const {
  return _makerNote;
}

unsigned int JP4::makerNoteLength() const {
  return _makerNoteLength;
}

bool JP4::linear() const {
  return _linear;
}

JP4* JP4::crop(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {

  JP4* cropped = new JP4();
  cropped->_filename  = _filename;
  cropped->_makerNote = _makerNote;
  cropped->_makerNoteLength = _makerNoteLength;
  cropped->_linear = _linear;
  cropped->_raw_app1 = _raw_app1;
  cropped->_raw_app1_length = _raw_app1_length;
  cropped->_ed = _ed;
  cropped->_bayer = _bayer;

  cropped->_image = _image->crop(x, y, width, height);
  return cropped;

}

bool JP4::open(const string& _filename) {

  this->_filename = string(_filename);

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct dinfo;

  JSAMPARRAY buffer;

  FILE *ifp = NULL;

  dinfo.err = jpeg_std_error (&jerr);

  ifp = fopen(filename().c_str(), "rb");

	if (!ifp)
		return false;

  jpeg_create_decompress (&dinfo);
  jpeg_stdio_src (&dinfo, ifp);
  // instruct it to save EXIF at APP1 (0xe1) data (up to 64k)
  jpeg_save_markers(&dinfo, 0xe1, 0xffff);
  jpeg_read_header (&dinfo, TRUE);

  dinfo.do_block_smoothing = FALSE;
  dinfo.out_color_space = JCS_GRAYSCALE;

  // save raw APP1 data (if any)
  if (dinfo.marker_list) {
    _raw_app1_length = dinfo.marker_list[0].data_length;
    _raw_app1 = new unsigned char[_raw_app1_length];
    memcpy(_raw_app1, dinfo.marker_list[0].data, _raw_app1_length);
  }

  _image = new ImageL16i(dinfo.image_width, dinfo.image_height);

  unsigned short* temp = new unsigned short[width()*height()];
  buffer = (*dinfo.mem->alloc_sarray)((j_common_ptr)&dinfo, JPOOL_IMAGE, width(), 1);

  jpeg_start_decompress (&dinfo);

  for (unsigned int line = 0; line < height(); line++) {
    jpeg_read_scanlines (&dinfo, buffer, 1);
    for (unsigned int column = 0; column < width(); column++)
      temp[line*width() + column] = buffer[0][column];
  }

  // EXIF
  readMakerNote();

  // JP4 deblocking
  // from http://code.google.com/p/gst-plugins-elphel/source/browse/trunk/jp462bayer/src/gstjp462bayer.c
  unsigned int y, x;
  unsigned int b_of = 0;
  unsigned int h_of;
  unsigned int i, j;
  unsigned int index1[16]={0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15};
  unsigned int  index2[16];

  for (j = 0;j < 16; ++j)
    index2[j] = index1[j] * width();

  for (y = 0; y < height(); y += 16, b_of += width() << 4)
    for (x = 0; x < width(); x += 16)
      for (j = 0, h_of = 0; j < 16; ++j, h_of += width())
        for (i = 0; i < 16; ++i)
          (*_image)[x + i + h_of + b_of] = temp[x + index1[i] + index2[j] + b_of];

  jpeg_finish_decompress (&dinfo);
  jpeg_destroy_decompress (&dinfo);
  fclose(ifp);
  delete[] temp;
	return true;
}

void JP4::readMakerNote() {

  if (_raw_app1)
    _ed = exif_data_new_from_data(_raw_app1, _raw_app1_length);

  if (!_ed)
    return;

  ExifEntry* makerNoteEntry = exif_data_get_entry(_ed, EXIF_TAG_MAKER_NOTE);

  if (!makerNoteEntry) {
    _makerNote.gain[0]  = _makerNote.gain[1]  = _makerNote.gain[2]  = _makerNote.gain[3] = 2.0;
    _makerNote.gamma[0] = _makerNote.gamma[1] = _makerNote.gamma[2] = _makerNote.gamma[3] = 0.57;
    _makerNote.gamma_scale[0] = _makerNote.gamma_scale[1] = _makerNote.gamma_scale[2] = _makerNote.gamma_scale[3] = 0xffff;

    _makerNote.black[0] = _makerNote.black[1] = _makerNote.black[2] = _makerNote.black[3] = 10/256.0;
    _makerNote.decim_hor = 1;
    _makerNote.decim_ver = 1;
    _makerNote.bin_hor = 1;
    _makerNote.bin_ver = 1;
    _makerNote.composite = 0;
   return;
  }

  _makerNoteLength = makerNoteEntry->size/4;
  long makerNote[_makerNoteLength];

  for (int i = 0; i < _makerNoteLength; i++) {
    makerNote[i] = get_long(makerNoteEntry->data + i*4);
  }

  for (int i = 0; i < 4; i++) {
    // channel gain  8.16 (0x10000 - 1.0). Combines both analog gain and digital scaling
    _makerNote.gain[i] = makerNote[i] / 65536.0;  
 
    // (P_PIXEL_LOW<<24) | (P_GAMMA <<16) and 16-bit (6.10) scale for gamma tables, 
    _makerNote.gamma_scale[i] = (makerNote[i+4] & 0xffff);
    _makerNote.gamma[i]       = ((makerNote[i+4]>>16) & 0xff) / 100.0;
    _makerNote.black[i]       =  (makerNote[i+4]>>24) / 256.0;
  }

  if (_makerNoteLength >= 12) {
    _makerNote.woi_left   = (makerNote[8] & 0xffff);
    _makerNote.woi_width  = (makerNote[8]>>16) & 0xffff;
    _makerNote.woi_top    = (makerNote[9] & 0xffff);
    _makerNote.woi_height = (makerNote[9]>>16);

    _makerNote.flip_hor = (makerNote[10] & 0x1);
    _makerNote.flip_ver = (makerNote[10]>>1) & 0x1;

    _makerNote.bayer_mode = (makerNote[10]>>2) & 0x3;
    _makerNote.color_mode = (makerNote[10]>>4) & 0x0f;

    _makerNote.decim_hor = (makerNote[10]>> 8) & 0x0f;
    _makerNote.decim_ver = (makerNote[10]>>12) & 0x0f;

    _makerNote.bin_hor = (makerNote[10]>>16) & 0x0f;
    _makerNote.bin_ver = (makerNote[10]>>20) & 0x0f;
  }

  _makerNote.composite = ((makerNote[10] & 0xc0000000)!=0);

  if (_makerNoteLength >= 14) {
    if (_makerNote.composite) {
      _makerNote.height1 = makerNote[11] & 0xffff;
      _makerNote.blank1  =(makerNote[11]>>16) & 0xffff;
      _makerNote.height2 = makerNote[12] & 0xffff;
      _makerNote.blank2  =(makerNote[12]>>16) & 0xffff;
      _makerNote.height3 =(makerNote[9]>>16) - _makerNote.height1-_makerNote.blank1-_makerNote.height2-_makerNote.blank2;

      _makerNote.flip_h1 = (((makerNote[10] >> 24) & 1)!=0);
      _makerNote.flip_v1 = (((makerNote[10] >> 25) & 1)!=0);
      _makerNote.flip_h2 = (((makerNote[10] >> 26) & 1)!=0);
      _makerNote.flip_v2 = (((makerNote[10] >> 27) & 1)!=0);
      _makerNote.flip_h3 = (((makerNote[10] >> 28) & 1)!=0);
      _makerNote.flip_v3 = (((makerNote[10] >> 29) & 1)!=0);

      _makerNote.portrait = (((makerNote[13] >>  7) & 1)!=0);
    }
  }

}

ExifData* JP4::exifData() const {
  return _ed;
}

bool JP4::hasTag(ExifTag tag) const {
  return exif_data_get_entry(_ed, tag) != NULL;
}

unsigned int JP4::getTagUInt(ExifTag tag) const {
  ExifEntry* e = exif_data_get_entry(_ed, tag);
  return exif_get_long(e->data, exif_data_get_byte_order(_ed));
}

string JP4::getTagString(ExifTag tag) const {
  ExifEntry* e = exif_data_get_entry(_ed, tag);
  char value[e->size];
  exif_entry_get_value(e, value, e->size);
  return string(value);
}

void JP4::getTagURational(ExifTag tag, unsigned int* n, unsigned int* d) const {
  ExifEntry* e = exif_data_get_entry(_ed, tag);
  ExifRational r = exif_get_rational(e->data, exif_data_get_byte_order(_ed));
  if (n) *n = r.numerator;
  if (d) *d = r.denominator;
}

void JP4::getTagSRational(ExifTag tag, int* n, int* d) const {
  ExifEntry* e = exif_data_get_entry(_ed, tag);
  ExifSRational r = exif_get_srational(e->data, exif_data_get_byte_order(_ed));
  if (n) *n = r.numerator;
  if (d) *d = r.denominator;
}

ExifEntry* JP4::getTagRaw(ExifTag tag) const {
  return exif_data_get_entry(_ed, tag);
}
