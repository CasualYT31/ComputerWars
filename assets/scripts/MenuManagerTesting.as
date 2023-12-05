class Test : Menu {
    Test() {
        info("Test constructor (replacement for SetUp())!");
    }

    string Name() const {
        return "Test";
    }

    void Open(Menu@ const previousMenu) {
    }

    void Close(Menu@ const nextMenu) {
    }

    void HandleInput(const dictionary controls, const dictionary mouseControls,
        const MousePosition&in previousMousePosition,
        const MousePosition&in currentMousePosition) {
    }
}

class AnotherTest : Menu {
    string Name() const {
        return "AnotherTest";
    }

    void Open(Menu@ const previousMenu) {
    }

    void Close(Menu@ const nextMenu) {
    }

    void HandleInput(const dictionary controls, const dictionary mouseControls,
        const MousePosition&in previousMousePosition,
        const MousePosition&in currentMousePosition) {
    }
}

class Base {
}

class Derived : Base {
}
