namespace Weather {
    namespace CLEAR {
        namespace Commander {
            namespace JAKE {
                const string longName("Jake's Clear Weather");
                string shortName("shortie");

                // void longName(string&out name) {
                //     name += "APPEND";
                // }
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

// Don't use pod&out here, I think it frees it early after the function ends?
void func(pod& passing, const pod&in around) {
    passing.message = around.message;
}

void main() {
    pod test();
    pod another("another");
    pod j(another);
    info(test.message);
    info(another.message);
    info(j.message += "2");
    if (another == j) {
        info("Yes!");
    }
    info((test = another).message);
    func(another, j);
    info(another.message);
    pod finalTest = another;
    info("?" + finalTest.message);
    info(j.message);
}
