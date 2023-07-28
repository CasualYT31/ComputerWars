/**
 * @file DamageWidget.as
 * Declares the damage label widget type.
 */

/**
 * Used to display damage to the user.
 */
class DamageWidget {
    /**
     * Creates no widgets.
     */
    DamageWidget() {}

    /**
     * Creates the widget.
     * @param parent The full name of the container to add the damage label to.
     */
    DamageWidget(const string&in parent) explicit {
        _damageSprite = parent + ".DamageSprite";
        addWidget("Picture", _damageSprite);
        setWidgetSprite(_damageSprite, "icon", "damagetooltip");
        _damageLabel = parent + ".DamageLabel";
        addWidget("Label", _damageLabel);
        setWidgetTextColour(_damageLabel, Colour(250,100,0,255));
        setWidgetTextOutlineColour(_damageLabel, Colour(0,0,0,255));
        setWidgetTextOutlineThickness(_damageLabel, 2.0f);
        setWidgetTextSize(_damageLabel, 15);
        setWidgetOrigin(_damageLabel, 0.5f, 0.2f);
        setWidgetPosition(_damageLabel,
            _damageSprite + ".left + " + _damageSprite + ".width / 2",
            _damageSprite + ".top + " + _damageSprite + ".height / 2");
    }

    /**
     * Sets the damage to display on the widget.
     * @param damage The damage to display.
     */
    void setDamage(const int damage) {
        setWidgetText(_damageLabel, "~" + formatInt(damage) + "%");
        if (damage <= 100) {
            setWidgetTextColour(_damageLabel,
                LOW_DAMAGE_COLOUR.gradientTo(HIGH_DAMAGE_COLOUR, double(damage)));
        } else {
            setWidgetTextColour(_damageLabel,
                HIGH_DAMAGE_COLOUR.gradientTo(VERY_HIGH_DAMAGE_COLOUR,
                    double(damage - 100)));
        }
    }

    /**
     * Sets the widget's visibility.
     * @param visible \c TRUE if the widget should be visible, \c FALSE if not.
     */
    void setVisibility(const bool visible) {
        setWidgetVisibility(_damageSprite, visible);
        setWidgetVisibility(_damageLabel, visible);
    }

    /**
     * Updates the damage label's position based on a given pixel location, and
     * quadrant.
     * @param pos  The pixel location to draw with.
     * @param quad The quadrant the cursor is currently in.
     */
    void updatePosition(const MousePosition&in pos, const Quadrant&in quad) {
        setWidgetPosition(_damageSprite, formatInt(pos.x / 2), formatInt(pos.y / 2));
		switch (quad) {
		case Quadrant::LowerLeft:
            setWidgetOrigin(_damageSprite, 0.0f, 1.0f);
			break;
		case Quadrant::LowerRight:
            setWidgetOrigin(_damageSprite, 1.0f, 1.0f);
			break;
		case Quadrant::UpperRight:
            setWidgetOrigin(_damageSprite, 1.0f, 0.0f);
			break;
		default: // UpperLeft:
            setWidgetOrigin(_damageSprite, 0.0f, 0.0f);
		}
    }

    /**
     * Stores the name of the damage picture widget.
     */
    private string _damageSprite;

    /**
     * Stores the name of the damage label widget.
     */
    private string _damageLabel;

    /**
     * The colour of the damage label text at 0% damage.
     */
    private Colour LOW_DAMAGE_COLOUR(255, 255, 255, 255);

    /**
     * The colour of the damage label text at 100% damage.
     */
    private Colour HIGH_DAMAGE_COLOUR(200, 0, 0, 255);

    /**
     * The colour of the damage label text as 200% or more damage.
     */
    private Colour VERY_HIGH_DAMAGE_COLOUR(150, 0, 0, 255);
}
