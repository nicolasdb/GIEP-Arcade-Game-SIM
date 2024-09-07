/*  GIMP header image file format (RGB): /home/nicolas/github/GIEP-Arcade-Game-SIM/include/GIEP4x14.h  */

static unsigned int width = 4;
static unsigned int height = 14;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data,pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}
static char *header_data =
	"!0]!!0]!!0]!!0]!!/-!!/-!!/-!!/-!!-=!!-=!!-=!!-=!!+M!!+M!!+M!!+M!"
	"`Q!!`Q!!`Q!!`Q!!!$)!!$)!!$)!!$)!!&=!!&=!!&=!!&=!!(-!!(-!!(-!!(-!"
	"!)]!!)]!!)]!!)]!!!$D!!$D!!$D!!$D!!$(!!$(!!$(!!$(!!#L!!#L!!#L!!#L"
	"RQ!!RQ!!KQ!!KQ!!!0``!0``!0``!0``";
