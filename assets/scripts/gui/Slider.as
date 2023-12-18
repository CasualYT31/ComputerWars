/**
 * @file Slider.as
 * Defines the \c Slider class.
 */

/**
 * Represents a slider in a GUI.
 */
shared class Slider : Widget {
    /**
     * Create the slider.
     */
    Slider() {
        super(SliderEngineName);
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
