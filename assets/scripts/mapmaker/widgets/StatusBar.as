/**
 * @file StatusBar.as
 * Declares the status bar widget type.
 */

/**
 * A status bar is a panel with a horizontal layout of label widgets.
 */
class StatusBar : Panel {
    /**
     * Define the number of labels in the status bar, and what ratios they should
     * have.
     * @param ratios The ratios each label should have. Each label must be given
     *               an entry in this array.
     */
    StatusBar(const array<float>&in ratios) {
        for (uint64 i = 0, len = ratios.length(); i < len; ++i) {
            groups.insertLast(Group());
            labels.insertLast(Label());
            groups[i].add(labels[i]);
            layout.add(groups[i]);
            layout.setRatioOfWidget(i, ratios[i]);
        }
        add(layout);
    }

    /**
     * Update's a label's text.
     * @param i    The index of the label to update.
     * @param text The text to assign to the label.
     * @param vars Optional list of variables to insert into the text.
     */
    void setLabel(const uint64 i, const string&in text,
        array<any>@ const vars = null) {
        labels[i].setText(text, vars);
    }

    /**
     * Clears the text of each of the status bar's labels.
     */
    void clear() {
        for (uint64 i = 0, len = labels.length(); i < len; ++i)
            labels[i].setText("~");
    }
    
    /**
     * The layout.
     */
    private HorizontalLayout layout;

    /**
     * The groups that parent each label.
     */
    private array<Group@> groups;

    /**
     * The label widgets.
     */
    private array<Label@> labels;
}
