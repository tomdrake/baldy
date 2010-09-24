library("RUnit")
library("boot")

# source the scripts so that the functions are available on all nodes
#for (nm in list.files("sprint-0.2/inst/unitTests/pboot/", pattern = "\\.[RrSsQq]$")) {
#       source(nm)
#}

for (nm in list.files("sprint-0.2/inst/unitTests/pboot/", pattern = "\\.[Rr]$")){
  source(file.path("sprint-0.2/inst/unitTests/pboot/", nm))
}


#source("baldy/sprint-0.2/inst/unitTests/pboot/runnit_standard.R")
test.suite <- defineTestSuite("pboot", dirs = file.path("sprint-0.2/inst/unitTests/pboot/"),testFileRegexp = '*.R')
library("sprint")
test.result <- runTestSuite(test.suite)
printTextProtocol(test.result)
pterminate()
quit()

