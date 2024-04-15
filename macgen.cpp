#define GAME_PROPERTY_$(cc, ac, gp, i`, p$, t$, e$`, e, a) class cc { \
    std::string _scriptName; \
public: \
	static constexpr char* const type = ac; \
	static constexpr char* const global_property = gp; \
    static constexpr std::array<const char* const, $> fields = { `#p$, `}; \
    static constexpr std::size_t overrideID = i; \
    cc(const std::string& scriptName, engine::json& j, engine::logger& logger) : \
        `p$(j, logger), `\
        _scriptName(scriptName) { e } \
	static void Register(asIScriptEngine* engine) { \
		engine->RegisterObjectMethod(ac, "const string& scriptName() const", \
			asMETHOD(cc, scriptName), asCALL_THISCALL); \
        `p$##_::Register(engine); `\
    } \
	inline const std::string& scriptName() const { return _scriptName; } \
    `PROPERTY(cc, ac, p$, t$, i, e$) `\
    static constexpr bool hasField(const std::string_view field) { \
        return `field == #p$ || `false; \
    } \
	static constexpr const char* const getFieldAngelScriptType(const std::string_view field) { \
		`if (field == #p$) return awe::AngelScriptOverrideType<t$>::value; `\
		return ""; \
	} \
	static std::any readFieldOverrideVariable(const std::string& field, \
		const std::shared_ptr<engine::scripts>& scripts, \
		const asUINT variable) { \
		`if (field == #p$) return awe::OverrideVariable<t$>::read(scripts, variable); `\
		return {}; \
	} \
	static std::any readFieldOverrideFunction(const std::string& field, \
		const std::shared_ptr<engine::scripts>& scripts, \
		asIScriptFunction* const function, const std::any& parent) { \
		`if (field == #p$) return awe::OverrideFunction<t$>::read(scripts, function, parent); `\
		return {}; \
	} \
    std::any getFieldDefaultValue(const std::string& field) { \
        `if (field == #p$) return p$(); `\
        return {}; \
    } \
    void setFieldValue(const std::string& field, const std::any& value, const awe::overrides& overrides) { \
        `if (field == #p$) p$(overrides) = std::any_cast<t$>(value); ` \
    } \
    a \
};
