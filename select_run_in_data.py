input_file = "DoubleMuon_Run2018A_raw_FNAL.list"
output_file = "DoubleMuon_Run2018A_Run_315512_raw.list"

f = open(input_file, "r")
all_lines = f.readlines()
f.close()

out_lines = ""
for line in all_lines:
    if "315/512" in line:
        out_lines = out_lines + "root://cmsxrootd.fnal.gov/" + line

f = open(output_file, "w")
f.write(out_lines)
f.close()
