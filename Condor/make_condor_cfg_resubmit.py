err_job_list = "/uscms_data/d3/huiwang/condor_temp/huiwang/HCAL/run3_relVal_PU65-2020-06-09/err_job.list"

f = open(err_job_list, "r")
err_index_list = f.readlines()
f.close()

for i in range(len(err_index_list)):
	err_index_list[i] = err_index_list[i].split(".")[0]

print err_index_list

f = open("condor_submit.txt", "r")
origin_list = f.readlines()
f.close()
#resub_list = list(origin_list) # copy a list in python 2, like list.copy() in python 3
resub_str = ""
for i in  range(len(origin_list)):
	if "Arguments" in origin_list[i]:
		origin_index =  origin_list[i].split(" ")[-1].split("\n")[0] # -1 returns the last element of a list
		if origin_index not in err_index_list:
			origin_list[i] = "\#"
			origin_list[i+1] = "\#"
	if origin_list[i] != "\#": resub_str += origin_list[i]

f = open("condor_re-submit.txt", "w")
f.write(resub_str)
f.close()
