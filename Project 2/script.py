import os

source_dir = "."
output_dir = "csv_files"
if not os.path.exists(output_dir):
    os.mkdir(output_dir)

files = ["customer.tbl", "lineitem.tbl", "nation.tbl", "orders.tbl", "part.tbl", "partsupp.tbl", "region.tbl", "supplier.tbl"]

for file in files:
    input_file = os.path.join(source_dir, file)
    output_file = os.path.join(output_dir, os.path.splitext(file)[0] + ".csv")

    with open(input_file, "r") as infile, open(output_file, "w") as outfile:
        for line in infile:
            # Remove trailing "|" character and write to the CSV file
            line = line.rstrip('\n')  # Remove newline characters
            if line.endswith("|"):
                line = line[:-1]  # Remove the trailing "|"
            outfile.write(line + "\n")

print("Conversion and cleanup complete.")
