import sys
import shutil
import os
import subprocess

def sync_and_install(src, dest, extensions):
    try:
        subprocess.check_call([sys.executable, "-m", "ensurepip", "--upgrade"])
    except subprocess.CalledProcessError:
        print("Warning: Could not bootstrap pip. Ensure your Python environment is valid.")
    
    # 1. Copy if folder doesn't exist
    if not os.path.exists(dest):
        print(f"Creating directory: {dest}")
        os.makedirs(dest)
        shutil.copytree(src, dest, dirs_exist_ok=True)
        changed = True
    else:
        changed = False

    # 2. Only install if we just copied, or if you want to force check
    if changed:
        print(f"Installing extensions into {dest}...")
        # Force pip to use the target folder as the installation site
        # We use the 'python -m pip' pattern to ensure we use the pip 
        # associated with the current interpreter, but redirect the --target
        subprocess.check_call([
            sys.executable, "-m", "pip", "install", 
            "--target", dest, 
            *extensions
        ])
    else:
        print("Python folder already exists. Skipping sync/install.")

if __name__ == "__main__":
    # sys.argv[1] = src, sys.argv[2] = dest, sys.argv[3:] = extensions
    sync_and_install(sys.argv[1], sys.argv[2], sys.argv[3:])