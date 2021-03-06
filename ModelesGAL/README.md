# GAL Models

This folder contains the GAL models built during the internship of Di Zhao, see [her report](https://github.com/lip6/TSAR-DHCCP/raw/master/Reports/rapport_dizhao_2015.pdf).

The models were analyzed with [ITS-tools](http://ddd.lip6.fr) embedded in Eclipse.

To reproduce the experiments, run the models with ITS-tools. 
From eclipse, right-click any GAL file, then ["Run As->ITS-Model-check".](https://lip6.github.io/ITSTools-web/running.html).

From the command line, you can use the [its-commandline](https://yanntm.github.io/ITS-commandline/) distribution to work on your files.
e.g. `./its-tools -i model.gal -ctl` or `./its-tools -i model.gal -reach`. 
Note that directly running the models with `its-reach` or `its-ctl` will not work, parameters need to be degeneralized. 

The folders contain :
* example : small models testing various features used in the real models
* components : the individually developed components of the system. There are "unit tests" in many of these files, ie. properties that assert reachability of certain situations, built to validate the modeling. Uncomment or comment those you are interested in, they come in packs corresponding to different scenarios. Look at the graphical depicition of the scenarios (sequence diagrams) in the same folder for details. There are also variants of the cache components bearing _vX markers, they should all be valid individually but not all versions can be assembled together.
* platform : The full assembled platform. idealtsar model is a non parametric version, the three other versions are semantically equivalent variants, that use different decompositions of the model. The most efficient version in our experiments was v2. Just set the three major parameters at the top of the file, add some properties if desired and right click -> run as "ITS model-check"
* gen : some models generated for different values of the parameters from v2. These models were used to collect the data points presented in our companion paper at MARS 2018. 


 

