#paste results.phasing.notrack.100.b* | 
< results.phasing.notrack.1000.b90 fldtotable j b90 | 
row 'j<17' | 
~/Scripts/plottable.py j,b90 color blue j,b90 marker circle color blue xtitle "Phasing J mag limit" ytitle "Sky coverage" title "Phasing sky coverage at Galactic pole"
