/**
 * @file BitmapButton.as
 * Defines the \c BitmapButton class.
 */

/**
 * Represents a bitmap button in a GUI.
 */
shared class BitmapButton : Button {
    /**
     * Create the bitmap button.
     */
    BitmapButton() {
        super(BitmapButtonEngineName);
    }

    void setSprite(const string&in sheet, const string&in sprite) {
        setWidgetSprite(this, sheet, sprite);
    }
}
