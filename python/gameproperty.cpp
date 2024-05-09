#define GAME_PROPERTY_$(cc, ac, gp`, p$, t$, o$, d$, e$`, d, e, a) class cc { \
    std::string _scriptName; \
public: \
	inline static const std::string type = ac; \
	inline static const std::string global_property = gp; \
    inline static const std::array<std::string, $> fields = { `#p$, `}; \
    cc(const std::string& scriptName, engine::json& j, engine::logger& logger, const std::shared_ptr<engine::scripts>& scripts) : \
        `p$(j, scriptName, logger, scripts), `\
        _scriptName(scriptName) { e } \
	static void Register(asIScriptEngine* engine, const std::shared_ptr<DocumentationGenerator>& document) { \
        d \
		engine->RegisterObjectMethod(ac, "const string& scriptName() const", \
			asMETHOD(cc, scriptName), asCALL_THISCALL); \
        `p$##_::Register(engine, document); `\
    } \
	inline const std::string& scriptName() const { return _scriptName; } \
    `PROPERTY(cc, ac, p$, t$, o$, d$, e$) `\
    inline static bool hasField(const std::string_view field) { \
        return `field == #p$ || `false; \
    } \
	inline static std::string getFieldAngelScriptType(const std::string_view field) { \
		`if (field == #p$) return engine::script_type<t$>(); `\
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
    static bool isFieldOverrideable(const std::string& field) { \
        `if (field == #p$) return !engine::script_type<t$>().empty(); `\
        return false; \
    } \
    std::any getFieldDefaultValue(const std::string& field) { \
        `if (field == #p$) return p$(); `\
        return {}; \
    } \
    void setFieldValue(const std::string& field, const std::any& value, const awe::overrides& overrides) { \
        `if (field == #p$) p$(overrides) = std::any_cast<t$>(value); `\
    } \
    a \
};

#define GAME_PROPERTY_VIEW_$(ns, cc, ac`, p$, t$`) namespace ns { \
    class cc##_view : public engine::script_reference_type<ns::cc##_view> { \
        mutable engine::logger _logger; \
        std::shared_ptr<const awe::bank<cc>> _bank; \
    public: \
        std::string scriptName; \
        awe::overrides overrides; \
        inline cc##_view(const engine::logger::data& data, const std::shared_ptr<const awe::banks>& banks, const std::string& name = "") : _logger(data), _bank(banks->get<cc>()), scriptName(name) {} \
        inline cc##_view(const cc##_view& c) : _logger(c._logger.getData()), _bank(c._bank), scriptName(c.scriptName), overrides(c.overrides) {} \
        static inline cc##_view* Create(const engine::logger::data& data, const std::shared_ptr<const awe::banks>& banks, const std::string& name = "") { return new cc##_view(data, banks, name); } \
        static void Register(asIScriptEngine* engine, const std::shared_ptr<DocumentationGenerator>& document) { \
            if (engine->GetTypeInfoByName(ac "View")) return; \
            RegisterType(engine, ac "View", [](asIScriptEngine* engine, const std::string& type) { /* No factory function. */ }); \
            engine->RegisterObjectProperty(ac "View", "string scriptName", asOFFSET(ns::cc##_view, scriptName)); \
            engine->RegisterObjectProperty(ac "View", "Overrides overrides", asOFFSET(ns::cc##_view, overrides)); \
            engine->RegisterObjectMethod(ac "View", "bool isScriptNameValid() const", asMETHOD(ns::cc##_view, isScriptNameValid), asCALL_THISCALL); \
            `engine->RegisterObjectMethod(ac "View", std::string(awe::bank_return_type<t$>()).append(" " #p$ "() const").c_str(), asMETHOD(ns::cc##_view, p$), asCALL_THISCALL); £\
            `\
        } \
        inline bool isScriptNameValid() const { return _bank->contains(scriptName); } \
        `inline typename boost::call_traits<t$>::const_reference p$() const { \
            if (isScriptNameValid()) { \
                return (*_bank)[scriptName]->p$(overrides); \
            } else { \
                _logger.error("\"{}\" view has incorrect script name \"{}\": returning default constructed value.", ac, scriptName); \
                return {}; \
            } \
        } £\
        `\
    }; \
}
