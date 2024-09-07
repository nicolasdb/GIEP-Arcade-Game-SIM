import sys
import os
import re

def read_xpm_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    return content

def preprocess_xpm(content):
    lines = content.strip().split('\n')
    
    # Find the line with dimensions and color count
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
        parts = line.strip('",').split(None, 2)
        char, color_def = parts[0], parts[-1]
        color_dict[char] = color_def

    # Check if space is used as a color character
    if ' ' in color_dict:
        # Find an unused character
        replacement_char = next(chr(i) for i in range(33, 127) if chr(i) not in color_dict)
        
        # Replace space with the new character in color definitions
        space_color = color_dict[' ']
        del color_dict[' ']
        color_dict[replacement_char] = space_color
        
        # Update color definition lines
        for j, line in enumerate(lines[i+1:i+1+num_colors], start=i+1):
            if line.strip('",').startswith(' '):
                lines[j] = line.replace(' ', replacement_char, 1)
        
        # Replace space with the new character in pixel data
        for j in range(i+1+num_colors, len(lines)):
            lines[j] = lines[j].replace(' ', replacement_char)

    return '\n'.join(lines)

def parse_xpm(content):
    lines = content.strip().split('\n')
    
    for i, line in enumerate(lines):
        if re.match(r'"\d+\s+\d+\s+\d+\s+\d+"', line):
            header = re.findall(r'\d+', line)
            width, height, num_colors, chars_per_pixel = map(int, header)
            break
    else:
        raise ValueError("Could not find dimension information in XPM file")

    color_dict = {}
    for line in lines[i+1:i+1+num_colors]:
        parts = line.strip('",').split(None, 2)
        char, color_def = parts[0], parts[-1]
        if color_def.startswith('c '):
            color_def = color_def[2:]  # Remove the 'c ' prefix
        color_dict[char] = color_def.strip()

    pixel_data = ''.join([line.strip('",') for line in lines[i+1+num_colors:] if line.strip().startswith('"')])
    pixel_data = pixel_data[:width * height]

    return width, height, color_dict, pixel_data

def xpm_to_bitmap(color_dict, pixel_data, width, height):
    bitmap = []
    for char in pixel_data:
        color = color_dict.get(char, '#FFFFFF')  # Default to white if character not found
        if color.startswith('#'):
            color = '0x' + color[1:]  # Convert '#RRGGBB' to '0xRRGGBB'
        bitmap.append(color)
    return bitmap

def main():
    if len(sys.argv) != 2:
        print("Usage: python xpm2array.py <input_file.xpm>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    
    if not os.path.isfile(input_file):
        print(f"Error: File '{input_file}' does not exist.")
        sys.exit(1)
    
    print(f"Input file: {input_file}")
    print("File exists and is ready to be processed.")
    
    xpm_content = read_xpm_file(input_file)
    preprocessed_content = preprocess_xpm(xpm_content)
    width, height, color_dict, pixel_data = parse_xpm(preprocessed_content)
    bitmap = xpm_to_bitmap(color_dict, pixel_data, width, height)
    
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
    print("// Color definitions:")
    for char, color in color_dict.items():
        print(f"//   '{char}': {color}")

if __name__ == "__main__":
    main()