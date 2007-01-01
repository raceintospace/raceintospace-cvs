struct pcx_hdr {
  unsigned char manufacturer;
  unsigned char version;
  unsigned char encoding;
  unsigned char bits_per_pixel;
  ui16 xmin,ymin,xmax,ymax,hres,vres;
  unsigned char palette[48];
  unsigned char reserved;
  unsigned char color_planes;
  ui16 bytes_per_line;
  ui16 palette_type;
  unsigned char filler[58];
};
