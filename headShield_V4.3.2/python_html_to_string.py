input_file_path = r"C:\Users\user\Desktop\headShield\3_programming\headShield_V4.3.2\webpage.html"
output_file_path = r"C:\Users\user\Desktop\headShield\3_programming\headShield_V4.3.2\webpage.h"

with open(input_file_path, 'r', encoding='utf-8') as f:
    content = f.read()

header_content = f'#pragma once\n\nconst char* webpageCode = R"=====(\n{content})=====";'

with open(output_file_path, 'w', encoding='utf-8') as f:
    f.write(header_content)
