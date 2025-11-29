# ECE2165_FileCorruption

## Data configurations

| Config Number | Bit Length Dataword | Bit Length Code | Code Type              | Features |
|:-------------:|:-------------------:|:---------------:|:----------------------:|:--------:|
| 1             | 8                   | 4               | SnglPrec Checksum      | Detect   |
| 2             | 8                   | 8               | DblPrec Checksum       | Detect   |
| 3             | 8                   | 4               | SnglePrec Res Checksum | Detect   |
| 4             | 8                   | 8               | Honeywell Checksum     | Detect   |
| 5             | 8                   | 4               | CRC                    | Detect   |

## Corruption configurations
| Corruption Number | Corruption Type | Notes                                                                    |
|:-----------------:|:---------------:|:------------------------------------------------------------------------:|
| 1                 | Single bit flip |                                                                          |
| 2                 | Burst bit flips | Specify length of burst through corruptionTypeOption (default = 3)       |
| 3                 | Random flips    | The number and position of flips is completely random                    |

## How to run

Run the project as follows

chmod +x ./run_project.sh

./run_project.sh "configNumber" "corruptionType" "corruptionTypeOption"

EX: ./run_project.sh 2 3 15