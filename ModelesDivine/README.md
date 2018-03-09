# Divine Models

This folder contains the Divine models built during the internship of Zahia Gharbi, see [her report](https://github.com/lip6/TSAR-DHCCP/raw/master/Reports/rapport_final_gharbi_2013.pdf).

Most folders contain a trace of execution in an OpenOffice odt file and explain the nature of each model.

The models were analyzed with [DiViNe](https://divine.fi.muni.cz/) and with [ITS-tools](http://ddd.lip6.fr)

To reproduce the experiments, run the models with one of these two tools.

* With Divine, pass the .mdve file to the tool directly.
* With its-tools, there are two ways to do it currently; 
  * either run the [command line its-ctl](https://lip6.github.io/ITS-CTL/)  (or `its-reach, its-ltl`...) with flag -t DVE on non parametric DVE models. There are shell scripts that show how to do this.
  * or open the DVE files in eclipse, right click then choose "DVE to GAL", and finally right click -> "Run As" -> "ITS-tools model-check".

The second path is more modern, and includes some structural simplifications that are not available in the first path.
The experiments in Zahia's work used the first path.


