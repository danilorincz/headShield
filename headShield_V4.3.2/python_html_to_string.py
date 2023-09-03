def escape_string(s):
    return s.replace("\\", "\\\\").replace("\"", "\\\"")

input_file_path = r"C:\Users\user\Desktop\headShield\3_programming\headShield_V4.3.2\webpage.html"
output_file_path = r"C:\Users\user\Desktop\headShield\3_programming\headShield_V4.3.2\webpage.h"

with open(input_file_path, 'r', encoding='utf-8') as f:
    content = f.read()

escaped_content = escape_string(content)

header_content = f'#pragma once\n\nconst char* webpageCode = R"=====(\n{escaped_content})=====";'

with open(output_file_path, 'w', encoding='utf-8') as f:
    f.write(header_content)