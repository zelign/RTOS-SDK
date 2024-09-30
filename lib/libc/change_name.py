import os
import glob
import re

def change_name(dir):
    files = glob.glob(os.path.join(dir, '*'))
    for file in files:
        if os.path.isfile(file):
            new_file_name = (os.path.basename(file)).replace("aml_", "sm_", 1)
            new_file_path = os.path.join(dir, new_file_name)
            os.rename(file, new_file_path)
def change_content(dir):
    files = glob.glob(os.path.join(dir, '*'))
    modified_files = []
    for file in files:
        if os.path.isfile(file):
            with open(file, 'r') as in_file:
                lines = in_file.readlines()
                for line in lines:
                    if line.startswith('#ifndef'):
                        match = re.search(r'#ifndef\s+(__AML_\w+)', line)
                        if match:
                            ml = match.group(1)
                            modify_ml = ml.replace('__AML', '__SM', 1)
                            line = f'#ifndef {modify_ml}\n'
                    modified_files.append(line)
            with open(file, 'w') as out_file:
                out_file.writelines(modified_files)
                modified_files.clear()   
                                 

if __name__ == "__main__":
    root_dir = os.getcwd()
    # change_name(os.path.join(root_dir, "sm_libc"))
    change_content(os.path.join(root_dir, "sm_libc"))

