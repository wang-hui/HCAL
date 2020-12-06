import os
from shutil import copyfile
from datetime import date

folder_name = "DoubleMuon_Run2018A_Run_315512_raw_DLPHIN_no_SF"
result_path = "/eos/uscms/store/user/lpcrutgers/huiwang/HCAL/"
condor_path = "/uscms_data/d3/huiwang/condor_temp/huiwang/HCAL/"
file_list = "../FileList/DoubleMuon_Run2018A_Run_315512_raw.list"
tot_jobs = 200

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

tot_lines = len(my_list)
my_step = max(int(tot_lines / tot_jobs),1) #int returns floor
list_of_sublist = []
for i in xrange (0, tot_lines, my_step):
	list_of_sublist.append(my_list[i : i + my_step])

for j in range (len(list_of_sublist)):
	f = open("FileList_" + str(j) + ".list", "w")
	for line in list_of_sublist[j]:
		f.write(line)
	f.close()
	header = header + "\nArguments = FileList_" + str(j) + ".list " + result_path_full + "/ " + str(j)
	header = header + "\nQueue"

os.system("tar -cf FileList.tar FileList_*.list")
os.system("mkdir -p FileList_test")
os.system("rm FileList_test/*.list")
os.system("mv FileList_*.list FileList_test")

copyfile("condor_submit.back", "condor_submit.txt")
f = open("condor_submit.txt", "a")
f.write(header)
f.close()
