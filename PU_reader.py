import ROOT as rt
from DataFormats.FWLite import Events, Handle

out_file = rt.TFile("PU_test.root","RECREATE")

obsPU_h = rt.TH1F("obsPU_h", "observe pileup", 100, 0.0, 100.0)
truePU_h = rt.TH1F("truePU_h", "true pileup", 100, 0.0, 100.0)

events = Events ('/eos/uscms/store/user/huiwang/HCAL/MH-125_MFF-50_CTau-10000mm_step1.root')

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
