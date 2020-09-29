input_file = "DoubleMuon_Run2018A_raw.list" 
output_file = "DoubleMuon_Run2018A_Run_316944_raw.list" 

f = open(input_file, "r")
all_lines = f.readlines()
f.close()

out_lines = ""
for line in all_lines:
    if "316/944" in line:
        out_lines += line

f = open(output_file, "w")
f.write(out_lines)
f.close()
