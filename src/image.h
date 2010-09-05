#ifndef IMAGE_H
#define IMAGE_H 1

// TODO: better memory management

#include <stdint.h>

template <typename T>
struct PixelRGB {

  static const uint8_t _n_channel = 3;

  union {
    T _channel[_n_channel];
    struct {
      T _R;
      T _G;
      T _B;
    };
  };

  PixelRGB() { _R = _G = _B = T(); }
  PixelRGB(T iR, T iG, T iB) { _R = iR; _G = iG; _B = iB; }
  PixelRGB(const PixelRGB<T>& i) { _R = i._R; _G = i._G; _B = i._B; }

  T& R() { return _R; }
  T& G() { return _G; }
  T& B() { return _B; }

  const T& R() const { return _R; }
  const T& G() const { return _G; }
  const T& B() const { return _B; }

  T& R(T R) { _R = R; return *this; }
  T& G(T G) { _G = G; return *this; }
  T& B(T B) { _B = B; return *this; }

  T& operator[](uint8_t i) { return _channel[i]; }

  PixelRGB<T>& operator=(const PixelRGB<T>& i) { _R = i.R(); _G = i.G(); _B = i.B(); return *this; } 

};

template <typename T>
class Image {
public:

  Image(uint16_t ysize, uint16_t xsize) {
    _data = new T[ysize*xsize];
    _ysize = ysize;
    _xsize = xsize;
    _yorigin = 0;
    _xorigin = 0;
    _ystep = xsize;
    _xstep = 1; 
  }

  //virtual ~Image() { delete[] _data; }

  uint16_t xsize() const { return _xsize; }
  uint16_t ysize() const { return _ysize; }

  T* data() const { return _data; }

  Image<T> block(uint16_t y, uint16_t x) {
    return Image<T>(*this, y, x);
  }

  // raw access
  T& operator[](int32_t i) const { return _data[i]; }

  // smart access
  T& operator()(int16_t y) const { return _data[ (_yorigin+y*_ystep) ]; }
  T& operator()(int16_t y, uint16_t x) const { return _data[ (_yorigin+y*_ystep)+(_xorigin+x*_xstep) ]; }

  void flipX() {
    _xstep   = -_xstep;
    _xorigin = _xorigin? 0: _xsize-1;
  }

  void flipY() {
    _ystep   = -_ystep;
    _yorigin = _yorigin? 0: (_ysize-1)*_xsize;
  }

  void flipXY() { flipX(); flipY(); }

  void flipNone() { _xorigin = _yorigin = 0; _ystep = _xsize; _xstep = 1; }

protected:

  Image(const Image& img, uint16_t yoffset, uint16_t xoffset) {
    _data  = img._data;
    _ysize = img._ysize;
    _xsize = img._xsize;
    _ystep = img._ystep;
    _xstep = img._xstep;
    _yorigin = img._yorigin + yoffset*img._ystep;
    _xorigin = img._xorigin + xoffset*img._xstep;
  }

private:
  T* _data;

  uint16_t _ysize;
  uint16_t _xsize;

  uint16_t _yorigin;
  uint16_t _xorigin;

  int16_t _ystep;
  int16_t _xstep;

};

typedef PixelRGB<uint8_t>  PixelRGB8i;
typedef PixelRGB<uint16_t> PixelRGB16i;
typedef PixelRGB<uint32_t> PixelRGB32i;
typedef PixelRGB<float>  PixelRGB32f;
typedef PixelRGB<double> PixelRGB64f;

typedef Image<PixelRGB8i>  ImageRGB8i;
typedef Image<PixelRGB16i> ImageRGB16i;
typedef Image<PixelRGB32i> ImageRGB32i;
typedef Image<PixelRGB32f> ImageRGB32f;
typedef Image<PixelRGB64f> ImageRGB64f;

typedef Image<uint8_t>  ImageL8i;
typedef Image<uint16_t> ImageL16i;
typedef Image<uint32_t> ImageL32i;
typedef Image<float>    ImageL32f;
typedef Image<double>   ImageL64f;

#endif
