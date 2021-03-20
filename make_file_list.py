import glob

eos_dir = "/eos/uscms/store/user/lpcrutgers/huiwang/HCAL_ML_test_samples/UL_1TeV_pion_gun_PU_2d"
post_fix = "csv"

file_name_list = glob.glob(eos_dir + "/*." + post_fix)
for file_name in file_name_list:
    out_name = "root://cmseos.fnal.gov/" + file_name
    print out_name

