# Setup
setwd("C:/Users/Luis Henrique/Documents/Faculdade/Orion/ct-transportes/implementacao_romulo/hybrid_cpp/irace")
library(irace)
library(iraceplot)

# Irace
scenario <- readScenario(filename = "scenario.txt",
                         scenario = defaultScenario())
irace_main(scenario = scenario)

# Read log file
report("logs/irace.log")