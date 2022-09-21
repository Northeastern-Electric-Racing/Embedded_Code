# Embedded_Code
Contains all the code for the embedded systems found within the car as of the 2023 competition

Built off of the [NERduino](https://github.com/Northeastern-Electric-Racing/NERduino) library

## Steps for Contributing
1. Clone the repo via SSH
```
git clone git@github.com:Northeastern-Electric-Racing/Embedded_Code.git
```

2. Update the submodules
```
git submodule update --init
```

**NOTE** If you want to pull new changes from the dependencies (i.e. new changes in the NERduino library)


## Overview of Repo
```
.
|
|───MPU (Master Processing Unit)
|───TCU (Telemetry Control Unit)
|
|───Legacy Code
|   |───Brakelight
|   |───Dashboard
|   └───PedalBox
|
└───README.md
```

## Introduction
If you're new here, make sure to complete all the steps from [this onboarding document](https://nerdocs.atlassian.net/wiki/spaces/NER/pages/1343533/Data+Controls+Onboarding). It'll basically run you through the basics of git and setting up git on your system, installing VSCode and PlatformIO, and some basics of functions and objects in C++.
