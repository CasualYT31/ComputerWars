int test(int val) {
    return val;
}
int val;

namespace parent {
    int test2(int val) {
        return val;
    }
    int val2;

    namespace child {
        int test3(int val) {
            return val;
        }
        int val3;

        namespace grandchild {
            int test4(int val) {
                return val;
            }
            int val4;
        }
    }
}
