import sys
import os
import re

def read_xpm_file(file_path):
    """
    Reads the content of an XPM file.
    
    Args:
    file_path (str): The path to the XPM file.
    
    Returns:
    str: The content of the XPM file.
    """
    with open(file_path, 'r') as file:
        content = file.read()
    return content

def parse_xpm(content):
    """
    Parses the XPM content to extract dimensions, colors, and pixel data.
    
    Args:
    content (str): The content of the XPM file.
    
    Returns:
    tuple: (width, height, color_dict, pixel_data)
    """
    lines = content.strip().split('\n')
    
    # Find the line with dimensions
    for i, line in enumerate(lines):
        if re.match(r'"\d+\s+\d+\s+\d+\s+\d+"', line):
            header = re.findall(r'\d+', line)
            width, height, num_colors, chars_per_pixel = map(int, header)
            break
    else:
        raise ValueError("Could not find dimension information in XPM file")

    # Extract color definitions
    color_dict = {}
    for line in lines[i+1:i+1+num_colors]:
        parts = line.strip('",').split(None, 1)
        char, color_def = parts[0], parts[1]
        if color_def.startswith('c '):
            color_def = color_def[2:]  # Remove the 'c ' prefix
        color_dict[char] = color_def.strip()

    # Extract pixel data
    pixel_data = ''.join([line.strip('",') for line in lines[i+1+num_colors:] if line.strip().startswith('"')])
    
    # Ensure we only have the exact number of pixels
    pixel_data = pixel_data[:width * height]

    return width, height, color_dict, pixel_data

def xpm_to_bitmap(color_dict, pixel_data, width, height):
    """
    Converts XPM pixel data to a bitmap array.
    
    Args:
    color_dict (dict): Dictionary mapping characters to color values.
    pixel_data (str): String representing all pixels.
    width (int): Width of the image.
    height (int): Height of the image.
    
    Returns:
    list: Bitmap array of color values.
    """
    bitmap = []
    for char in pixel_data:
        color = color_dict.get(char, color_dict.get(' ', '#FFFFFF'))  # Default to white if not found
        if color.startswith('#'):
            color = '0x' + color[1:]  # Convert '#RRGGBB' to '0xRRGGBB'
        bitmap.append(color)
    
    return bitmap

def main():
    # Check if the correct number of arguments is provided
    if len(sys.argv) != 2:
        print("Usage: python xpm2array.py <input_file.xpm>")
        sys.exit(1)
    
    # Get the input file name from command line argument
    input_file = sys.argv[1]
    
    # Check if the specified file actually exists
    if not os.path.isfile(input_file):
        print(f"Error: File '{input_file}' does not exist.")
        sys.exit(1)
    
    print(f"Input file: {input_file}")
    print("File exists and is ready to be processed.")
    
    # Read the content of the XPM file
    xpm_content = read_xpm_file(input_file)
    
    # Parse the XPM content
    width, height, color_dict, pixel_data = parse_xpm(xpm_content)
    
    # Convert to bitmap array
    bitmap = xpm_to_bitmap(color_dict, pixel_data, width, height)
    
    # Print the bitmap array in C-style format
    print(f"\nconst uint32_t BITMAP[{width * height}] = {{")
    for i, color in enumerate(bitmap):
        if i % width == 0:
            print("    ", end="")
        print(f"{color}", end="")
        if i < len(bitmap) - 1:
            print(", ", end="")
        if (i + 1) % width == 0:
            print()
    print("};")
    
    # Print additional information
    print(f"\n// Image dimensions: {width}x{height}")
    print("// Color definitions:")
    for char, color in color_dict.items():
        print(f"//   '{char}': {color}")

if __name__ == "__main__":
    main()