namespace Weather {
    namespace CLEAR {
        namespace Commander {
            namespace JAKE {
                const string longName("Jake's Clear Weather");
                string shortName("shortie");

                // void longName(string& name) {
                //     name += "APPEND";
                // }

                void particles(ParticleDataArray& particles) {
                    particles.array.resize(5);
                    particles.array[4].vector.x = -1000.5;
                }
            }
        }
    }
}

namespace Environment {
    namespace NORMAL {
        namespace Weather {
            namespace CLEAR {
                void icon(string&out icon) {
                    icon += "-clear";
                }
            }
        }
        namespace Commander {
            namespace JAKE {
                void icon(string&out icon) {
                    icon += "-jake";
                }
            }
        }
    }
}

namespace Country {
    namespace ORANGE {
        namespace Environment {
            namespace _NORMAL {
                void longName(string&out name) {
                    name += "-normal";
                }
            }
        }
        namespace Weather {
            namespace CLEAR {
                void longName(string&out name) {
                    name += "-clear";
                }
            }
        }
        namespace Commander {
            namespace JAKE {
                void longName(string&out name) {
                    name += "-jake";
                }
            }
        }
    }
}

// // Don't use pod&out here, I think it frees it early after the function ends?
// void func(pod& passing, const pod&in around) {
//     passing.message = around.message;
// }

// void main(Coords simCoords) {
//     pod test();
//     pod another("another");
//     pod j(another);
//     info(test.message);
//     info(another.message);
//     info(j.message += "2");
//     if (another == j) {
//         info("Yes!");
//     }
//     info((test = another).message);
//     func(another, j);
//     info(another.message);
//     pod finalTest = another;
//     if (finalTest == another) info("EQUAL!");
//     info("?" + finalTest.message);
//     info(j.message);
//     simCoords.x += -0.5;
//     info(formatFloat(simCoords.x, "", 0, 4));
//     Coords copy = simCoords;
//     copy.x += 0.00001;
//     copy.y -= 0.000001;
//     if (copy == simCoords) info("YES!");
//     copy.z = 10000.78;
//     if (copy != simCoords) info("NO!");
//     CoordPair pair1(simCoords, copy);
//     info(formatFloat(pair1.b.z, "", 0, 4));
//     info("Zeroed");
//     Coords zeroTest;
//     info(formatFloat(zeroTest.x, "", 0, 4));
// }

// #expand Test a b c d e f g h i j k l m n o p q r s t u v w x y z
#expand Test 1  3 4 5 6 7 8 9 0 1 2 3 4 5 6

// #expand ThisIsATest

// #assert true;
// #assert true; "Proper"
// #assert true; "Proper
// #assert true; Proper"
// #assert true; "Pro""er"
// #assert true; Pro""er
// #assert true; None!
// #assert true; """
// #assert true; "
