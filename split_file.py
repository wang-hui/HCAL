import os
os.system("rm split_file/*.txt")
source_file = open("results_temp/reco_test.stdout","r")
event = 1
for gen_and_reco in source_file.read().split("gen: "):
	if gen_and_reco != "":
		gen_file = open ("split_file/gen_" + str(event) + ".txt", 'w')
		gen_file.write(gen_and_reco.split("reco: ")[0])
		gen_file.close()
		reco_file = open ("split_file/reco_" + str(event) + ".txt", 'w')
		reco_file.write(gen_and_reco.split("reco: ")[1])
		reco_file.close()
		event = event + 1
source_file.close()
