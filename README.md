# Automation script for adding include guards to C++ headers.

## Part of my Open source contribution to the Noelle project.

### How to run:

- Running test

1. Requires the json with the necessary configuration.
2. Need to build the text file in the required format. Command to do that. Note that excluding test files is optional.
```bash
grep -R "#pragma once" --include=\*.hpp --exclude-dir=\*tests/
```
For test, build the text file in the assets folder.
3. Run the command:
```python
python3 add_guard.py -f json_file.json
```

