/**
 * @file MapProperties.as
 * Defines code that manages the map properties child window.
 */

namespace map_properties_internal {
    /**
     * Stores the name of the map properties \c ChildWindow.
     */
    string WINDOW;
}

/**
 * Sets up the map properties child window.
 * @param parent The parent of the child window.
 */
void MapPropertiesSetUp(const string&in parent = "") {
    map_properties_internal::WINDOW =
        parent + (parent.isEmpty() ? "" : ".") + "SetMapProperties";
    addWidget("ChildWindow", map_properties_internal::WINDOW);
    setWidgetText(map_properties_internal::WINDOW, "mapprops");
    setWidgetSize(map_properties_internal::WINDOW, "250px", "175px");
    setWidgetResizable(map_properties_internal::WINDOW, false);
    setChildWindowTitleButtons(map_properties_internal::WINDOW,
        TitleButton::Close);

    const auto baseLayout = map_properties_internal::WINDOW + ".BaseLayout";
    addWidget("HorizontalLayout", baseLayout);
    setWidgetSize(baseLayout, "100%", "100%-30px");
    setGroupPadding(baseLayout, "5px");

    const auto labelLayout = baseLayout + ".LabelLayout";
    addWidget("VerticalLayout", labelLayout);
    setWidgetRatioInLayout(baseLayout, 0, 0.2f);

    const auto nameLabelGroup = labelLayout + ".MapNameLabelGroup";
    addWidget("Group", nameLabelGroup);

    const auto nameLabel = nameLabelGroup + ".MapNameLabel";
    addWidget("Label", nameLabel);
    setWidgetText(nameLabel, "name");
    setWidgetOrigin(nameLabel, 1.0f, 0.5f);
    setWidgetPosition(nameLabel, "100%", "50%");

    const auto dayLabelGroup = labelLayout + ".DayLabelGroup";
    addWidget("Group", dayLabelGroup);

    const auto dayLabel = dayLabelGroup + ".DayLabel";
    addWidget("Label", dayLabel);
    setWidgetText(dayLabel, "daylabel");
    setWidgetOrigin(dayLabel, 1.0f, 0.5f);
    setWidgetPosition(dayLabel, "100%", "50%");

    const auto sizeLabelGroup = labelLayout + ".SizeLabelGroup";
    addWidget("Group", sizeLabelGroup);

    const auto sizeLabel = sizeLabelGroup + ".SizeLabel";
    addWidget("Label", sizeLabel);
    setWidgetText(sizeLabel, "size");
    setWidgetOrigin(sizeLabel, 1.0f, 0.5f);
    setWidgetPosition(sizeLabel, "100%", "50%");

    const auto editLayout = baseLayout + ".EditLayout";
    addWidget("VerticalLayout", editLayout);

    const auto nameEditGroup = editLayout + ".MapNameEditGroup";
    addWidget("Group", nameEditGroup);
    setGroupPadding(nameEditGroup, "5px", "20%", "5px", "20%");

    const auto nameEdit = nameEditGroup + ".MapNameEdit";
    addWidget("EditBox", nameEdit);
    setWidgetSize(nameEdit, "100%", "100%");

    const auto dayEditGroup = editLayout + ".DayEditGroup";
    addWidget("Group", dayEditGroup);
    setGroupPadding(dayEditGroup, "5px", "20%", "5px", "20%");

    const auto dayEdit = dayEditGroup + ".DayEdit";
    addWidget("EditBox", dayEdit);
    setWidgetSize(dayEdit, "100%", "100%");

    const auto sizeLayout = editLayout + ".SizeLayout";
    addWidget("HorizontalLayout", sizeLayout);

    const auto widthEditGroup = sizeLayout + ".WidthEditGroup";
    addWidget("Group", widthEditGroup);
    setGroupPadding(widthEditGroup, "5px", "20%", "5px", "20%");

    const auto widthEdit = widthEditGroup + ".WidthEdit";
    addWidget("EditBox", widthEdit);
    setWidgetSize(widthEdit, "100%", "100%");

    const auto xLabelGroup = sizeLayout + ".XGroup";
    addWidget("Group", xLabelGroup);
    setWidgetRatioInLayout(sizeLayout, 1, 0.2f);

    const auto xLabel = xLabelGroup + ".X";
    addWidget("Label", xLabel);
    setWidgetText(xLabel, "~x");
    setWidgetOrigin(xLabel, 0.5f, 0.5f);
    setWidgetPosition(xLabel, "50%", "50%");

    const auto heightEditGroup = sizeLayout + ".HeightEditGroup";
    addWidget("Group", heightEditGroup);
    setGroupPadding(heightEditGroup, "5px", "20%", "5px", "20%");

    const auto heightEdit = heightEditGroup + ".HeightEdit";
    addWidget("EditBox", heightEdit);
    setWidgetSize(heightEdit, "100%", "100%");

    const auto ok = map_properties_internal::WINDOW + ".MapPropertiesOK";
    addWidget("Button", ok);
    setWidgetText(ok, "ok");
    setWidgetOrigin(ok, 1.0f, 1.0f);
    setWidgetPosition(ok, "100%-5px", "100%-5px");
    setWidgetSize(ok, "50px", "25px");

    const auto cancel = map_properties_internal::WINDOW + ".MapPropertiesCancel";
    addWidget("Button", cancel);
    setWidgetText(cancel, "cancel");
    setWidgetOrigin(cancel, 1.0f, 1.0f);
    setWidgetPosition(cancel, "100%-60px", "100%-5px");
    setWidgetSize(cancel, "50px", "25px");

    // CloseMapProperties();
}

/**
 * Opens the map properties dialog window.
 */
void OpenMapProperties() {
    openChildWindow(map_properties_internal::WINDOW, "50px", "35px");
}

/**
 * Closes the map properties dialog window.
 */
void CloseMapProperties() {
    closeChildWindow(map_properties_internal::WINDOW);
}

/**
 * Closes the map properties dialog without applying any changes.
 */
void MapMakerMenu_MapPropertiesCancel_Pressed() {
    CloseMapProperties();
}
