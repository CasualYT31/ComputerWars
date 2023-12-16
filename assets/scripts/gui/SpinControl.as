/**
 * @file SpinControl.as
 * Defines the \c SpinControl class.
 */

/**
 * Represents a spin control in a GUI.
 */
shared class SpinControl : SubwidgetContainer {
    /**
     * Create the spin control.
     */
    SpinControl() {
        super(SpinControlEngineName);
    }

    void setValue(const float v) {
        ::setWidgetValue(this, v);
    }

    float getValue() const {
        return ::getWidgetValue(this);
    }

    void setMinMaxValues(const float min, const float max) {
        ::setWidgetMinMaxValues(this, min, max);
    }
}
