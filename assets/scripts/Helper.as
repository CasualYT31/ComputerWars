/**
 * @file Helper.as
 * Contains helper functions.
 */

namespace awe {
    /**
     * Splits a string into an array of smaller strings, separated by the given
     * delimiter.
     * The delimiter will not be included in any of the substrings. If an empty
     * \c str is given, an empty array will be returned. If an empty \c delimiter
     * is given, an array with one element will be returned. The element's value
     * will be equal to \c str.
     * @remark This is documented to be a part of the string library in
     *         AngelScript, but it just doesn't appear to be registered at all...
     * @param  str       The string to split.
     * @param  delimiter The string sequence which, if found in \c str, will cause
     *                   a split.
     * @return An array of each split string. The order of the characters will be
     *         retained.
     */
    shared array<string>@ split(const string&in str, const string&in delimiter) {
        array<string>@ arr = array<string>();
        int lastPos = 0, delimiterPos = str.findFirst(delimiter);
        while (delimiterPos >= 0) {
            arr.insertLast(str.substr(lastPos, delimiterPos - lastPos));
            lastPos = delimiterPos + delimiter.length();
            delimiterPos = str.findFirst(delimiter, lastPos);
        }
        if (uint64(lastPos) < str.length()) arr.insertLast(str.substr(lastPos));
        return arr;
    }

    /**
     * Concatenate a list of strings into one big string, separated by the given
     * delimiter.
     * @remark This is documented to be a part of the string library in
     *         AngelScript, but it just doesn't appear to be registered at all...
     * @param  arr       The array of strings to join together.
     * @param  delimiter The extra string sequence to append to each string in
     *                   \c arr before joining (the original strings are not
     *                   changed).
     * @return The result. An empty string is \c arr is \c null.
     */
    shared string join(const array<string>@ const arr, const string&in delimiter) {
        if (arr is null) return "";
        string res;
        for (uint64 i = 0, len = arr.length(); i < len; ++i) {
            res += arr[i];
            if (i < len - 1) res += delimiter;
        }
        return res;
    }

    /**
     * Retrieves the edit map's current environment spritesheet.
     * @return The edit map's current environment spritesheet, or \c "tile.normal"
     *         if an edit map is not currently open.
     */
    string getEditMapEnvironmentSpritesheet() {
        return edit is null ? "tile.normal" : edit.map.getEnvironmentSpritesheet();
    }

    /**
     * Retrieves the edit map's current environment tile picture spritesheet.
     * @return The edit map's current environment tile picture spritesheet, or
     *         \c "tilePicture.normal" if an edit map is not currently open.
     */
    string getEditMapEnvironmentPictureSpritesheet() {
        return edit is null ? "tilePicture.normal" :
            edit.map.getEnvironmentPictureSpritesheet();
    }

    /**
     * Retrieves the edit map's current environment structure icon spritesheet.
     * @return The edit map's current environment structure icon spritesheet, or
     *         \c "structure" if an edit map is not currently open.
     */
    string getEditMapEnvironmentStructureIconSpritesheet() {
        return edit is null ? "structure" :
            edit.map.getEnvironmentStructureIconSpritesheet();
    }
}
