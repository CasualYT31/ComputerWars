/**
 * @file FileDialog.as
 * Defines the \c FileDialog class.
 */

/**
 * Represents a file dialog in a GUI.
 */
shared class FileDialog : ChildWindow {
    /**
     * Create the file dialog.
     */
    FileDialog() {
        super(FileDialogEngineName);
    }

    void setStrings(const string&in s1, array<any>@ const v1,
        const string&in s2, array<any>@ const v2,
        const string&in s3, array<any>@ const v3,
        const string&in s4, array<any>@ const v4,
        const string&in s5, array<any>@ const v5,
        const string&in s6, array<any>@ const v6,
        const string&in s7, array<any>@ const v7,
        const string&in s8, array<any>@ const v8,
        const string&in s9, array<any>@ const v9) {
        setFileDialogStrings(this,
            s1, v1,
            s2, v2,
            s3, v3,
            s4, v4,
            s5, v5,
            s6, v6,
            s7, v7,
            s8, v8,
            s9, v9
        );
    }

    void setPath(const string&in p) {
        setFileDialogPath(this, p);
    }

    void mustExist(const bool m) {
        setFileDialogFileMustExist(this, m);
    }

    void addFilter(const string&in n, array<any>@ const v,
        array<string>@ const f) {
        addFileDialogFileTypeFilter(this, n, v, f);
    }

    void setDefaultFilter(const uint64 i) {
        setFileDialogDefaultFileFilter(this, i);
    }

    array<string>@ getSelectedPaths() const {
        return getFileDialogSelectedPaths(this);
    }
}
