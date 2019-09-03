#include "BMPFile.h"

#define RLE

struct RGBQUAD {
  TUint8 blue, green, red, reserved;
};

// This struct assumes x86 (little endian)
struct BMPHEADER {
  // struct BMPFILEHDR {
  TUint8  bfType1;      // type of file (must be BM)
  TUint8  bfType2;      // type of file (must be BM)
  TUint32 bfSize;      // size of file in bytes (lo word)
  TUint16 bfReserved1; // set to zero
  TUint16 bfReserved2; // set to zero
  TUint32
          bfOffBits;   // offset from BITMAPFILEHEADER struct to actual bitmap data
  TInt32  biSize;   // sizeof(BITMAPINFOHEADER) = 40
  TInt32  biWidth;  // width of bitmap in pixels
  TInt32  biHeight; // height of bitmap in pixels
  TInt16  biPlanes; // number of bitplanes (must be 1)
  TInt16  biBitCount;      // bits per pixel (1, 4, 8, 16, or 24)
  TInt32  biCompression;   // type of compression 0 = none
  TInt32  biSizeImage;     // size of image, in bytes.  0 if no compression
  TInt32  biXPelsPerMeter; // pixels per meter (0)
  TInt32  biYPelsPerMeter; // pixels per meter (0)
  TInt32  biClrUsed;       // number of color entries in color table
  TInt32  biClrImportant;  // number of colors that are used in the color table

  RGBQUAD palette[256];

  TUint8 bits[4]; // WARNING!!! SUBTRACT 4 FROM SIZE
} __attribute__((packed));

BMPFile::BMPFile(const char *filename) {
  //
  char work[2048];
  getcwd(work, 2048);
  int fd = open(filename, O_RDONLY | O_BINARY);
  if (fd < 0) {
    abort("*** BMPFile: could not open %s (%s)\n", filename, work);
  }
  off_t size = lseek(fd, 0, 2);
  lseek(fd, 0, 0);
  rawFile = new TUint8[size];
  if (read(fd, rawFile, size) != size) {
    abort("read error\n");
  }
  close(fd);
  bmp = (BMPHEADER *) rawFile;
  if (bmp->bfType1 != 'B' || bmp->bfType2 != 'M') {
    abort("Expected BM at start of header\n");
  }
  if (bmp->biCompression != 0) {
    abort("Compressed BMP not supported (%d)\n", bmp->biCompression);
  }
  const int num_colors = bmp->biClrImportant ? bmp->biClrImportant : 256;
  this->filename     = strdup(filename);
  this->width        = bmp->biWidth;
  this->height       = bmp->biHeight;
  this->depth        = bmp->biBitCount;
  this->bytesPerRow  = this->width;
  this->palette_size = num_colors;
  this->palette      = new uint8_t[num_colors * 3];
  // copy palette
  for (int i     = 0; i < num_colors; i++) {
    this->palette[i * 3 + 0] = bmp->palette[i].red;
    this->palette[i * 3 + 1] = bmp->palette[i].green;
    this->palette[i * 3 + 2] = bmp->palette[i].blue;
  }
  // copy pixels
  uint8_t  *src  = &rawFile[bmp->bfOffBits];
  int      pitch = (bmp->biWidth + 3) & ~0x03; // round up to 4 byte boundary
  this->pixels = new uint8_t[this->width * this->height];
  uint8_t  *dst = this->pixels;
  for (int y    = 0; y < this->height; y++) {
    for (int x = 0; x < this->width; x++) {
      //        dst[y*width +resources] = src[y * pitch + resources];
      dst[(this->height - y - 1) * this->width + x] = src[y * pitch + x];
    }
  }
#ifdef VERBOSE
  printf("%u bits per pixel\n", bmp->biBitCount);
  printf("%d size\n", bmp->bfSize);
  printf("%dx%d pixels\n", bmp->biWidth, bmp->biHeight);
  printf("offset to pixels: %d\n", bmp->bfOffBits);
  printf("color table entries: %d\n", bmp->biClrUsed);
  printf("color table entries used: %d\n", bmp->biClrImportant);
#endif
}

BMPFile::~BMPFile() {
  //
}

static TBool once = EFalse;

void BMPFile::Write(ResourceFile &resourceFile) {
  // copy the bitmap values into output
  resourceFile.Write(&width, sizeof(width));
  resourceFile.Write(&height, sizeof(height));
  resourceFile.Write(&depth, sizeof(depth));
  resourceFile.Write(&bytesPerRow, sizeof(bytesPerRow));
  resourceFile.Write(&palette_size, sizeof(palette_size));
  resourceFile.Write(palette, 3 * palette_size);
#ifdef RLE
  TInt length = BytesInBitmap(), sum = length;
  TInt size = 0;
  TUint8 *src = pixels, *end = &src[length];

  while (src < end) {
    TUint8 *ptr = src;
    TUint8 byte = *src++;
    TUint8 rle  = 1;

    while (src < end && *src == byte && rle < 127) {
      src++;
      rle++;
    }
    if (rle > 1) {
      sum -= rle;
      rle--;
      size += 2;
      resourceFile.Write(&rle, sizeof(rle));
      resourceFile.Write(&byte, sizeof(byte));
    } else {
      while (src < end && *src != byte && src[1] != *src && rle < 127) {
        byte = *src++;
        rle++;
      }
      sum -= rle;
      size += rle;
      size += 1;
      TUint8 count = rle;
      rle--;
      rle |= 0x80;
      resourceFile.Write(&rle, sizeof(rle));
      resourceFile.Write(ptr, count);
    }
  }
  once = ETrue;
  printf("  RLE Compressed %d -> %ld\n", BytesInBitmap(), size);


#if 0

  long    size = 0, runs = 0;
  uint8_t *src = pixels, *end = &pixels[BytesInBitmap()];
  uint8_t *ptr = src;
  while (ptr < end) {
    int     count = 0;
    uint8_t byte  = *ptr++;
    if (*ptr == byte) {
      while (ptr < end && *ptr == byte && count < 127) {
        count++;
        ptr++;
      }
      runs += count;
      uint8_t rle = (count & 0x7f) - 1;
      resourceFile.Write(&rle, sizeof(rle));
      resourceFile.Write(&byte, sizeof(byte));
      size += 2;
    }
    else {
      uint8_t *seek = ptr;
      while (seek < end && *seek != byte && count < 127) {
        byte = *seek++;
        count++;
      }
      runs += count;

      uint8_t rle = 0x80 | ((count & 0x7f) - 1);
      resourceFile.Write(&rle, sizeof(rle));
      resourceFile.Write(ptr, seek - ptr);
      size += seek - ptr + 1;
      ptr         = seek;
    }
  }

  once = ETrue;
#endif
#else
  resourceFile.Write(pixels, BytesInBitmap());
#endif
}
