import numpy as np
import pandas as pd
import glob

#pd.set_option('display.max_rows', None)

reco_list = glob.glob("split_file/reco_*.txt")
print "total reco files: ", len(reco_list)
gen_list = glob.glob("split_file/gen_*.txt")
print "total gen files: ", len(gen_list)

print "read the first file and change float64 to float 32"
reco_test = pd.read_csv(reco_list[0], nrows=2, sep=',', skipinitialspace = True, header=0)
reco64_cols = [c for c in reco_test if reco_test[c].dtype == "float64"]
reco32_cols = {c: np.float32 for c in reco64_cols}
gen_test = pd.read_csv(gen_list[0], nrows=2, sep=',', skipinitialspace = True, header=0)
gen64_cols = [c for c in gen_test if gen_test[c].dtype == "float64"]
gen32_cols = {c: np.float32 for c in gen64_cols}

result = pd.DataFrame()
for i in range (len(reco_list)):
#for i in range (5):
	if i%100 == 0: print "processing %d file" %i
	reco_df=pd.read_csv(reco_list[i], engine='c', dtype=reco32_cols, sep=',', skipinitialspace = True, header=0)
	gen_df=pd.read_csv(gen_list[i], engine='c', dtype=gen32_cols, sep=',', skipinitialspace = True, header=0)

	#print reco_df["id"]
	#print "origin number of id: ", len(gen_df["id"])
	#for rawId in gen_df["id"]:
	#if rawId not in reco_df["id"].values: print "mismatch id: ", rawId

	result = result.append(pd.merge(reco_df, gen_df, on="id", how="left"))
	PU = gen_df["PU"][0]
	result["energy"].fillna(0, inplace=True)
	result["PU"].fillna(PU, inplace=True)

print "final processing"
#temp_list = []
#for energy in result["energy"]:
#	if energy != 0: temp_list.append(energy)

#print "number of id after merge: ", len(temp_list)
#print temp_list

#print list(result)
result.drop(['id'], axis=1, inplace=True)
result.rename(columns={"energy": "truth energy"}, inplace=True)
#print list(result)
print "saving to csv"
result.to_csv("result.csv", index=False)
