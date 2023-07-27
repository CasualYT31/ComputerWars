/**
 * @file Helper.as
 * Contains helper functions.
 */

namespace awe {
	/**
	 * Writes a dictionary to the logs, assuming all of its values are strings.
	 * @param dict Handle to the dictionary to log.
	 */
	void logDictionary(const dictionary@ dict) {
		info("Dictionary size: " + formatUInt(dict.getSize()));
		const auto keys = dict.getKeys();
		for (uint i = 0, len = keys.length(); i < len; ++i) {
			string value;
			dict.get(keys[i], value);
			info(keys[i] + " = " + value);
		}
	}

    /**
     * Formats a \c MenuItemID.
     * @param id The menu item ID to format.
     */
    string formatMenuItemID(const MenuItemID id) {
        return formatUInt(id);
    }
}
