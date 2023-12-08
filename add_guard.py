'''
Automation script to do the following to read the list of all C++ headers
and add include guards to them.
Include guards added in format:
    <PROJECT_NAME>_<PATH>_<FILENAME>_H_
'''
import argparse
import json
import os
import subprocess
import sys

def return_json(file_name):
    '''
    Function to read json data
    '''
    json_file = open(file_name)
    data = json.load(json_file)
    json_file.close()
    return data

if __name__ == "__main__":
    parser = argparse.ArgumentParser(usage= "python3 add_guard.py -f [json file]",
                                     description= "Automation script to add include guards")
    parser.add_argument("-f", "--filename")
    args = parser.parse_args()
    if args.filename is None:
        print("Invalid use. Missing argument -f\n{}".format(parser.usage), file=sys.stderr)
        sys.exit(1)
    data = return_json(args.filename)


    END_ = data["end"]
    FILE = data["file"]
    CURR_PATH = os.getcwd()

    #Compile the C++ program and keep the binary ready
    subprocess.run(data["c-compile"])

    with open(FILE, 'r') as f:
        for file_name in f:
            path_vars = set()
            #Add 'include' to list of keywords in path to be ignored
            path_vars.add("include")
            header_keywords = []

            file_name = data["project"] + file_name.strip()[1:]
            filename_complete_path = os.path.join(CURR_PATH, file_name)
            path = filename_complete_path.split(data["delim1"])[5:]
            header_name = path[-1].split(data["delim2"])[0]
            path[-1] = header_name
            for path_var in path:
                if path_var in path_vars:
                    continue
                header_keywords.append(path_var.upper())
                path_vars.add(path_var)
                #Checking if any of the directories in path equal filename. Don't want repetition in header
                if path_var == header_name.lower():
                    break
            header = "_".join(header_keywords) +  END_
            #Run the C++ program to add include guards to header files
            completed = subprocess.run(" ".join([data["run-bin"], filename_complete_path, header]),
                                       shell=True, capture_output=True)
            if completed.returncode != 0:
                print(completed.args)
                print("Failed to write to file: {}".format(path[-1] + ".hpp"), sys.stderr)
            print(completed.stderr.decode("utf-8"), sys.stderr)

    subprocess.run(data["clean"])
