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
