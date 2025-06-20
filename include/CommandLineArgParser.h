#pragma once
#include <stdarg.h>
#include <string.h>
#include <cassert>
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <set>
#include <fstream>
#include <algorithm>

namespace CommandLine
{
	enum class EArgRequirementType
	{
		Optional,
		Required,
		SpecifiedAtLeastOne,
		MultipleOptional,
	};

	class CArgDefinition
	{
		typedef CArgDefinition CThis;
		using OnFoundArgFunc = std::function<void()>;
	public:
		CArgDefinition()
			: m_requirementType(EArgRequirementType::Optional)
			, m_hasValue(true)
		{
		}
		CThis& SetDescription(const std::string& desc)
		{
			m_desc = desc;
			return *this;
		}
		CThis& SetExample(const std::string& str)
		{
			m_exampleDesc = str;
			return *this;
		}
		CThis& SetRequirementType(EArgRequirementType type)
		{
			m_requirementType = type;
			return *this;
		}
		CThis& SetNoValue()
		{
			m_hasValue = false;
			return *this;
		}
		CThis& SetOnFoundArgFunc(const OnFoundArgFunc& OnFoundArg)
		{
			m_OnFoundArg = OnFoundArg;
			return *this;
		}

	public:
		std::string m_desc;
		EArgRequirementType m_requirementType;
		std::string m_exampleDesc;
		bool m_hasValue;
		OnFoundArgFunc m_OnFoundArg;
	};

	class CCommandLineArg
	{
	public:
		CCommandLineArg()
		{
		}
		void Init(const std::string& name, const CArgDefinition& definition)
		{
			m_name = name;
			m_definition = definition;
		}

		std::string m_name;
		CArgDefinition m_definition;
	};

	class CCommandLineArgParser
	{
		using int32 = int;
		using uint32 = unsigned int;
		enum {InvalidIdx = -1};
	public:
		CCommandLineArgParser(int32 argc, const char** argv)
			: m_argc(argc)
			, m_argv(argv)
			, m_intrinsicArgsCount(0)
			, m_parsingIdx(InvalidIdx)
			, m_parsingArg(NULL)
			, m_parsingIsAcceptedHelpTypeArgs(false)
			//, m_parsingIsSpecifiedInvalidValue(false)
			, m_parsingIsArgValueInvalid(false)
		{
		}
		CCommandLineArg* Register(const std::string& name, const CArgDefinition& opt = CArgDefinition())
		{
			auto shared = std::make_shared<CCommandLineArg>();
			shared->Init(name, opt);
			m_vecArg.push_back(shared);
			return shared.get();
		}
		void RegisterHelp()
		{
			this->Register("--help", CArgDefinition()
				.SetDescription("This help information")
				.SetNoValue()
				.SetOnFoundArgFunc([&]
					{
						this->Help();
						this->SetParsingAcceptedHelpTypeArgs();
					}));
			m_intrinsicArgsCount++;
		}
		void RegisterHelpExampleUsage(const std::string& example)
		{
			this->Register("--example", CArgDefinition()
				.SetDescription("Example usage")
				.SetNoValue()
				.SetOnFoundArgFunc([&, example]
					{
						printf("%s", example.c_str());
						this->SetParsingAcceptedHelpTypeArgs();
					}));
			m_intrinsicArgsCount++;
		}
		void RegisterHelpRequirementType()
		{
			this->Register("--requirementtype", CArgDefinition()
				.SetDescription("Description of the 'Requirement Type' for Arguments")
				.SetNoValue()
				.SetOnFoundArgFunc([&]
					{
						printf(
R"(The 'Requirement Type' of an Argument includes the following types:
- Optional:			Specifying the argument is optional.
- Required:			Specifying The argument is required.
- Specified at least one:	Specifying at least one of the argument is required.
- Multiple optional:		Specifying one or more of the argument is optional.)");
						this->SetParsingAcceptedHelpTypeArgs();
					}));
			m_intrinsicArgsCount++;
		}
		void RegisterHelpVersion(uint32 major, uint32 minor, uint32 patch)
		{
			this->Register("--version", CArgDefinition()
				.SetDescription("Version in format of Major.Minor.Patch")
				.SetNoValue()
				.SetOnFoundArgFunc([&, major, minor, patch]
					{
						printf("Version: %u.%u.%u", major, minor, patch);
						this->SetParsingAcceptedHelpTypeArgs();
					}));
			m_intrinsicArgsCount++;
		}
		void RegisterDefaultHelps(const std::string& example, uint32 versionMajor, uint32 versionMinor, uint32 versionPatch)
		{
			this->RegisterHelp();
			this->RegisterHelpExampleUsage(example);
			this->RegisterHelpRequirementType();
			this->RegisterHelpVersion(versionMajor, versionMinor, versionPatch);
		}
		bool Parse()
		{
			bool isValidInvocation = false;
			bool printingHelp = false;
			if (m_argc > 1)
			{
				isValidInvocation = this->CheckForFormat();
				if (isValidInvocation)
				{
					for (m_parsingIdx = 1; m_parsingIdx < m_argc; ++m_parsingIdx)
					{
						auto& pszV = m_argv[m_parsingIdx];
						if (auto arg = this->FindArg(pszV))
						{
							m_parsingArg = arg;
							auto& Func = arg->m_definition.m_OnFoundArg;
							if (Func != NULL)
							{
								Func();
								if (m_parsingIsArgValueInvalid || m_parsingIsAcceptedHelpTypeArgs)
								{
									isValidInvocation = false;
									break;
								}
							}
							else
							{
								assert(false);
							}
						}
						else
						{
							isValidInvocation = false;
							printf("Unknown arg: %s\n", pszV);
							this->SuggestArgs(pszV);
						}
					}
				}
				if (!m_parsingIsAcceptedHelpTypeArgs)
				{
					if (!this->CheckForRequirements())
						isValidInvocation = false;
				}
			}
			else
			{
				printingHelp = (m_vecArg.size() - m_intrinsicArgsCount) > 0;
			}
			if (printingHelp)
				this->Help();
			return isValidInvocation;
		}
		void Help() const
		{
			assert(m_vecArg.size() > 0);
			uint32 cnt = static_cast<uint32>(m_vecArg.size());
			printf("Usage of %u argument%s:\n", cnt, cnt > 1 ? "s" : "");
			for (uint32 idx = 0; idx < cnt; ++idx)
			{
				auto arg = m_vecArg[idx].get();
				auto& def = arg->m_definition;
				printf("[%u]------------------------------------------------------\n", idx);
				printf("Argument:		%s\n", arg->m_name.c_str());
				printf("Requirement Type:	%s\n", ConvertArgRequirementTypeToString(def.m_requirementType).c_str());
				if (!def.m_desc.empty())
					printf("Description:		%s\n", def.m_desc.c_str());
				if (!def.m_exampleDesc.empty())
					printf("Example:		%s %s\n", arg->m_name.c_str(), def.m_exampleDesc.c_str());
			}
		}

	public:
		bool TryParseNextValue(std::string& value)
		{
			if ((m_parsingIdx + 1) >= m_argc)
			{
				assert(m_parsingArg != NULL);
				if (m_parsingArg->m_definition.m_hasValue)
					printf("Expected the value of %s\n", m_parsingArg->m_name.c_str());
				else
					printf("Unknown error\n");
				this->SetParsingArgValueInvalid();
			}
			if (!m_parsingIsArgValueInvalid)
			{
				value = this->ParseNextArgValue();
				return true;
			}
			return false;
		}

	private:
		std::string ParseNextArgValue()
		{
			auto& argv = m_argv;
			auto& idx = m_parsingIdx;
			idx += 1;
			std::string str;
			auto& psz = argv[idx];
			if (psz[0] == '\"')
			{
				str = &psz[1];
				assert(str.length() >= 2);
				if (str.back() == '\"')
					str.erase(str.begin() + str.length() - 1);
				else
					assert(false);
			}
			else
			{
				str = psz;
			}
			return str;
		}
		void SetParsingAcceptedHelpTypeArgs()
		{
			m_parsingIsAcceptedHelpTypeArgs = true;
		}
		void SetParsingArgValueInvalid()
		{
			m_parsingIsArgValueInvalid = true;
		}
		CCommandLineArg* FindArg(const char* pszV) const
		{
			for (auto& it : m_vecArg)
			{
				if (strcmp(pszV, it->m_name.c_str()) == 0)
					return it.get();
			}
			return NULL;
		}
		bool CheckForFormat() const
		{
			std::vector<int32> vecInputIdx;
			for (auto& it : m_vecArg)
			{
				auto arg = it.get();
				if (arg->m_definition.m_hasValue)
				{
					for (int32 idx1 = 1; idx1 < m_argc; ++idx1)
					{
						if (arg->m_name == m_argv[idx1])
							vecInputIdx.push_back(idx1);
					}
				}
			}
			std::set<std::string> setArg;
			for (auto& it : m_vecArg)
			{
				auto arg = it.get();
				setArg.insert(arg->m_name);
			}
			bool ok = m_argc > 1;
			for (auto& idxFollowedValue : vecInputIdx)
			{
				bool valid = true;
				auto followedIdx = idxFollowedValue + 1;
				if (followedIdx < m_argc)
				{
					auto& followedInput = m_argv[followedIdx];
					if (setArg.find(followedInput) != setArg.end())
					{
						ok = false;
						valid = false;
					}
				}
				else
				{
					valid = false;
					ok = false;
				}
				if (!valid)
					printf("%s must be followed with a value\n", m_argv[idxFollowedValue]);
			}
			return ok;
		}
		bool CheckForRequirements() const
		{
			std::set<std::string> setRequiredButNotFound;
			for (auto& it : m_vecArg)
			{
				auto arg = it.get();
				auto& type = arg->m_definition.m_requirementType;
				if (type == EArgRequirementType::Required || type == EArgRequirementType::SpecifiedAtLeastOne)
					setRequiredButNotFound.insert(arg->m_name);
			}
			if (m_argc > 1)
			{
				for (int32 idx = 1; idx < m_argc; ++idx)
				{
					auto& pszV = m_argv[idx];
					auto itFound = setRequiredButNotFound.find(pszV);
					if (itFound != setRequiredButNotFound.end())
						setRequiredButNotFound.erase(itFound);
				}
			}

			if (setRequiredButNotFound.size() > 0)
			{
				printf("The following argument%s must be specified: ", setRequiredButNotFound.size() > 1 ? "s" : "");
				uint32 idx = 0;
				for (auto& it : setRequiredButNotFound)
				{
					printf("%s", it.c_str());
					if (idx != setRequiredButNotFound.size() - 1)
						printf(", ");
					idx++;
				}
				printf("\n");
				return false;
			}
			return true;
		}
		void SuggestArgs(const std::string& userInput) const
		{
			std::vector<std::string> suggestions;

			for (const auto& command : m_vecArg) {
				auto input = RemoveNonAlpha(userInput);
				auto name = RemoveNonAlpha(command->m_name);
				if (IsPrefixMatch(input, name)) {
					suggestions.push_back(command->m_name);
				}
			}

			if (!suggestions.empty()) {
				uint32 cnt = static_cast<uint32>(suggestions.size());
				printf("Did you mean %s: ", cnt > 1 ? FormatString("these %u args", cnt).c_str() : "this arg");
				for (uint32 idx = 0; idx < cnt; ++idx) {
					printf("%s", suggestions[idx].c_str());
					if (idx != cnt - 1)
						printf(", ");
				}
				printf("\n");
			}
			else {
				//std::cout << "No suggestions found." << std::endl;
			}
		}

	private:
		static std::string ConvertArgRequirementTypeToString(EArgRequirementType type)
		{
			std::string str;
			switch (type)
			{
			case EArgRequirementType::Optional: str = "Optional"; break;
			case EArgRequirementType::Required: str = "Required";  break;
			case EArgRequirementType::SpecifiedAtLeastOne: str = "Specified at least one";  break;
			case EArgRequirementType::MultipleOptional: str = "Multiple optional";  break;
			default:
				break;
			}
			return str;
		}
		static std::string RemoveNonAlpha(const std::string& input) {
			std::string result;
			for (char ch : input) {
				if (std::isalpha(ch)) {
					result += ch;
				}
			}
			return result;
		}
		static std::string ToLower(const std::string& str) {
			std::string lowerStr;
			std::transform(str.begin(), str.end(), std::back_inserter(lowerStr),
				[](unsigned char c) { return std::tolower(c); });
			return lowerStr;
		}
		static bool IsPrefixMatch(const std::string& input, const std::string& option) {
			std::string lowerInput = ToLower(input);
			std::string lowerOption = ToLower(option);
			return lowerInput.size() <= lowerOption.size() &&
				std::equal(lowerInput.begin(), lowerInput.end(), lowerOption.begin());
		}
		static std::string FormatString(const char* format, ...)
		{
			char szContent[10240] = "";
			va_list va_alist;
			va_start(va_alist, format);
			vsnprintf(szContent, 10240, format, va_alist);
			va_end(va_alist);
			return szContent;
		}

	private:
		const int32 m_argc;
		const char* const* m_argv;
		std::vector<std::shared_ptr<CCommandLineArg> > m_vecArg;
		uint32 m_intrinsicArgsCount;
		int32 m_parsingIdx;
		const CCommandLineArg* m_parsingArg;
		bool m_parsingIsAcceptedHelpTypeArgs;
		//bool m_parsingIsSpecifiedInvalidValue;
		bool m_parsingIsArgValueInvalid;
	};
}