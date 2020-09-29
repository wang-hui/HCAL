import ROOT as rt
reco_output = "TTbar_PU65.stderr" 

f = open(reco_output, "r")
all_lines = f.readlines()
f.close()

out_file = rt.TFile("HBHE_reco_time.root","RECREATE")
HBHE_reco_time_h = rt.TH1D("HBHE_reco_time_h", "HBHE_reco_time_h", 100, 0.0, 0.15)

for line in all_lines:
    if "HBHEPhase1Reconstructor" in line:
        #print line
        time = line.split("HBHEPhase1Reconstructor")[1]
        time = time.split("\n")[0]
        HBHE_reco_time_h.Fill(float(time))

out_file.cd()
out_file.Write()
out_file.Close()
