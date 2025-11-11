#include "Domain.hpp"

namespace IL2CPP
{
	Image* Domain::OpenAssembly(const std::string& assemblyName)
	{
		const IMPORT::Il2CppAssembly* result = IMPORT::il2cpp_domain_assembly_open(
			this,
			assemblyName.c_str()
		);

		if (result == nullptr)
		{
			throw Exception::NoSuchImage(assemblyName);
		}

		return reinterpret_cast<Image *>(result->image);
	}

	std::vector<Image*> Domain::OpenEveryAssembly()
    {
		size_t size;
		const IMPORT::Il2CppAssembly** result = IMPORT::il2cpp_domain_get_assemblies(
			this,
			&size
		);

	    std::vector<Image*> out;
	    for (size_t i = 0; i < size; i++)
	    {
            const IMPORT::Il2CppAssembly* assembly = result[i];
	        out.push_back(reinterpret_cast<Image *>(assembly->image));
	    }

        return std::move(out);
	}
}