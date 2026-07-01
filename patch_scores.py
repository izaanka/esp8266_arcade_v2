import os
import re

directory = '/home/izaan/Documents/esp8266_arcade-main'
count = 0

for filename in os.listdir(directory):
    if filename.endswith(".h"):
        filepath = os.path.join(directory, filename)
        with open(filepath, 'r') as f:
            content = f.read()
            
        if 'getScore()' in content:
            continue
            
        if re.search(r'\bscore\b', content, re.IGNORECASE):
            # It has some kind of score. Find "public:"
            if "public:" in content:
                content = content.replace("public:", "public:\n    virtual int getScore() override { return score; }")
                with open(filepath, 'w') as f:
                    f.write(content)
                count += 1

print(f"Patched {count} files.")
