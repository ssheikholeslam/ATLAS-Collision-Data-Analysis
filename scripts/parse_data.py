import uproot
import numpy as np
import struct
import os

"""
Parses ATLAS DAOD_PHYSLITE collision data from multiple ROOT files into a single
binary format for C++ analysis.

Background: Calculates rest-energy out as the sum of rest masses for stable particles
(electrons, muons, photons) and invariant masses for jets and tau jets, converted
from MeV to GeV. Efficiency is rest_energy_out / total_energy_in, where
total_energy_in is the 13 TeV center-of-mass energy (NOTE: we ignore rest-energy
of the two protons involved in the collision for simplicity as order-of-magnitude
estimate). Ensures physical consistency by maintaining units in GeV and validating efficiency ≤ 1.
"""

# List your five ROOT files
root_files = [
    "data/DAOD_PHYSLITE.37019878._000001.pool.root.1",
    "data/DAOD_PHYSLITE.37019878._000002.pool.root.1",
    "data/DAOD_PHYSLITE.37019878._000003.pool.root.1",
    "data/DAOD_PHYSLITE.37019878._000004.pool.root.1",
    "data/DAOD_PHYSLITE.37019878._000006.pool.root.1",
    "data/DAOD_PHYSLITE.37019878._000007.pool.root.1",
    "data/DAOD_PHYSLITE.37019878._000008.pool.root.1", # This file here is technically unneeded for collecting 100,000 events
]

# Verify they exist
root_files = [f for f in root_files if os.path.exists(f)]
if not root_files:
    raise FileNotFoundError("No ROOT files found in data/ directory")

# Particle collections
fixed_mass_collections = [
    "AnalysisElectronsAuxDyn",
    "AnalysisMuonsAuxDyn",
    "AnalysisPhotonsAuxDyn"
]
variable_mass_collections = [
    "AnalysisJetsAuxDyn",
    "AnalysisTauJetsAuxDyn"
]

# Rest masses in GeV
rest_masses = {
    "AnalysisElectronsAuxDyn": 0.000511,
    "AnalysisMuonsAuxDyn":    0.10566,
    "AnalysisPhotonsAuxDyn":  0.0
}

# Names for output
particle_names = {
    "AnalysisElectronsAuxDyn": "electron",
    "AnalysisMuonsAuxDyn":     "muon",
    "AnalysisPhotonsAuxDyn":   "photon",
    "AnalysisJetsAuxDyn":      "jet",
    "AnalysisTauJetsAuxDyn":   "tau"
}

# Branches we need
branches = (
        ["EventInfoAuxDyn.eventNumber"] +
        [f"{col}.pt" for col in fixed_mass_collections] +
        [f"{col}.m"  for col in variable_mass_collections]
)

events = []
events_per_file = []
event_limit     = 100_000
total_energy_in = 13_000.0  # GeV

# Loop file by file
for filepath in root_files:
    tree = uproot.open(filepath)["CollectionTree"]
    count_this_file = 0

    # iterate in batches of up to event_limit (or smaller)
    for batch in tree.iterate(branches, library="np"):
        event_numbers = batch["EventInfoAuxDyn.eventNumber"]
        for idx in range(len(event_numbers)):
            # global stop
            if len(events) >= event_limit:
                break

            eid = int(event_numbers[idx])
            rest_energy_out = 0.0
            outgoing = []

            # fixed‑mass
            for col in fixed_mass_collections:
                pts = batch[f"{col}.pt"][idx]
                n   = len(pts)
                rest_energy_out += n * rest_masses[col]
                outgoing.extend([particle_names[col]] * n)

            # variable‑mass
            for col in variable_mass_collections:
                masses = batch[f"{col}.m"][idx]
                rest_energy_out += np.sum(masses) / 1000.0
                outgoing.extend([particle_names[col]] * len(masses))

            in_str  = "proton,proton".ljust(32)
            out_str = ",".join(outgoing)[:256].ljust(256)
            eff     = rest_energy_out / total_energy_in

            if eff > 1.0:
                print(f"⚠️  Warning: Event {eid} efficiency={eff:.2f}")

            events.append((eid, in_str, out_str, total_energy_in, rest_energy_out, eff))
            count_this_file += 1

        if len(events) >= event_limit:
            break

    events_per_file.append(count_this_file)
    if len(events) >= event_limit:
        break

# Summary
print(f"Processed {len(events)} events across {len(events_per_file)} files")
for i, cnt in enumerate(events_per_file, 1):
    print(f"  File {i}: {cnt} events")
print(f"Max efficiency:       {max(evt[5] for evt in events):.4f}")
print(f"Max rest-energy-out:  {max(evt[4] for evt in events):.2f} GeV")

# Write binary
with open("data/collision_data.bin", "wb") as f:
    for eid, in_str, out_str, kin, rest, eff in events:
        f.write(struct.pack("i",   eid))
        f.write(struct.pack("32s", in_str.encode("utf-8")))
        f.write(struct.pack("256s", out_str.encode("utf-8")))
        f.write(struct.pack("fff", kin, rest, eff))
