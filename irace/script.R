setwd("C:/workingdir/irace")
library(irace)

scenario <- readScenario(filename = "scenario.txt",
                         scenario = defaultScenario())
irace_main(scenario = scenario)