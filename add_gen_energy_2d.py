import numpy as np
import pandas as pd
import ROOT as rt
import glob

#pd.set_option('display.max_rows', None)

reco_list = glob.glob("split_file/reco_*.txt")
gen_list = glob.glob("split_file/gen_*.txt")
reco_list.sort()
gen_list.sort()
reco_list_tmp = [item.replace("reco", "gen") for item in reco_list]
if reco_list_tmp != gen_list:
    print "gen and reco list do not match"
    quit()

print "read the first file and change float64 to float 32"
reco_test = pd.read_csv(reco_list[0], sep=',', skipinitialspace = True, header=0)
reco64_cols = [c for c in reco_test if reco_test[c].dtype == "float64"]
reco32_cols = {c: np.float32 for c in reco64_cols}
gen_test = pd.read_csv(gen_list[0], sep=',', skipinitialspace = True, header=0)
gen64_cols = [c for c in gen_test if gen_test[c].dtype == "float64"]
gen32_cols = {c: np.float32 for c in gen64_cols}

#out_file = rt.TFile("reco_miss_id.root","RECREATE")
#miss_energy_h = rt.TH1F("miss_energy_h", "energy of simHit miss in reco", 100, 0.0, 10.0)

result = pd.DataFrame()
for i in range (len(reco_list)):
#for i in range (1):
    if i%50 == 0: print "processing %d file" %i
    #print "reco file: ", reco_list[i]
    #print "gen file: ", gen_list[i]
    reco_df=pd.read_csv(reco_list[i], engine='c', dtype=reco32_cols, sep=',', skipinitialspace = True, header=0)
    gen_df=pd.read_csv(gen_list[i], engine='c', dtype=gen32_cols, sep=',', skipinitialspace = True, header=0)

    if len(gen_df.index) == 0: continue
    #print reco_df["id"]
    #print "origin number of id: ", len(gen_df["id"])
    #for rawId in gen_df["id"]:
    #    if rawId not in reco_df["id"].values: print "mismatch id: ", rawId
    #Nrows = gen_df.shape[0]
    #for j in range(Nrows):
    #    for k in range(6):
    #        depth = "d" + str(k+1)
    #        if reco_df[depth + " is real channel"][j] == 1 and (reco_df[depth + " gain"][j] == 0 or reco_df[depth + " raw gain"][j] == 0):
    #            print reco_df.loc[[j]]

    next_pd = pd.merge(reco_df, gen_df, on=["ieta", "iphi"], how="left")
    result = result.append(next_pd)

print "final processing"
#print list(result)
print "saving to csv"
result.to_csv("results_temp/result_2d.csv", index=False)
