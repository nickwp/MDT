import sys
sys.path.append('/home/ryosuke/play/MDT')

import MDT


def get_true_hits(event_id):
    tube_id = []
    hit_time = []
    f = open('TrueHits_pdg11_tres1p5ns_NoDark_mom200.txt','r')
    for line in f:
        data = line.split()
        if int(data[0])==event_id :
            tube_id.append(int(data[1])) 
            hit_time.append(float(data[2]))
    f.close()
    return tube_id, hit_time

event_id = 0
conf = MDT.Configuration()
conf.ReadParameter('./../MDTParamenter.txt')
mdt_man = MDT.MDTManager(78923)
hit_col = mdt_man.GetHitTubeCollection()
tube_id, hit_time = get_true_hits(event_id)
print('hit_col.GetNumOfHitTubes():', hit_col.GetNumOfHitTubes())
for i in range(0, len(tube_id)):
    ht = hit_col.AddTrueHit(tube_id[i], hit_time[i])
print('hit_col.GetNumOfHitTubes():', hit_col.GetNumOfHitTubes())
print('hit_col.GetTotalNumOfTrueHits:', hit_col.GetTotalNumOfTrueHits())
print('hit_col.GetTotalNumOfDigiHits:', hit_col.GetTotalNumOfDigiHits())

mdt_man.DoAddDark()
mdt_man.DoDigitize()
mdt_man.DoTrigger()

print('hit_col.GetTotalNumOfDigiHits:', hit_col.GetTotalNumOfDigiHits())

ti = mdt_man.GetTriggerInfo()
print("ti.GetNumOfTrigger", ti.GetNumOfTrigger())
print("ti.GetTriggerTime", ti.GetTriggerTime(0))
print("ti.GetLowEdge", ti.GetLowEdge(0))
print("ti.GetUpEdge", ti.GetUpEdge(0))
print("ti.GetType", ti.GetType(0))

tube_id_list = hit_col.GetHitTubeIDList()
n_digi_hits = 0
print("n_digi_hits: ", n_digi_hits)
print("len(tube_id_list)", len(tube_id_list))
for id in tube_id_list:
    ht = hit_col.GetHitTube(id)
    n_digi_hits += ht.GetNDigiHits()

print("n_digi_hits: ", n_digi_hits)
