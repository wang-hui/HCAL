import ROOT as rt
from DataFormats.FWLite import Events, Handle

out_file = rt.TFile("PU_test.root","RECREATE")

obsPU_h = rt.TH1F("obsPU_h", "observe pileup", 100, 0.0, 100.0)
truePU_h = rt.TH1F("truePU_h", "true pileup", 100, 0.0, 100.0)

#events = Events ('root://cmsxrootd.fnal.gov//store/relval/CMSSW_11_0_0_patch1/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/110X_mcRun3_2021_realistic_v6-v1/20000/CE2177A9-5C2D-DD48-B9AB-42AAB7180734.root')
#events = Events ('/eos/uscms/store/user/huiwang/HCAL/opendata/206A6E9A-4DB2-1941-A60B-7174FA398D86.root')
events = Events ('/eos/uscms/store/user/huiwang/HCAL/TTToHadronic_TuneCP5_13TeV-powheg-pythia8_GEN-SIM-RAW_NZSFlatPU28to62_102X_upgrade2018_realistic_v15_ext1-v1_FC39115C-B4B9-C247-A1EC-50A4C0754E62.root')
#events = Events ('/eos/uscms/store/user/huiwang/ElectroWeakino/official_samples/Neutrino_E-10_gun_GEN-SIM-DIGI-RAW_MC_v2_94X_mc2017_realistic_v9-v1_2EA05F08-17CE-E711-8DEC-A4BF0112BC6A.root')

#my_list = []
#for i in range(15):
#	my_list.append('/eos/uscms/store/user/huiwang/ElectroWeakino/miniAOD_test/mini_mn1_100_mx1_110_' + str(i+1) + '.root')
#events = Events (my_list)

# create handle outside of loop
handle  = Handle ('vector<PileupSummaryInfo>')
label = ("addPileupInfo")
#label = ("slimmedAddPileupInfo")
# loop over events
max_event = -1
cnt=0
for event in events:
    if max_event != -1 and cnt > max_event:
    	continue
    if cnt%1000 == 0: print "process %d events" %cnt

    event.getByLabel(label, handle)
    #print event.object().id().event()
    products = handle.product()
    
    for product in products:
	if product.getBunchCrossing() == 0:
		#print product.getPU_NumInteractions (), product.getTrueNumInteractions ()
		obsPU_h.Fill(product.getPU_NumInteractions ())
		truePU_h.Fill(product.getTrueNumInteractions ())
    cnt+=1 

out_file.cd()
out_file.Write()
out_file.Close()
