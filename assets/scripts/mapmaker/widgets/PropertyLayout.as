/**
 * @file PropertyLayout.as
 * Defines property layout widget types.
 */

/**
 * Defines constants used within the property layout widget classes.
 */
namespace PropertyLayoutConstants {
    /**
     * The space between widgets within some of the property layouts.
     */
    const float SpaceBetweenWidgets = 5.0;
}

/**
 * Defines the property layout widget type.
 * A property layout is a horizontal layout with the following widgets:
 * <ol><li>Centred label with custom static text.</li>
 *     <li>Edit box that's centrally aligned and given the UINT validator.</li>
 *     <li>Centred label with "~/" text.</li>
 *     <li>Centred label with custom dynamic text.</li></ol>
 * This widget type is primarily used within the \c TilePropertiesWindow class.
 */
class PropertyLayout : HorizontalLayout {
    /**
     * Sets up the property layout.
     * @param staticText The text to assign to the static label.
     */
    PropertyLayout(const string&in staticText) {
        setupWidget(@staticLabelGroup, @staticLabel);
        staticLabel.setText(staticText);
        setupWidget(@editBoxGroup, @editBox);
        editBox.setSize("80%", "25");
        editBox.setValidator(VALIDATOR_UINT);
        editBox.setAlignment(EditBoxAlignment::Centre);
        setupWidget(@slashLabelGroup, @slashLabel);
        slashLabel.setText("~/");
        setupWidget(@dynamicLabelGroup, @dynamicLabel);
        add(staticLabelGroup);
        add(editBoxGroup);
        add(slashLabelGroup);
        add(dynamicLabelGroup);
    }

    /**
     * Connects a handler to the edit box's \c ReturnOrUnfocused signal.
     * @param returnOrUnfocused Points to the handler to connect. \c null to
     *                          disconnect.
     */
    void connectEditBox(SingleSignalHandler@ const returnOrUnfocused) {
        editBox.connect(ReturnOrUnfocused, returnOrUnfocused);
    }

    /**
     * Connects a multi signal handler to the edit box.
     * Only useful when you have a collection of property layouts that carry out
     * the same function and you need to be able to tell which layout is emitting
     * the \c ReturnOrUnfocused signal. Otherwise, use the \c SingleSignalHandler
     * version.
     * @param handler The handler to connect.
     */
    void connectEditBoxMulti(MultiSignalHandler@ const handler) explicit {
        editBox.connect(handler);
    }

    /**
     * Updates the static label.
     * Really wanted to avoid this but UnitPropertiesPanel::ammoLayouts has forced
     * my hand.
     * @param text The text to set.
     */
    void setStaticText(const string&in text) {
        staticLabel.setText(text);
    }

    /**
     * Updates the edit box's text.
     * @param value The value to set as the edit box's text.
     */
    void setEditBoxText(const uint64 value) {
        editBox.setText(formatUInt(value));
    }

    /**
     * Updates the edit box's text.
     * @param text The text to set to the edit box.
     */
    void setEditBoxText(const string&in text) {
        editBox.setText(text);
    }

    /**
     * Retrieves the text that is currently in the edit box.
     * @return The text parsed into an unsigned int.
     */
    uint64 getEditBoxText() const {
        return parseUInt(editBox.getText());
    }

    /**
     * Enables or disables the edit box.
     * @param enabled \c TRUE to enable, \c FALSE to disable.
     */
    void setEditBoxEnabled(const bool enabled) {
        editBox.setEnabled(enabled);
    }

    /**
     * Update's the dynamic label's untranslated text.
     * @param value The value to assign directly (without translation) to the
     *              dynamic label's text.
     */
    void setDynamicText(const uint64 value) {
        dynamicLabel.setText("~" + formatUInt(value));
    }

    /**
     * Update's the dynamic label's translated text.
     * @param text The value to assign to the dynamic label's text.
     */
    void setDynamicText(const string&in text) {
        dynamicLabel.setText(text);
    }

    /**
     * Sets up a label/edit box widget in this layout.
     * @param group Points to the group to add the widget to.
     * @param w     Points to the widget to setup.
     * @param text  The text to assign to the label. Is empty, no text will be
     *              assigned.
     */
    private void setupWidget(Group@ const group, Widget@ const w) {
        w.setOrigin(0.5, 0.5);
        w.setPosition("50%", "50%");
        group.add(w);
    }

    /**
     * The group containing the static label.
     */
    private Group staticLabelGroup;

    /**
     * The static label.
     * Used to describe the property.
     */
    private Label staticLabel;

    /**
     * The group containing the edit box.
     */
    private Group editBoxGroup;

    /**
     * The edit box.
     */
    private EditBox editBox;

    /**
     * The group containing the "~/" label.
     */
    private Group slashLabelGroup;

    /**
     * The "~/" label.
     */
    private Label slashLabel;

    /**
     * The group containing the dynamic label.
     */
    private Group dynamicLabelGroup;

    /**
     * The dynamic label.
     * Used to tell the user what the property's maximum value is.
     */
    private Label dynamicLabel;
}

/**
 * Defines the toggle property layout widget type.
 * This layout has the following widgets:
 * <ol><li>Centre right-aligned label with custom static text.</li>
 *     <li>Centre left-aligned checkbox with no text.</li></ol>
 * This widget type is primarily used within the \c TilePropertiesWindow class.
 */
class TogglePropertyLayout : HorizontalLayout {
    /**
     * Sets up the toggle property layout.
     * @param text Text to assign to the static label.
     */
    TogglePropertyLayout(const string&in text) {
        label.setOrigin(1.0, 0.5);
        label.setPosition("100%", "50%");
        label.setText(text);
        checkBox.setOrigin(0.0, 0.5);
        checkBox.setPosition("0%", "50%");
        labelGroup.add(label);
        checkBoxGroup.add(checkBox);
        add(labelGroup);
        add(checkBoxGroup);
        setSpaceBetweenWidgets(PropertyLayoutConstants::SpaceBetweenWidgets);
    }

    /**
     * Connects a handler to the checkbox's \c Changed signal.
     * @param onChanged Points to the handler to connect. \c null to disconnect.
     */
    void connectCheckBox(SingleSignalHandler@ const onChanged) {
        checkBox.connect(Changed, onChanged);
    }

    /**
     * Updates the checked state of the checkbox.
     * @param checked \c TRUE to check, \c FALSE to uncheck.
     */
    void setChecked(const bool checked) {
        checkBox.setChecked(checked);
    }

    /**
     * Get the checked state of the checkbox.
     * @return \c TRUE if checked, \c FALSE if unchecked.
     */
    bool getChecked() const {
        return checkBox.getChecked();
    }

    /**
     * The group containing the label.
     */
    private Group labelGroup;

    /**
     * The static label.
     */
    private Label label;

    /**
     * The group containing the checkbox.
     */
    private Group checkBoxGroup;

    /**
     * The checkbox.
     */
    private CheckBox checkBox;
}

/**
 * Defines the read-only property layout widget type.
 * This layout has the following widgets:
 * <ol><li>Centre right-aligned label with custom static text.</li>
 *     <li>Centre left-aligned label with custom dynamic text.</li></ol>
 * This widget type is primarily used within the \c TilePropertiesWindow class.
 */
class ReadOnlyPropertyLayout : HorizontalLayout {
    /**
     * Sets up the read-only property layout.
     * @param text The text to assign to the static label.
     */
    ReadOnlyPropertyLayout(const string&in text) {
        staticLabel.setOrigin(1.0, 0.5);
        staticLabel.setPosition("100%", "50%");
        staticLabel.setText(text);
        dynamicLabel.setOrigin(0.0, 0.5);
        dynamicLabel.setPosition("0%", "50%");
        staticLabelGroup.add(staticLabel);
        dynamicLabelGroup.add(dynamicLabel);
        add(staticLabelGroup);
        add(dynamicLabelGroup);
        setSpaceBetweenWidgets(PropertyLayoutConstants::SpaceBetweenWidgets);
    }

    /**
     * Updates the dynamic label's text.
     * @param text The text to apply.
     * @param vars The variables to insert into the text, if any.
     */
    void setText(const string&in text, array<any>@ const vars = null) {
        dynamicLabel.setText(text, vars);
    }

    /**
     * The group containing the static label.
     */
    private Group staticLabelGroup;

    /**
     * The static label.
     */
    private Label staticLabel;

    /**
     * The group containing the dynamic label.
     */
    private Group dynamicLabelGroup;

    /**
     * The dynamic label.
     */
    private Label dynamicLabel;
}
