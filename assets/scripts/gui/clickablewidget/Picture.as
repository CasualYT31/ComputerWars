/**
 * @file Picture.as
 * Defines the \c Picture class.
 */

/**
 * Represents a picture in a GUI.
 */
shared class Picture : ClickableWidget {
    /**
     * Create the picture.
     */
    Picture() {
        super(PictureEngineName);
    }

    void setSprite(const string&in sheet, const string&in sprite) {
        setWidgetSprite(this, sheet, sprite);
    }
}
