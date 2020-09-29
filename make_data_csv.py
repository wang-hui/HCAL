input_file = "results_temp/reco_data.stdout" 
output_file = "results_temp/result_data.csv" 

f = open(input_file, "r")
all_lines = f.readlines()
f.close()

out_lines = all_lines[0].split("reco: ")[1]
for line in all_lines:
    if "reco" not in line:
        out_lines += line

f = open(output_file, "w")
f.write(out_lines)
f.close()
