from ROOT import *
import pandas as pd

#def return_HcalDetId_member(HcalDetId_, member):
#	hid = HcalDetId(HcalDetId_)
#	return getattr(hid, member)

my_pd = pd.read_csv("reco_miss_id.txt", sep=' ', names=["event", "id", "truth energy"])
#my_pd["subdet"] = my_pd.apply(lambda row: HcalDetId(row["id"]).subdet, axis = 1)

#rawId = return_HcalDetId_member(my_pd["id"][0], "rawId")
#print rawId
#print my_pd

Nrows = my_pd.shape[0]
for i in range(Nrows):
	hid = HcalDetId(my_pd["id"][i])
	rawId = hid.rawId()
	subdet = hid.subdet()
	depth = hid.depth()
	ieta = hid.ieta()
	iphi = hid.iphi()
	energy = my_pd["truth energy"][i]
	if energy > 20:
		print my_pd["event"][i], subdet, depth, ieta, iphi, energy
