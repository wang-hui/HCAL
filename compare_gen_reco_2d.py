import ROOT as rt
import pandas as pd
pd.set_option("display.max_columns", 999)

result_dir = "results_temp/"
result_file = "result_2d"

tot_rows = None
#tot_rows = 100000

Ebins = 400
Emin = 0.0
Emax = 1000.0

col_keep = []
for d in range(1,8):
    col_keep.append('d{} is real channel'.format(d))
    col_keep.append('d{} weighted time'.format(d))
    col_keep.append('d{} raw truth energy'.format(d))
    col_keep.append('d{} raw gain'.format(d))
    col_keep.append('d{} gain'.format(d))
    col_keep.append('d{} raw energy'.format(d))
    col_keep.append('d{} aux energy'.format(d))
    col_keep.append('d{} mahi energy'.format(d)) 
    col_keep.append('d{} DLPHIN energy'.format(d))
result = pd.read_csv(result_dir + result_file + ".csv", dtype='float32', sep=',', skipinitialspace = True, header=0, nrows=tot_rows, usecols=col_keep)

out_file = rt.TFile(result_dir + result_file + ".root","RECREATE")

raw_vs_gen_sum_h = rt.TH2F("raw_vs_gen_sum_h", "raw_vs_gen_sum_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_sum_h = rt.TH2F("aux_vs_gen_sum_h", "aux_vs_gen_sum_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
mahi_vs_gen_sum_h = rt.TH2F("mahi_vs_gen_sum_h", "mahi_vs_gen_sum_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_sum_h = rt.TH2F("DLPHIN_vs_gen_sum_h", "DLPHIN_vs_gen_sum_h", Ebins, Emin, Emax, Ebins, Emin, Emax)

raw_vs_gen_list = []
aux_vs_gen_list = []
mahi_vs_gen_list = []
DLPHIN_vs_gen_list = []
for d in range(1,8):
    raw_vs_gen_hist = rt.TH2F("raw_vs_gen_depth_" + str(d) + "_h", "raw_vs_gen_depth_" + str(d) + "_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
    raw_vs_gen_list.append(raw_vs_gen_hist)
    aux_vs_gen_hist = rt.TH2F("aux_vs_gen_depth_" + str(d) + "_h", "aux_vs_gen_depth_" + str(d) + "_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
    aux_vs_gen_list.append(aux_vs_gen_hist)
    mahi_vs_gen_hist = rt.TH2F("mahi_vs_gen_depth_" + str(d) + "_h", "mahi_vs_gen_depth_" + str(d) + "_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
    mahi_vs_gen_list.append(mahi_vs_gen_hist)
    DLPHIN_vs_gen_hist = rt.TH2F("DLPHIN_vs_gen_depth_" + str(d) + "_h", "DLPHIN_vs_gen_depth_" + str(d) + "_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
    DLPHIN_vs_gen_list.append(DLPHIN_vs_gen_hist)

def row_filter(df):
    df = df[df['d1 is real channel']+df['d2 is real channel']+df['d3 is real channel']+df['d4 is real channel']
        +df['d5 is real channel']+df['d6 is real channel']+df['d7 is real channel'] > 0]
    df = df[df['d1 raw truth energy']+df['d2 raw truth energy']+df['d3 raw truth energy']+df['d4 raw truth energy']
        +df['d5 raw truth energy']+df['d6 raw truth energy']+df['d7 raw truth energy'] > 0]

    for d in range(1,8):
        df['d{} mask'.format(d)] = ((df['d{} is real channel'.format(d)] == 1)
                                & (df['d{} weighted time'.format(d)] < 100)).astype(int)
        df.pop('d{} is real channel'.format(d))
        df.pop('d{} weighted time'.format(d))

    df = df[df['d1 mask']+df['d2 mask']+df['d3 mask']+df['d4 mask']
        +df['d5 mask']+df['d6 mask']+df['d7 mask'] > 0]    
    return df

def scale_reco(df):
    for d in range(1,8):
        df['d{} respCorr'.format(d)] = df['d{} raw gain'.format(d)]/df['d{} gain'.format(d)]
        df['d{} respCorr'.format(d)] = df['d{} respCorr'.format(d)].fillna(0)
        df.pop('d{} raw gain'.format(d))
        df.pop('d{} gain'.format(d))
        df['d{} raw energy'.format(d)] = df['d{} raw energy'.format(d)] * df['d{} respCorr'.format(d)]
        df['d{} aux energy'.format(d)] = df['d{} aux energy'.format(d)] * df['d{} respCorr'.format(d)]
        df['d{} mahi energy'.format(d)] = df['d{} mahi energy'.format(d)] * df['d{} respCorr'.format(d)]
    return df

Nrows = result.shape[0]
print "total rows: ", Nrows
result = row_filter(result)
Nrows = result.shape[0]
print "left rows: ", Nrows
print "scale reco now"
result = scale_reco(result)
result = result.reset_index(drop=True)

for i in range(Nrows):
    if i%100000 == 0: print "process %d rows" %i

    gen_energy_sum = 0
    raw_energy_sum = 0
    aux_energy_sum = 0
    mahi_energy_sum = 0
    DLPHIN_energy_sum = 0

    for d in range(1,8):
        mask = result["d{} mask".format(d)][i]
        if mask > 0:
            gen_energy = result["d{} raw truth energy".format(d)][i]
            raw_energy = result["d{} raw energy".format(d)][i]
            aux_energy = result["d{} aux energy".format(d)][i]
            mahi_energy = result["d{} mahi energy".format(d)][i]
            DLPHIN_energy = result["d{} DLPHIN energy".format(d)][i]

            raw_vs_gen_list[d-1].Fill(gen_energy, raw_energy)
            aux_vs_gen_list[d-1].Fill(gen_energy, aux_energy)
            mahi_vs_gen_list[d-1].Fill(gen_energy, mahi_energy)
            DLPHIN_vs_gen_list[d-1].Fill(gen_energy, DLPHIN_energy)

            gen_energy_sum += gen_energy
            raw_energy_sum += raw_energy
            aux_energy_sum += aux_energy
            mahi_energy_sum += mahi_energy
            DLPHIN_energy_sum += DLPHIN_energy

    raw_vs_gen_sum_h.Fill(gen_energy_sum, raw_energy_sum)
    aux_vs_gen_sum_h.Fill(gen_energy_sum, aux_energy_sum)
    mahi_vs_gen_sum_h.Fill(gen_energy_sum, mahi_energy_sum)
    DLPHIN_vs_gen_sum_h.Fill(gen_energy_sum, DLPHIN_energy_sum)

out_file.cd()
out_file.Write()
out_file.Close()
