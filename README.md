# lheToRoot
A root script to convert a lhe file (txt file) to a root file,
each event in lhe file becomes an entry in the treeEvent of root file

Ref.: https://arxiv.org/pdf/hep-ph/0609017.pdf

Branch in treeInit -- store <init></init> information in lhe file

Branch in treeEvent -- store <event></event> information, an entry per event
* PUP[NUP][5] -- PUP[i][5] is the px, py, pz, e, mass for particle i
* IDUP[NUP] -- IDUP[i] is the pdgid of particle i
* ISTUP[NUP] -- ISTUP[i] is the status of particle i
  * -1 means incoming
  * 1 means final
  * 2 means intermediate
