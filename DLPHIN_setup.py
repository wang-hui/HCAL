import sys
import os

if len(sys.argv) != 2 or sys.argv[1] not in ["diff", "cp", "mv"]:
    print "Bad argument. Should be:"
    print "python DLPHIN_setup.py <diff / cp / mv>"
    quit()

command = sys.argv[1]
os.system(command + "  BuildFile.xml ../RecoLocalCalo/HcalRecAlgos/")
os.system(command + "  HBHEPhase1Reconstructor_cfi.py ../RecoLocalCalo/HcalRecProducers/python/")
os.system(command + "  HBHEPhase1Reconstructor.cc ../RecoLocalCalo/HcalRecProducers/src/")

os.system(command + "  DLPHIN.h ../RecoLocalCalo/HcalRecAlgos/interface/")
os.system(command + "  DLPHIN.cc ../RecoLocalCalo/HcalRecAlgos/src/")
os.system(command + "  DLPHIN_cfi.py ../RecoLocalCalo/HcalRecProducers/python/")
