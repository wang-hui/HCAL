import glob
import sys

if len(sys.argv) != 3:
    print "Require 2 argv"
    print "argv[1] = EOS dir"
    print "argv[2] = file postfix"
    quit()

eos_dir = sys.argv[1]
post_fix = sys.argv[2]

file_name_list = glob.glob(eos_dir + "/*." + post_fix)
for file_name in file_name_list:
    out_name = "root://cmseos.fnal.gov/" + file_name
    print out_name

