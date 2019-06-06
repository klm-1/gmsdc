#ifndef GMLEXPRCONTEXT_H
#define GMLEXPRCONTEXT_H

#include <map>
#include <vector>
#include <string>

class ExprContext
{
public:
    enum Type 
	{
        Unknown,
        Any,

        Sprite,
        Sound,
        Background,
        Path,
        Script,
        Shader,
        Font,
        Timeline,
        Object,
        Room,
        AudioGroup,
        TextureGroup,
        File,  // ?
        Extension, // ?

        Bool,
        Color,
        HAlign,
        VAlign,
        AudioFalloff,
        PathAction,
        KeyCode,
        MouseButton,
        GamepadInput,
        AchType,
        AudioType,
        BufferFormat,
        BufferType,
        BufferSeekBase,
        BlendMode,
        BlendModeExt,
        Cursor,
        Primitive,
        Timezone,
        VBMethod,
        CallConv,
        CallArg,
        DataStructure,
        Effect,
        Event,
        FileAttr,
        FbLogin,
        LBSort,
        LBDisplay,
        Matrix,
        NetworkConf,
        NetworkType,
        SteamOverlay,
        EmitterShape,
        EmitterDistr,
        PartShape,
        PhysicsDrawFlag,
        JointProp,
        PhysicsPartTypeFlag,
        PhysicsPartDataFlag,
        PhysicsGroupFlag,
        UGCFiletype,
        UGCQuery,
        UGCMatch,
        UGCList,
        UGCSort,
        UGCVisibility,
        VType,
        VUsage,
    };

    static const auto* FuncArgContext(const std::string& fun)
    {
        auto ret = argContext_.find(fun);
        return ret == argContext_.end() ? nullptr : &ret->second;
    }

    static const char* ValueInContext(int val, Type ctx);

private:
    ExprContext() = delete;

    static const std::map<std::string, std::vector<Type>> argContext_;
    static const std::map<Type, std::map<int, std::string>> contextVal_;
    static const std::map<Type, std::map<int, std::string>> flagVal_;
};

#endif // GMLEXPRCONTEXT_H
