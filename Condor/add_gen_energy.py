import numpy as np
import pandas as pd
import ROOT as rt
import glob
import pickle

#pd.set_option('display.max_rows', None)

f = open("MinBias.pkl","rb")
MinBias_dic = pickle.load(f)
f.close

def find_PU_energy(sub_det, depth, ieta):
	return MinBias_dic[(sub_det, depth, abs(ieta))]

reco_list = glob.glob("split_file/reco_*.txt")
gen_list = glob.glob("split_file/gen_*.txt")
reco_list.sort()
gen_list.sort()
reco_list_tmp = [item.replace("reco", "gen") for item in reco_list]
if reco_list_tmp != gen_list: print "gen and reco list do not match"

print "read the first file and change float64 to float 32"
reco_test = pd.read_csv(reco_list[0], nrows=10, sep=',', skipinitialspace = True, header=0)
reco64_cols = [c for c in reco_test if reco_test[c].dtype == "float64"]
reco32_cols = {c: np.float32 for c in reco64_cols}
gen_test = pd.read_csv(gen_list[0], nrows=10, sep=',', skipinitialspace = True, header=0)
gen64_cols = [c for c in gen_test if gen_test[c].dtype == "float64"]
gen32_cols = {c: np.float32 for c in gen64_cols}

out_file = rt.TFile("reco_miss_id.root","RECREATE")
miss_energy_h = rt.TH1F("miss_energy_h", "energy of simHit miss in reco", 100, 0.0, 10.0)

result = pd.DataFrame()
for i in range (len(reco_list)):
#for i in range (1):
	if i%50 == 0: print "processing %d file" %i
	#print "reco file: ", reco_list[i]
	#print "gen file: ", gen_list[i]
	reco_df=pd.read_csv(reco_list[i], engine='c', dtype=reco32_cols, sep=',', skipinitialspace = True, header=0)
	gen_df=pd.read_csv(gen_list[i], engine='c', dtype=gen32_cols, sep=',', skipinitialspace = True, header=0)

	#print reco_df["id"]
	#print "origin number of id: ", len(gen_df["id"])
	#for rawId in gen_df["id"]:
	#	if rawId not in reco_df["id"].values: print "mismatch id: ", rawId
	#Nrows = gen_df.shape[0]
	#for j in range(Nrows):
	#	if gen_df["id"][j] not in reco_df["id"].values:
	#		gen_energy = gen_df["energy"][j]
	#		miss_energy_h.Fill(gen_energy)
	#		if gen_energy > 10:
	#			print i, gen_df["id"][j], gen_energy

	next_pd = pd.merge(reco_df, gen_df, on="id", how="left")
	PU = gen_df["PU"][0]
	next_pd["energy"].fillna(0, inplace=True)
	next_pd["PU"].fillna(PU, inplace=True)
	next_pd["unit PU energy"] = next_pd.apply(lambda row: find_PU_energy(row["sub detector"], row["depth"], row["ieta"]), axis = 1)
	next_pd["truth energy"] = next_pd.apply(lambda row: row["energy"] + row["PU"] * row["unit PU energy"], axis = 1)
	result = result.append(next_pd)

print "final processing"
#temp_list = []
#for energy in result["energy"]:
#	if energy != 0: temp_list.append(energy)

#print "number of id after merge: ", len(temp_list)
#print temp_list
#print list(result)

out_file.cd()
out_file.Write()
out_file.Close()

result.drop(['id'], axis=1, inplace=True)
result.rename(columns={"energy": "raw truth energy"}, inplace=True)

#print list(result)
print "saving to csv"
result.to_csv("result.csv", index=False)
