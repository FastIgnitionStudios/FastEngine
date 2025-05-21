#include "EnginePCH.h"
#include "ShaderVK.h"
#include "PipelineVK.h"
#include "shaderc/shaderc.hpp"

#include "Filesystem.h"

static shaderc_shader_kind GetShaderKind(Engine::ShaderType type)
{
    switch (type)
    {
    case Engine::ShaderType::VERTEX:
        return shaderc_glsl_vertex_shader;
    case Engine::ShaderType::FRAGMENT:
        return shaderc_glsl_fragment_shader;
    case Engine::ShaderType::COMPUTE:
        return shaderc_glsl_compute_shader;
    }
    ENGINE_CORE_WARN("Unknown shader type! (Defaulted to vertex shader)");
    return shaderc_glsl_vertex_shader;
}

namespace Engine
{
    ShaderVK::ShaderVK(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertexSrc = Filesystem::ReadFile(vertexPath);
        std::string fragmentSrc = Filesystem::ReadFile(fragmentPath);

        shaderSources.insert_or_assign(ShaderType::VERTEX, vertexSrc);
        shaderSources.insert_or_assign(ShaderType::FRAGMENT, fragmentSrc);

        Compile();
    }

    ShaderVK::ShaderVK(const std::string& computePath)
    {
        std::string computeSrc = Filesystem::ReadFile(computePath);

        shaderSources.insert_or_assign(ShaderType::COMPUTE, computeSrc);
        
        Compile();
    }

    void ShaderVK::Compile()
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions compileOptions;

        compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
        compileOptions.SetGenerateDebugInfo();

        for (const auto& shader : shaderSources)
        {
            shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shader.second, GetShaderKind(shader.first), "shader", compileOptions);
            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                ENGINE_CORE_ERROR("Shader Compilation failed: {0}", result.GetErrorMessage());
                ENGINE_CORE_ASSERT(false, "Shader Compilation Failed");
            }

            SPIRVCode.insert_or_assign(shader.first, std::vector<uint32_t>(result.cbegin(), result.cend()));
            isCompiled = true;
        }
    }
    

    void ShaderVK::CreateShaderModule(VkDevice device)
    {
        for (const auto& code : SPIRVCode)
        {
            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.pNext = nullptr;

            createInfo.codeSize = code.second.size() * sizeof(uint32_t);
            createInfo.pCode = code.second.data();

            VkShaderModule shaderModule;

            ENGINE_CORE_ASSERT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS, "Failed to create shader module!");

            shaderModules.insert_or_assign(code.first, shaderModule);

        }
    }
}
