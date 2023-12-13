/**
 * @file DamageWidget.as
 * Declares the damage label widget type.
 */

/**
 * Defines constants that the \c DamageWidget class uses internally.
 */
namespace DamageWidgetConstants {
    /**
     * The colour of the damage label text at 0% damage.
     */
    const Colour LowDamageColour(White);

    /**
     * The colour of the damage label text at 100% damage.
     */
    const Colour HighDamageColour(200, 0, 0, 255);

    /**
     * The colour of the damage label text at 200%+ damage.
     */
    const Colour VeryHighDamageColour(150, 0, 0, 255);
}

/**
 * Used to display damage to the user.
 */
class DamageWidget : Group {
    /**
     * Creates the vertical layout which will hold all of the command buttons.
     * @param menu   The name of the menu this command widget is being added to.
     * @param width  The width to keep the layout at.
     * @param height The height to assign to each command button.
     */
    DamageWidget() {
        // Setup the damage sprite.
        sprite.setName("DS");
        sprite.setSprite("icon", "damagetooltip");

        // Setup the damage label.
        label.setTextColour(Colour(250, 100, 0, 255));
        label.setTextOutlineColour(Black);
        label.setTextOutlineThickness(2.0);
        label.setTextSize(15);
        label.setOrigin(0.5, 0.2);
        // Should favour the Picture that's the direct sibling?
        label.setPosition("DS.left + DS.width / 2", "DS.top + DS.height / 2");

        // Setup the container.
        // Will be (0, 0) upon first iteration of game loop as the animated sprite
        // hasn't been allocated and assigned yet.
        setSize("DS.width", "DS.height");
        add(sprite);
        add(label);
    }

    /**
     * Sets the damage to display on the widget.
     * @param damage The damage to display.
     */
    void setDamage(const int damage) {
        label.setText("~" + formatInt(damage) + "%");
        if (damage <= 100)
            label.setTextColour(DamageWidgetConstants::LowDamageColour.gradientTo(
                DamageWidgetConstants::HighDamageColour, double(damage)));
        else
            label.setTextColour(
                DamageWidgetConstants::HighDamageColour.gradientTo(
                    DamageWidgetConstants::VeryHighDamageColour,
                    double(damage - 100)));
    }

    /**
     * The damage label sprite.
     */
    private Picture sprite;

    /**
     * The damage label.
     */
    private Label label;
}
