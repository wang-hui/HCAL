import sys
import os

if len(sys.argv) != 2 or sys.argv[1] not in ["diff", "cp", "mv"]:
    print "Bad argument. Should be:"
    print "python DLPHIN_setup.py <diff / cp / mv>"
    quit()

Oper = sys.argv[1]

File_Loca_Dict = {
"BuildFile.xml"                         : "../RecoLocalCalo/HcalRecAlgos/",
"HBHEPhase1Reconstructor_cfi.py"        : "../RecoLocalCalo/HcalRecProducers/python/",
"HBHEPhase1Reconstructor.cc"            : "../RecoLocalCalo/HcalRecProducers/src/",

"DLPHIN.h"                              : "../RecoLocalCalo/HcalRecAlgos/interface/",
"DLPHIN.cc"                             : "../RecoLocalCalo/HcalRecAlgos/src/",
"DLPHIN_cfi.py"                         : "../RecoLocalCalo/HcalRecProducers/python/",
}

for File, Loca in File_Loca_Dict.items():
    Command = Oper + " " + File + " " + Loca
    print Command
    os.system(Command)
