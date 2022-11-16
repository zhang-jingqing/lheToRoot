# lheToRoot
A root script to convert an lhe file (txt file) to a root file,
each event in lhe file becomes an entry in the treeEvent of the root file

Ref.: https://arxiv.org/pdf/hep-ph/0609017.pdf

Branch in treeInit -- store \<init\>\<\/init\> information in the lhe file

Branch in treeEvent -- store \<event\>\<\/event\> information, an entry per event
* PUP[NUP][5] -- PUP[i][5] is the px, py, pz, e, mass for particle i
* IDUP[NUP] -- IDUP[i] is the pdgid of particle i
* ISTUP[NUP] -- ISTUP[i] is the status of particle i
  * -1 means incoming
  * 1 means final
  * 2 means intermediate
* MOTHUP[NUP][2] -- MOTHUP[i][2] is the mother index (of the \*UP[NUP] array) of particle i. If from collision, particle i  has two mothers in MOTHUP[i][0] and MOTHUP[i][1]. If from decay, particle i has one mother, the two elements in MOTHUP[i][0] and MOTHUP[i][1] should be same.

To see all elements in the tree, use tree-\>Print(), or tree-\>Show(0)
