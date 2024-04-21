#define DECLARE_POD_$(cc, ac`, t$, p$`) struct cc : public engine::script_reference_type<cc> { \
    static void Register(asIScriptEngine* engine, const std::shared_ptr<DocumentationGenerator>& document); \
    inline static cc* Create() { return new cc(); }; \
    inline static cc* Create(`boost::call_traits<t$>::param_type param$£, `) { \
        return new cc(`param$£, `); \
    } \
    inline static cc* Create(const cc* const o) { return new cc(*o); } \
    cc() = default; \
    cc(`boost::call_traits<t$>::param_type£, `); \
    cc(const cc& o); \
    cc(cc&& o) noexcept; \
    bool operator==(const cc& o) const; \
    inline bool operator!=(const cc& o) const { return !(*this == o); } \
    cc& operator=(const cc& o); \
    cc& operator=(cc&& o) noexcept; \
    `t$ p$; `\
private: \
    inline cc* opAssign(const cc* const o) { return &(*this = *o); } \
    inline bool opEquals(const cc* const o) const { return *this == *o; } \
}; \
template <> \
inline constexpr const char* engine::script_type<cc>() { return ac; }

#define DEFINE_POD_$(cc, ac`, t$, p$`) void cc::Register(asIScriptEngine* engine, const std::shared_ptr<DocumentationGenerator>& document) { \
    if (engine->GetTypeInfoByName(ac)) return; \
    auto r = RegisterType(engine, ac, \
        [](asIScriptEngine* engine, const std::string& type) { \
            engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY, \
                ac "@ f()", \
                asFUNCTIONPR(cc::Create, (), cc*), asCALL_CDECL); \
            engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY, \
                std::string(ac "@ f(")`.append(engine::script_param_type<t$>())£.append(", ")`.append(")").c_str(), \
                asFUNCTIONPR(cc::Create, (`boost::call_traits<t$>::param_type£, `), cc*), asCALL_CDECL); \
            engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY, \
                ac "@ f(const " ac "&in)", \
                asFUNCTIONPR(cc::Create, (const cc* const), cc*), asCALL_CDECL); \
        }); \
    `r = engine->RegisterObjectProperty(ac, std::string(engine::script_type<t$>()).append(" ").append(#p$).c_str(), asOFFSET(cc, p$)); \
    `r = engine->RegisterObjectMethod(ac, ac "@ opAssign(const " ac "&in)", \
        asMETHOD(cc, opAssign), asCALL_THISCALL); \
    r = engine->RegisterObjectMethod(ac, "bool opEquals(const " ac "&in) const", \
        asMETHOD(cc, opEquals), asCALL_THISCALL); \
} \
\
cc::cc(`boost::call_traits<t$>::param_type param$£, `) : `p$(param$)£, ` {} \
\
cc::cc(const cc& o) : `p$(o.p$)£, ` {} \
\
cc::cc(cc&& o) noexcept : `p$(std::move(o.p$))£, ` {} \
\
bool cc::operator==(const cc& o) const { \
    return `engine::isEqual<t$, t$>(p$, o.p$)£ && `; \
} \
\
cc& cc::operator=(const cc& o) { \
    `p$ = o.p$; `\
    return *this; \
} \
\
cc& cc::operator=(cc&& o) noexcept { \
    `p$ = std::move(o.p$); `\
    return *this; \
}
