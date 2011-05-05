#!/bin/bash

# script for the data generation -- also serves as "run log"

#arguments: run number, energy (keV), field (G), lambda (nm), dead layer (nm), broadening (G), asymmetry

#./simDataFullSpec 1   2.0  50.0 100.0  5.0  1.174  0.060
#./simDataFullSpec 2   3.6  50.0 100.0  5.0  1.174  0.065
#./simDataFullSpec 3   5.0  50.0 100.0  5.0  1.174  0.075
#./simDataFullSpec 4   7.5  50.0 100.0  5.0  1.174  0.085
#./simDataFullSpec 5   10.0 50.0 100.0  5.0  1.174  0.095
#./simDataFullSpec 6   14.1 50.0 100.0  5.0  1.174  0.100
#./simDataFullSpec 7   17.8 50.0 100.0  5.0  1.174  0.105
#./simDataFullSpec 8   21.1 50.0 100.0  5.0  1.174  0.110
#./simDataFullSpec 9   23.1 50.0 100.0  5.0  1.174  0.115
#./simDataFullSpec 10  24.6 50.0 100.0  5.0  1.174  0.120

#./simDataFullSpec 11  2.0  50.0 100.0  5.0  5.87   0.060
#./simDataFullSpec 12  3.6  50.0 100.0  5.0  5.87   0.065
#./simDataFullSpec 13  5.0  50.0 100.0  5.0  5.87   0.075
#./simDataFullSpec 14  7.5  50.0 100.0  5.0  5.87   0.085
#./simDataFullSpec 15  10.0 50.0 100.0  5.0  5.87   0.095
#./simDataFullSpec 16  14.1 50.0 100.0  5.0  5.87   0.100
#./simDataFullSpec 17  17.8 50.0 100.0  5.0  5.87   0.105
#./simDataFullSpec 18  21.1 50.0 100.0  5.0  5.87   0.110
#./simDataFullSpec 19  23.1 50.0 100.0  5.0  5.87   0.115
#./simDataFullSpec 20  24.6 50.0 100.0  5.0  5.87   0.12

#./simDataFullSpec 21   2.0  100.0 100.0  5.0  1.174  0.060
#./simDataFullSpec 22   3.6  100.0 100.0  5.0  1.174  0.065
#./simDataFullSpec 23   5.0  100.0 100.0  5.0  1.174  0.075
#./simDataFullSpec 24   7.5  100.0 100.0  5.0  1.174  0.085
#./simDataFullSpec 25   10.0 100.0 100.0  5.0  1.174  0.095
#./simDataFullSpec 26   14.1 100.0 100.0  5.0  1.174  0.100
#./simDataFullSpec 27   17.8 100.0 100.0  5.0  1.174  0.105
#./simDataFullSpec 28   21.1 100.0 100.0  5.0  1.174  0.110
#./simDataFullSpec 29   23.1 100.0 100.0  5.0  1.174  0.115
#./simDataFullSpec 30   24.6 100.0 100.0  5.0  1.174  0.120

#./simDataFullSpec 31  2.0  100.0 100.0  5.0  8.218   0.060
#./simDataFullSpec 32  3.6  100.0 100.0  5.0  8.218  0.065
#./simDataFullSpec 33  5.0  100.0 100.0  5.0  8.218   0.075
#./simDataFullSpec 34  7.5  100.0 100.0  5.0  8.218   0.085
#./simDataFullSpec 35  10.0 100.0 100.0  5.0  8.218   0.095
#./simDataFullSpec 36  14.1 100.0 100.0  5.0  8.218   0.100
#./simDataFullSpec 37  17.8 100.0 100.0  5.0  8.218   0.105
#./simDataFullSpec 38  21.1 100.0 100.0  5.0  8.218   0.110
#./simDataFullSpec 39  23.1 100.0 100.0  5.0  8.218   0.115
#./simDataFullSpec 40  24.6 100.0 100.0  5.0  8.218   0.120

#./simDataFullSpec 41  2.0  200.0 100.0  5.0  11.74   0.060
#./simDataFullSpec 42  3.6  200.0 100.0  5.0  11.74   0.065
#./simDataFullSpec 43  5.0  200.0 100.0  5.0  11.74   0.075
#./simDataFullSpec 44  7.5  200.0 100.0  5.0  11.74   0.085
#./simDataFullSpec 45  10.0 200.0 100.0  5.0  11.74   0.095
#./simDataFullSpec 46  14.1 200.0 100.0  5.0  11.74   0.100
#./simDataFullSpec 47  17.8 200.0 100.0  5.0  11.74   0.105
#./simDataFullSpec 48  21.1 200.0 100.0  5.0  11.74   0.110
#./simDataFullSpec 49  23.1 200.0 100.0  5.0  11.74   0.115
#./simDataFullSpec 50  24.6 200.0 100.0  5.0  11.74   0.120

#./simDataFullSpec 51  2.0  25.0 100.0  5.0  3.522   0.060
#./simDataFullSpec 52  3.6  25.0 100.0  5.0  3.522   0.065
#./simDataFullSpec 53  5.0  25.0 100.0  5.0  3.522   0.075
#./simDataFullSpec 54  7.5  25.0 100.0  5.0  3.522   0.085
#./simDataFullSpec 55  10.0 25.0 100.0  5.0  3.522   0.095
#./simDataFullSpec 56  14.1 25.0 100.0  5.0  3.522   0.100
#./simDataFullSpec 57  17.8 25.0 100.0  5.0  3.522   0.105
#./simDataFullSpec 58  21.1 25.0 100.0  5.0  3.522   0.110
#./simDataFullSpec 59  23.1 25.0 100.0  5.0  3.522   0.115
#./simDataFullSpec 60  24.6 25.0 100.0  5.0  3.522   0.120

#./simDataFullSpec 61   2.0  50.0 250.0  5.0  5.87  0.060
#./simDataFullSpec 62   3.6  50.0 250.0  5.0  5.87  0.065
#./simDataFullSpec 63   5.0  50.0 250.0  5.0  5.87  0.075
#./simDataFullSpec 64   7.5  50.0 250.0  5.0  5.87  0.085
#./simDataFullSpec 65   10.0 50.0 250.0  5.0  5.87  0.095
#./simDataFullSpec 66   14.1 50.0 250.0  5.0  5.87  0.100
#./simDataFullSpec 67   17.8 50.0 250.0  5.0  5.87  0.105
#./simDataFullSpec 68   21.1 50.0 250.0  5.0  5.87  0.110
#./simDataFullSpec 69   23.1 50.0 250.0  5.0  5.87  0.115
#./simDataFullSpec 70  24.6 50.0 250.0  5.0  5.87  0.120

#./simDataFullSpec 71  2.0  100.0 250.0  5.0  8.218   0.060
#./simDataFullSpec 72  3.6  100.0 250.0  5.0  8.218  0.065
#./simDataFullSpec 73  5.0  100.0 250.0  5.0  8.218   0.075
#./simDataFullSpec 74  7.5  100.0 250.0  5.0  8.218   0.085
#./simDataFullSpec 75  10.0 100.0 250.0  5.0  8.218   0.095
#./simDataFullSpec 76  14.1 100.0 250.0  5.0  8.218   0.100
#./simDataFullSpec 77  17.8 100.0 250.0  5.0  8.218   0.105
#./simDataFullSpec 78  21.1 100.0 250.0  5.0  8.218   0.110
#./simDataFullSpec 79  23.1 100.0 250.0  5.0  8.218   0.115
#./simDataFullSpec 80  24.6 100.0 250.0  5.0  8.218   0.120

#./simDataFullSpec 81  2.0  200.0 250.0  5.0  11.74   0.060
#./simDataFullSpec 82  3.6  200.0 250.0  5.0  11.74   0.065
#./simDataFullSpec 83  5.0  200.0 250.0  5.0  11.74   0.075
#./simDataFullSpec 84  7.5  200.0 250.0  5.0  11.74   0.085
#./simDataFullSpec 85  10.0 200.0 250.0  5.0  11.74   0.095
#./simDataFullSpec 86  14.1 200.0 250.0  5.0  11.74   0.100
#./simDataFullSpec 87  17.8 200.0 250.0  5.0  11.74   0.105
#./simDataFullSpec 88  21.1 200.0 250.0  5.0  11.74   0.110
#./simDataFullSpec 89  23.1 200.0 250.0  5.0  11.74   0.115
#./simDataFullSpec 90  24.6 200.0 250.0  5.0  11.74   0.120

#./simDataFullSpec 91  2.0  25.0 250.0  5.0  3.522   0.060
#./simDataFullSpec 92  3.6  25.0 250.0  5.0  3.522   0.065
#./simDataFullSpec 93  5.0  25.0 250.0  5.0  3.522   0.075
#./simDataFullSpec 94  7.5  25.0 250.0  5.0  3.522   0.085
#./simDataFullSpec 95  10.0 25.0 250.0  5.0  3.522   0.095
#./simDataFullSpec 96  14.1 25.0 250.0  5.0  3.522   0.100
#./simDataFullSpec 97  17.8 25.0 250.0  5.0  3.522   0.105
#./simDataFullSpec 98  21.1 25.0 250.0  5.0  3.522   0.110
#./simDataFullSpec 99  23.1 25.0 250.0  5.0  3.522   0.115
#./simDataFullSpec 100  24.6 25.0 250.0  5.0  3.522   0.120

./simDataFullSpec 101  2.0  25.0 100.0  5.0  5.87   0.060
./simDataFullSpec 102  3.6  25.0 100.0  5.0  5.87   0.065
./simDataFullSpec 103  5.0  25.0 100.0  5.0  5.87   0.075
./simDataFullSpec 104  7.5  25.0 100.0  5.0  5.87   0.085
./simDataFullSpec 105  10.0 25.0 100.0  5.0  5.87   0.095
./simDataFullSpec 106  14.1 25.0 100.0  5.0  5.87   0.100
./simDataFullSpec 107  17.8 25.0 100.0  5.0  5.87   0.105
./simDataFullSpec 108  21.1 25.0 100.0  5.0  5.87   0.110
./simDataFullSpec 109  23.1 25.0 100.0  5.0  5.87   0.115
./simDataFullSpec 110  24.6 25.0 100.0  5.0  5.87   0.120
