Import("env")
import os
import shutil

def copy_files(source, destination):
    if not os.path.exists(destination):
        os.makedirs(destination)
    for item in os.listdir(source):
        s = os.path.join(source, item)
        d = os.path.join(destination, item)
        if os.path.isdir(s):
            copy_files(s, d)
        else:
            shutil.copy2(s, d)

def add_spiffs_files(source, target, env):
    spiffs_dir = os.path.join(env.get("PROJECT_DIR"), "data")
    config_dir = os.path.join(env.get("PROJECT_DIR"))
    
    # Copy config files to SPIFFS directory
    copy_files(config_dir, spiffs_dir)

    # Remove unnecessary files from SPIFFS directory
    for item in os.listdir(spiffs_dir):
        if item.endswith(('.py', '.ini', '.md', '.gitignore')):
            os.remove(os.path.join(spiffs_dir, item))

env.AddPreAction("buildfs", add_spiffs_files)