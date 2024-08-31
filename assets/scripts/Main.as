uint returnThis(const uint i) {
    throw("Hi");
    return i;
}

uint func() {
    int y = 5;
    return returnThis(32);
}
