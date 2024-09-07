import sys
import os
import re

def read_h_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    return content

def parse_h_file(content):
    # Extract width and height
    width = int(content.split('width = ')[1].split(';')[0])
    height = int(content.split('height = ')[1].split(';')[0])

    # Extract header_data
    header_data = re.search(r'static char \*header_data =\s*((?:"[^"]*"\s*)+);', content, re.DOTALL)
    if header_data:
        header_data = header_data.group(1).replace('"', '').replace('\n', '').replace('\t', '')
    else:
        raise ValueError("Could not find header_data in the file")

    return width, height, header_data

def decode_pixel(data):
    pixel = [0, 0, 0]
    pixel[0] = (((ord(data[0]) - 33) << 2) | ((ord(data[1]) - 33) >> 4))
    pixel[1] = ((((ord(data[1]) - 33) & 0xF) << 4) | ((ord(data[2]) - 33) >> 2))
    pixel[2] = ((((ord(data[2]) - 33) & 0x3) << 6) | ((ord(data[3]) - 33)))
    return pixel

def h_to_bitmap(header_data, width, height):
    bitmap = []
    for i in range(0, len(header_data), 4):
        pixel = decode_pixel(header_data[i:i+4])
        color = f"0x{pixel[0]:02X}{pixel[1]:02X}{pixel[2]:02X}"
        bitmap.append(color)
    return bitmap

def main():
    if len(sys.argv) != 2:
        print("Usage: python h2array.py <input_file.h>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    
    if not os.path.isfile(input_file):
        print(f"Error: File '{input_file}' does not exist.")
        sys.exit(1)
    
    print(f"Input file: {input_file}")
    print("File exists and is ready to be processed.")
    
    h_content = read_h_file(input_file)
    width, height, header_data = parse_h_file(h_content)
    bitmap = h_to_bitmap(header_data, width, height)
    
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
    
    print(f"\n// Image dimensions: {width}x{height}")

if __name__ == "__main__":
    main()