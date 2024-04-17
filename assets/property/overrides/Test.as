namespace Weather {
    namespace CLEAR {
        namespace Commander {
            namespace JAKE {
                const string longName("Jake's Clear Weather");
                // Short is overriding long somehow.
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
                const string icon("clear");
            }
        }
        namespace Commander {
            namespace JAKE {
                const string icon("jake");
            }
        }
    }
}
