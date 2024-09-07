# Creating C++ Array Bitmaps from GIMP

Using GIMP to draw the layout and then converting it to a bitmap for Arduino is a smart and efficient way to define the "Type" or behavior type on the matrix. It's much more intuitive than manually creating complex arrays.

## Steps to create a C++ array bitmap from GIMP:

1. Create your image in GIMP:
   - Set the image size to match your LED matrix dimensions (e.g., 4x14 or 8x8).
   - Use specific colors for each type of pixel (e.g., rain, button, sewer, basin).
   - Ensure you use solid colors without anti-aliasing to get clean, distinct pixels.

2. Export the image from GIMP:
   - Go to File -> Export As
   - Choose "C source code header (.h)" as the file type.
   - Save the file (e.g., as "GIEP4x14.h") in your project's include directory.

3. Use the `h2array.py` script to convert the .h file to a C++ array:
   - Open a terminal and navigate to your project directory.
   - Run the following command:
     ```
     python3 include/h2array.py include/GIEP4x14.h
     ```
   - This will output a C++ array representation of your image.

4. Copy the output and paste it into your Arduino code where you need to define the bitmap.

## Example

From a GIMP-exported .h file:

```c
/*  GIMP header image file format (RGB): /path/to/GIEP4x14.h  */

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
```

To a C++ array:

```cpp
const uint32_t BITMAP[56] = {
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 
    0x00E300, 0x00E300, 0x00E300, 0x00E300, 
    0x00C700, 0x00C700, 0x00C700, 0x00C700, 
    0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 
    0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 
    0x003200, 0x003200, 0x003200, 0x003200, 
    0x005700, 0x005700, 0x005700, 0x005700, 
    0x007300, 0x007300, 0x007300, 0x007300, 
    0x008F00, 0x008F00, 0x008F00, 0x008F00, 
    0x0000E3, 0x0000E3, 0x0000E3, 0x0000E3, 
    0x0000C7, 0x0000C7, 0x0000C7, 0x0000C7, 
    0x0000AB, 0x0000AB, 0x0000AB, 0x0000AB, 
    0xC70000, 0xC70000, 0xAB0000, 0xAB0000, 
    0x00FFFF, 0x00FFFF, 0x00FFFF, 0x00FFFF
};

// Image dimensions: 4x14
```

This process allows you to easily create and update bitmaps for your LED matrix directly from GIMP, making it much easier to design and modify your layouts.