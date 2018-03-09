# TSAR-DHCCP
Verification of the Distributed Hybrid Cache Coherence protocol of the TeraScale Architecture

This repository contains formal models of the [DHCCP](https://www-soc.lip6.fr/trac/tsar/wiki/CacheCoherence) protocol used in [TSAR](https://www-soc.lip6.fr/trac/tsar/wiki/Specification).

These models were built by students at Sorbonne Université (formerly Université Pierre et Marie Curie), at the Laboratoire d'Informatique de Paris 6 (LIP6), in Paris, France.

Students that participated :
* Mohamad Najem, Master 1, 2011
* Akli Mansour, Master 1, 2012
* Zahia Gharbi, Master 1, 2013
* Di Zhao, Master 2, 2015

Each of these students built models and a report (**in French**) on their work, which can be found in the "Reports" folder. 
There are also a couple of slide shows and other documentation (automata of the components) in that folder.
 

These students were supervised by [Emmanuelle Encrenaz](http://www-soc.lip6.fr/~ema/), [Quentin Meunier](http://www-soc.lip6.fr/~meunier/) and [Yann Thierry-Mieg](https://pages.lip6.fr/Yann.Thierry-Mieg/), all three being professors at Sorbonne Université. See each report for more details.

Three modeling languages were used to build the models
* **Promela** the language of the tool [Spin](http://spinroot.com/)
* **[Divine Language](https://divine.fi.muni.cz/darcs/branch-3.0/gui/help/divine/language.html)** the language of the tool [DiViNe](https://divine.fi.muni.cz/). We used Divine version 2 in these experiments.
* **[GAL](https://lip6.github.io/ITSTools-web/gal.html)** the Guarded Action Language of the [ITS-tools](http://ddd.lip6.fr)

There is a folder with a README for each of these languages, containing the models and how to run the experiments.

This repository is a companion to our [MARS2018](http://mars-workshop.org/mars2018/) paper. 

All model files in this repository can be freely used and modified. 
The various reports remain the copyrighted property of their respective authors, they may be copied and distributed but not edited.