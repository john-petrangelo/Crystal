import os

def generate_header_file(header_file, file_data):
    with open(header_file, 'w') as f:
        f.write('#pragma once\n\n')
        for name in file_data.keys():
            f.write(f'extern char const * {name};\n')

def generate_source_file(source_file, header_file, file_data):
    with open(source_file, 'w') as f:
        f.write(f'#include "{header_file}"\n\n')
        for name, content in file_data.items():
            f.write(f'char const * {name} = R"rawliteral(\n')
            f.write(content)
            f.write(')rawliteral";\n\n')

def read_file_content(file_path):
    with open(file_path, 'r') as file:
        return file.read()

def main():
    # List of files to process
    files = [
        '../../data/index.html',
        '../../data/crystal.css',
        '../../data/crystal.js'
    ]

    file_data = {}

    # Iterate over specified files
    for file_path in files:
        name = os.path.basename(file_path).replace('.', '_').replace('-', '_').upper()
        content = read_file_content(file_path)
        file_data[name] = content

    # Output header and source file names
    header_file = 'web_files.h'
    source_file = 'web_files.cpp'

    # Generate header and source files
    generate_header_file(header_file, file_data)
    generate_source_file(source_file, header_file, file_data)

if __name__ == '__main__':
    main()
