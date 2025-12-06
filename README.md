# ECE2165_FileCorruption

## Data configurations

| Config Number | Bit Length Dataword | Bit Length Code | Code Type              | Features |
|:-------------:|:-------------------:|:---------------:|:----------------------:|:--------:|
| 0             | 8                   | 4               | SnglPrec Checksum      | Detect   |
| 1             | 8                   | 8               | DblPrec Checksum       | Detect   |
| 2             | 8                   | 4               | SnglePrec Res Checksum | Detect   |
| 3             | 8                   | 8               | Honeywell Checksum     | Detect   |
| 4             | 8                   | 4               | CRC                    | Detect   |

## Corruption configurations
| Corruption Number | Corruption Type | Notes                                                                    |
|:-----------------:|:---------------:|:------------------------------------------------------------------------:|
| 0                 | Single bit flip |                                                                          |
| 1                 | Burst bit flips | Specify length of burst through corruptionTypeOption (default = 3)       |
| 2                 | Random flips    | Specify the number of flips through corruptionTypeOption (default = 3)   |
| 4                 | No corruption   | Use as a baseline test (should get 0% detection)                         |

## How to run

Run the project as follows

chmod +x ./run_project.sh

./run_project.sh "configNumber" "corruptionType" "corruptionTypeOption"

EX: ./run_project.sh 2 3 15