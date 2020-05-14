import os
from shutil import copyfile
from datetime import date

folder_name = "opendata_2018_TTbar"
result_path = "/eos/uscms/store/user/huiwang/HCAL/"
condor_path = "/uscms_data/d3/huiwang/condor_temp/huiwang/HCAL/"
file_list = "../FileList/opendata_2018_TTbar.list"
tot_jobs = 32

today = str(date.today())
folder_name_full = folder_name + "-" + today
result_path_full = result_path + folder_name_full
condor_path_full = condor_path + folder_name_full

os.mkdir(result_path_full)
os.mkdir(condor_path_full)

f = open(file_list, "r")
my_list = f.readlines()
f.close()

header = (""
+ "Output = " + condor_path_full + "/$(Process).out\n"
+ "Error = " + condor_path_full + "/$(Process).err\n"
+ "Log = " + condor_path_full + "/$(Process).log\n"
)

tot_lines = len(my_list)
my_step = int(tot_lines / tot_jobs) #int returns ceil
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

os.system("tar -cvf FileList.tar FileList_*.list")
#os.system("rm -f FileList_*.list")

copyfile("condor_submit.back", "condor_submit.txt")
f = open("condor_submit.txt", "a")
f.write(header)
f.close()
