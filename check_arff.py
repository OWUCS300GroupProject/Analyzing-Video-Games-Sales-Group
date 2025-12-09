num_attributes = 11
with open("output.arff") as f:
    in_data = False
    for i, line in enumerate(f, 1):
        line = line.strip()
        if line.lower() == "@data":
            in_data = True
            continue
        if in_data and line:
            values = line.split(",")
            if len(values) != num_attributes:
                print(f"Line {i} has {len(values)} values: {line}")
