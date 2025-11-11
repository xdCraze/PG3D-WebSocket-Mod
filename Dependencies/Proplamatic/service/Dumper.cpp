#include <iostream>
#include <sstream>
#include <vector>
#include <streambuf>
#include <fstream>

#include "Dumper.hpp"
#include "../IL2CPP_DEFS.hpp"

static int g_classCount = 0;

int GetTotalClassCount()
{
    return g_classCount;
}

namespace IL2CPP
{
    namespace Dumper
    {
        bool is_obfuscated(std::string& str)
        {
            for (unsigned char ch : str)
            {
                if (static_cast<unsigned>(ch) > 127)
                {
                    return true;
                }
            }
            return false;
        }

        std::string GenerateMethodPattern(IL2CPP::MethodInfo* method)
        {
            std::stringstream outPut;
            std::string methodTypeName = method->GetReturnClass()->GetName();
            std::string methodName = method->GetName();
            auto modifier = method->GetFlagsAsString();
            outPut << "{" << "\"" << modifier << "\", ";
            if (is_obfuscated(methodTypeName))
            {
                outPut << "nullptr" << ", ";
            }
            else
            {
                outPut << "\"" << methodTypeName << "\", ";
            }
            if (is_obfuscated(methodName))
            {
                outPut << "nullptr" << ", {";
            }
            else
            {
                outPut << "\"" << methodName << "\", {";
            }
            auto param_count = method->GetParametersCount();
            bool first = true;
            for (size_t i = 0; i < param_count; ++i)
            {
                auto param = method->GetParameterType(i)->GetClass();
                std::string paramTypeName = param->GetName();
                if (param->IsEnum())
                {
                    outPut << (first ? "\"ENUM\"" : ", \"ENUM\"");
                    first = false;
                }
                else if (is_obfuscated(paramTypeName))
                {
                    outPut << (first ? "nullptr" : ", nullptr");
                    first = false;
                }
                else
                {
                    outPut << (first ? "\"" : ", \"") << paramTypeName << "\"";
                    first = false;
                }
            }
            outPut << "}};";
            return outPut.str();
        }

        std::string GenerateFieldPattern(Class* klass)
        {
            std::stringstream outPut;
            outPut << "\t// Field Pattern: {";
            for (uint16_t i = 0; i < klass->FieldCount(); i++)
            {
                auto fieldClass = klass->GetField(i)->GetType()->GetClass();
                std::string name = fieldClass->GetName();
                if (!is_obfuscated(name))
                {
                    outPut << "\"" << name << "\"" << (i < klass->FieldCount() - 1 ? ", " : "");
                }
                else
                {
                    outPut << "nullptr" << (i < klass->FieldCount() - 1 ? ", " : "");
                }
            }
            outPut << "};" << std::endl << std::endl;
            return outPut.str();
        }

        static std::string EscapeString(const std::string& str) {
            std::string escaped;
            for (char c : str) {
                if (c == '\\') escaped += "\\\\";
                else if (c == '"') escaped += "\\\"";
                else escaped += c;
            }
            return escaped;
        }

        // thanks dari
        static std::string GenerateField(Class* klass) {
            std::ostringstream oss;
            oss << "\t/* Fields */\n";
            bool isEnum = klass->IsEnum();

            for (size_t i = 0; i < klass->FieldCount(); ++i) {
                auto* field = klass->GetField(i);
                int attrs = field->GetFlags();
                int access = attrs & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;

                oss << "\t";
                switch (access) {
                case FIELD_ATTRIBUTE_PRIVATE: oss << "private "; break;
                case FIELD_ATTRIBUTE_PUBLIC: oss << "public "; break;
                case FIELD_ATTRIBUTE_FAMILY: oss << "protected "; break;
                case FIELD_ATTRIBUTE_ASSEMBLY:
                case FIELD_ATTRIBUTE_FAM_AND_ASSEM: oss << "internal "; break;
                case FIELD_ATTRIBUTE_FAM_OR_ASSEM: oss << "protected internal "; break;
                default: break;
                }

                if (attrs & FIELD_ATTRIBUTE_LITERAL)
                    oss << "const ";
                else {
                    if (attrs & FIELD_ATTRIBUTE_STATIC) oss << "static ";
                    if (attrs & FIELD_ATTRIBUTE_INIT_ONLY) oss << "readonly ";
                }

                auto* fieldClass = field->GetType()->GetClass();
                auto fieldTypeName = fieldClass->GetName();
                oss << fieldTypeName << " " << field->GetName();

                if (field->IsLiteral()) {
                    if (isEnum)
                        oss << std::format(" = {}", field->GetValue<uint64_t>(nullptr));
                    else if (fieldTypeName == "String") {
                        auto data = field->GetValue<IL2CPP::String*>(nullptr);
                        if (data) oss << std::format(" = \"{}\"", EscapeString(data->ToString().c_str()));
                    }
                    else if (fieldTypeName == "Boolean")
                        oss << std::format(" = {}", field->GetValue<bool>(nullptr) ? "true" : "false");
                    else if (fieldTypeName == "Int32")
                        oss << std::format(" = {}", field->GetValue<int32_t>(nullptr));
                    else if (fieldTypeName == "UInt32")
                        oss << std::format(" = {}u", field->GetValue<uint32_t>(nullptr));
                    else if (fieldTypeName == "Int64")
                        oss << std::format(" = {}L", field->GetValue<int64_t>(nullptr));
                    else if (fieldTypeName == "UInt64")
                        oss << std::format(" = {}UL", field->GetValue<uint64_t>(nullptr));
                    else if (fieldTypeName == "Single")
                        oss << std::format(" = {:.8f}f", field->GetValue<float>(nullptr));
                    else if (fieldTypeName == "Double")
                        oss << std::format(" = {:.16f}", field->GetValue<double>(nullptr));
                }

                oss << std::format("; // Index: 0x{:X} | Offset: 0x{:X}\n", i, field->GetOffset());
            }

            return oss.str();
        }

        std::string GenerateMethods(Class* klass, const DumperSettings& settings)
        {
            std::stringstream outPut;
            outPut << "\n\t/* Methods */\n";
            for (size_t index = 0; index < klass->MethodCount(); index++)
            {
                MethodInfo* method = klass->GetMethod(index);
                if (method->GetPointer())
                {
                    if (settings.generatePattern)
                    {
                        outPut << "\t// Signature Pattern: " << GenerateMethodPattern(method) << "\n";
                    }
                    outPut << "\t// Method Number: " << index;
                    outPut << " | Index: 0x";
                    outPut << std::hex << index;
                    outPut << " | Offset: 0x" << std::hex << (uint64_t)method->GetPointer() - (uint64_t)GetModuleHandleA("GameAssembly.dll");
                    outPut << " | VA: 0x";
                    outPut << std::hex << (uint64_t)method->GetPointer();
                }
                else
                {
                    outPut << "\t// Method Number: " << index << " | Offset: null | VA: null";
                }
                outPut << "\n\t";
                outPut << method->GetFlagsAsString() << " ";
                Type* returnType = method->GetReturnType();
                if (returnType->IsPassByReference())
                {
                    outPut << "ref ";
                }
                Class* returnClass = returnType->GetClass();
                std::string returnTypeName = settings.generateFullnameTyping ? returnClass->GetFullName() : returnClass->GetName();
                outPut << returnTypeName << " " << method->GetName() << "(";
                size_t paramCount = method->GetParametersCount();
                for (uint32_t index = 0; index < paramCount; index++)
                {
                    Type* param = method->GetParameterType(index);
                    auto attrs = param->GetAttributes();
                    if (param->IsPassByReference())
                    {
                        if (attrs & PARAM_ATTRIBUTE_OUT && !(attrs & PARAM_ATTRIBUTE_IN))
                        {
                            outPut << "out ";
                        }
                        else if (attrs & PARAM_ATTRIBUTE_IN && !(attrs & PARAM_ATTRIBUTE_OUT))
                        {
                            outPut << "in ";
                        }
                        else
                        {
                            outPut << "ref ";
                        }
                    }
                    else
                    {
                        if (attrs & PARAM_ATTRIBUTE_IN)
                        {
                            outPut << "[In] ";
                        }
                        if (attrs & PARAM_ATTRIBUTE_OUT)
                        {
                            outPut << "[Out] ";
                        }
                    }
                    std::string paramTypeName = settings.generateFullnameTyping ? param->GetClass()->GetFullName() : param->GetClass()->GetName();
                    outPut << paramTypeName << " " << method->GetParameterName(index) << ", ";
                }
                if (paramCount > 0)
                {
                    outPut.seekp(-2, outPut.cur);
                }
                outPut << ") { }" << std::endl << std::endl;
            }
            return outPut.str();
        }

        std::string GenerateType(IL2CPP::Type* type, const DumperSettings& settings)
        {
            std::stringstream outPut;
            g_classCount++;
            outPut << "// Class Number: " << g_classCount << std::endl;
            Class* klass = type->GetClass();
            outPut << "// Namespace: " << (klass->GetNamespace().empty() ? "none" : klass->GetNamespace()) << "\n";
            outPut << "// Field Count: " << klass->FieldCount() << " | ";
            outPut << "Method Count: " << klass->MethodCount() << std::endl;
            uint32_t flags = klass->GetFlags();
            if (flags & TYPE_ATTRIBUTE_SERIALIZABLE)
            {
                outPut << "[Serializable]" << std::endl;
            }
            bool isValueType = klass->IsValueType();
            bool isEnum = klass->IsEnum();
            uint32_t visibility = flags & TYPE_ATTRIBUTE_VISIBILITY_MASK;
            switch (visibility)
            {
            case TYPE_ATTRIBUTE_PUBLIC:
            case TYPE_ATTRIBUTE_NESTED_PUBLIC: outPut << "public "; break;
            case TYPE_ATTRIBUTE_NOT_PUBLIC:
            case TYPE_ATTRIBUTE_NESTED_FAM_AND_ASSEM:
            case TYPE_ATTRIBUTE_NESTED_ASSEMBLY: outPut << "internal "; break;
            case TYPE_ATTRIBUTE_NESTED_PRIVATE: outPut << "private "; break;
            case TYPE_ATTRIBUTE_NESTED_FAMILY: outPut << "protected "; break;
            case TYPE_ATTRIBUTE_NESTED_FAM_OR_ASSEM: outPut << "protected internal "; break;
            }
            if (flags & TYPE_ATTRIBUTE_ABSTRACT && flags & TYPE_ATTRIBUTE_SEALED)
            {
                outPut << "static ";
            }
            else if (!(flags & TYPE_ATTRIBUTE_INTERFACE) && flags & TYPE_ATTRIBUTE_ABSTRACT)
            {
                outPut << "abstract ";
            }
            else if (!isValueType && !isEnum && flags & TYPE_ATTRIBUTE_SEALED)
            {
                outPut << "sealed ";
            }
            if (flags & TYPE_ATTRIBUTE_INTERFACE)
            {
                outPut << "interface ";
            }
            else if (isEnum)
            {
                outPut << "enum ";
            }
            else if (isValueType)
            {
                outPut << "struct ";
            }
            else
            {
                outPut << "class ";
            }
            outPut << klass->GetName();
            std::vector<std::string> extends;
            Class* parent = klass->GetParentClass();
            if (!isValueType && !isEnum && parent)
            {
                Type* parent_type = parent->GetType();
                if (parent_type->GetTypingType() != IL2CPP::IMPORT::Il2CppTypeEnum::IL2CPP_TYPE_OBJECT)
                {
                    extends.emplace_back(settings.generateFullnameTyping ? parent->GetFullName() : parent->GetName());
                }
            }
            for (size_t i = 0; i < klass->InterfaceCount(); i++)
            {
                extends.emplace_back(settings.generateFullnameTyping ? klass->GetInterface(i)->GetFullName() : klass->GetInterface(i)->GetName());
            }
            if (!extends.empty())
            {
                outPut << " : " << extends[0];
                for (int i = 1; i < extends.size(); ++i)
                {
                    outPut << ", " << extends[i];
                }
            }
            outPut << std::endl << "{" << std::endl;
            if (settings.generatePattern)
            {
                outPut << GenerateFieldPattern(klass);
            }
            outPut << GenerateField(klass);
            outPut << GenerateMethods(klass, settings);
            outPut << "}" << std::endl << std::endl;
            return outPut.str();
        }

        std::string DumpImage(Image* imageToDump, const DumperSettings& settings)
        {
            if (imageToDump == nullptr)
            {
                throw Exception::NullArgument(IL2CPP_NAMEOF(imageToDump));
            }
            std::stringstream imageOutput;
            for (size_t i = 0; i < imageToDump->ClassCount(); i++)
            {
                const Class* klass = imageToDump->GetClass(i);
                Type* type = klass->GetType();
                imageOutput << "// DLL Name: " << imageToDump->GetName() << std::endl;
                imageOutput << GenerateType(type, settings);
            }
            return imageOutput.str();
        }

        void StartDumper(const std::string& dumpDirectoryPath, const DumperSettings& settings)
        {
            const DWORD pathAttribute = GetFileAttributesA(dumpDirectoryPath.c_str());
            if (pathAttribute == INVALID_FILE_ATTRIBUTES || (pathAttribute & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                CreateDirectoryA(dumpDirectoryPath.c_str(), nullptr);
            }

            std::stringstream merged;

            for (Image* image : IL2CPP::CurrentDomain()->OpenEveryAssembly())
            {
                g_classCount = 0;
                std::string out = DumpImage(image, settings);

                if (settings.mergeDumpFiles)
                {
                    merged << out << "\n";
                }
                else
                {
                    auto outPath = std::string(dumpDirectoryPath).append("\\").append(image->GetName()).append(".cs");
                    FILE* file = std::fopen(outPath.c_str(), "w");
                    if (file == nullptr)
                    {
                        return;
                    }
                    std::fwrite(out.c_str(), out.size(), 1, file);
                    std::fclose(file);
                }
            }

            if (settings.mergeDumpFiles)
            {
                auto mergedPath = std::string(dumpDirectoryPath).append("\\Dump.cs");
                FILE* mergedFile = std::fopen(mergedPath.c_str(), "w");
                if (mergedFile)
                {
                    std::string mergedContent = merged.str();
                    std::fwrite(mergedContent.c_str(), mergedContent.size(), 1, mergedFile);
                    std::fclose(mergedFile);
                }
            }
        }
    }
}