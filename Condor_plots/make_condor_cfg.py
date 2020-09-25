import os
from shutil import copyfile
from datetime import date

folder_name = "2018_1TeV_pion_gun_PU0_plots"
result_path = "/eos/uscms/store/user/huiwang/HCAL/"
condor_path = "/uscms_data/d3/huiwang/condor_temp/huiwang/HCAL/"
file_list = "../FileList/2018_1TeV_pion_gun_PU0_results_csv.list"

today = str(date.today())
folder_name_full = folder_name + "-" + today
result_path_full = result_path + folder_name_full
condor_path_full = condor_path + folder_name_full

#try: os.mkdir(result_path_full)
#except OSError: print result_path_full, " already exisit. Make sure you want to save there" 
#try: os.mkdir(condor_path_full)
#except OSError: print condor_path_full, " already exisit. Make sure you want to save there" 
os.system("mkdir -p " + result_path_full)
os.system("mkdir -p " + condor_path_full)

f = open(file_list, "r")
my_list = f.readlines()
f.close()

header = (""
+ "Output = " + condor_path_full + "/$(Process).out\n"
+ "Error = " + condor_path_full + "/$(Process).err\n"
+ "Log = " + condor_path_full + "/$(Process).log\n"
)

for j in range (len(my_list)):
    my_file = my_list[j].strip()
    header = header + "\nArguments = " + my_file + " " + result_path_full + "/ " + str(j)
    header = header + "\nQueue"

copyfile("condor_submit.back", "condor_submit.txt")
f = open("condor_submit.txt", "a")
f.write(header)
f.close()
