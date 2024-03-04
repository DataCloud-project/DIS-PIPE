import subprocess
import os

array_subfolder_path=["AutSeg/segmentator", "Example/SEG/AutSeg/segmentator", "storage/testuser/Example/SEG/AutSeg/segmentator"]

for subfolder_path in array_subfolder_path:
    if os.path.exists(subfolder_path):
        # Run the "make" command in the specified subfolder
        result1 = subprocess.run(["make", "clean"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=subfolder_path)
        # Check if the command was successful
        if result1.returncode == 0:
            print("Command 'make clean' executed successfully on: "+subfolder_path)
        else:
            print("'make clean' on: "+subfolder_path+" error executing command:")
            print(result1.stderr.decode("utf-8"))


        # Run the "make" command in the specified subfolder
        result2 = subprocess.run(["make"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=subfolder_path)
        # Check if the command was successful
        if result2.returncode == 0:
            print("Command 'make' executed successfully on: "+subfolder_path)
        else:
            print("'make' error on: "+subfolder_path+" executing command:")
            print(result2.stderr.decode("utf-8"))



        # Run the "make" command in the specified subfolder
        result3 = subprocess.run(["make","install"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=subfolder_path)
        # Check if the command was successful
        if result3.returncode == 0:
            print("Command 'make install' executed successfully on: "+subfolder_path)
        else:
            print("'make install' on: "+subfolder_path+" error executing command:")
            print(result3.stderr.decode("utf-8"))
    else:
        if(subfolder_path=="/storage/testuser/Example/SEG/AutSeg/segmentator"):
            print("The specified subfolder path does not exist but you could do the next step")
        else:
            print("The specified subfolder path does not exist there is a problem, try to download source another time")
            