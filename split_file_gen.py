import os
os.system("rm split_file_gen/*.txt")
source_file = open("opendata_2018_TTbar_gen.stdout","r")
event = 1
for gen_and_reco in source_file.read().split("gen: "):
	if gen_and_reco != "":
		gen_file = open ("split_file_gen/gen_" + str(event) + ".txt", 'w')
		gen_file.write(gen_and_reco)
		gen_file.close()
		event = event + 1
source_file.close()
