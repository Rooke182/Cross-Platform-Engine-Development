using System;
using System.Text.RegularExpressions;

namespace MyApp
{
    public class Component
    {
        public string name;
        public string sourceFile;

        public List<string> publicMembers;

        void FindAllPublicMembers(int startingLine)
        {
            string[] file = File.ReadAllLines(sourceFile);
            publicMembers = new List<string>();

            bool isPrivate = true;

            Regex regex = new Regex(@"[^ return{:][a-zA-Z0-9<>]+ m_\w*");
            Regex secondaryRegex = new Regex(@"m_\w+");

            int bracketLevel = 0;
            if (!file[startingLine].Contains('{'))
                startingLine++;

            for (int i = startingLine; i < file.Length; i++)
            {
                if (file[i].Contains("public:"))
                    isPrivate = false;
                else if (file[i].Contains("private:") || file[i].Contains("protected:"))
                    isPrivate = true;

                int openingBrackets = file[i].Count(x => x == '{');
                int closingBrackets = file[i].Count(x => x == '}');

                bracketLevel += openingBrackets - closingBrackets;

                if (bracketLevel <= 0)
                    break;

                if(!isPrivate && bracketLevel == 1)
                {
                    var matches = regex.Matches(file[i]);

                    foreach (Match match in matches)
                    {
                        string matchString = match.ToString();
                        matchString = matchString.Substring(matchString.IndexOf(' ') + 1);
                        if (file[i].Contains('(') && file[i].IndexOf('(') < file[i].IndexOf(matchString) || file[i].Contains("const"))
                            break;

                        if (file[i].Contains(','))
                        {
                            var secondaryMatches = secondaryRegex.Matches(file[i]);
                            foreach (Match secondaryMatch in secondaryMatches)
                                publicMembers.Add(secondaryMatch.ToString());
                        }
                        else
                        {
                            publicMembers.Add(matchString);
                        }
                    }
                }
            }
        }

        public Component(string name, string source, int startingLine)
        {
            this.name = name;
            this.sourceFile = source.Substring(Directory.GetCurrentDirectory().Length + "\\".Length);
            FindAllPublicMembers(startingLine);
        }
    }

    public class Class
    {
        public string name;
        public string sourceFile;
        public int startingLine;

        public Class(string name, string source, int startingLine)
        {
            this.name = name;
            this.sourceFile = source;
            this.startingLine = startingLine;
        }
    }

    internal class Program
    {

        //gets all strings in file matching the format
        //class X : public Y
        //Since any components have to either inherit from other components
        //Or from the base component
        static List<Class> GetInheritedClassesFromFile(string filePath)
        {
            List<Class> classes = new List<Class>();
            string[] file = File.ReadAllLines(filePath);

            string pattern = @"\bclass\b \n?\w+\b\s*:\s*public \w+\b";
            Regex regex = new Regex(pattern);

            for (int i = 0; i < file.Length; i++)
            {
                //class definition can be over multiple lines
                string lines;
                if (i < file.Length - 1)
                    lines = file[i] + file[i + 1];
                else
                    lines = file[i];

                var matches = regex.Matches(lines);
                foreach (Match match in matches)
                {
                    classes.Add(new Class(match.Value, filePath, i + 1));
                }
            }

            return classes;
        }

        static List<Component> GetAllComponents()
        {
            List<Class> classes = new List<Class>();
            List<Component> components = new List<Component>();
            Dictionary<string, string> classInherits = new Dictionary<string, string>();
            string[] files = Directory.GetFiles(Directory.GetCurrentDirectory());

            foreach (string file in files)
            {
                if(file.EndsWith(".cpp") || file.EndsWith(".h") || file.EndsWith(".hpp"))
                    classes.AddRange(GetInheritedClassesFromFile(file));
            }

            foreach(Class currentClass in classes)
            {
                string className = currentClass.name.Substring(currentClass.name.IndexOf("class ") + "class ".Length, currentClass.name.IndexOf(" :") - "class ".Length);
                string inheritsFrom = currentClass.name.Substring(currentClass.name.IndexOf("public ") + "public ".Length);

                if(!classInherits.ContainsKey(className))
                    classInherits.Add(className, inheritsFrom);
            }

            foreach(KeyValuePair<string, string> currentClass in classInherits)
            {
                string inherits = currentClass.Value;

                while(classInherits.ContainsKey(inherits) || inherits == "BaseComponent")
                {
                    if (inherits == "BaseComponent")
                    {
                        var temp = classes.Find(x => x.name.Contains(currentClass.Key));
                        components.Add(new Component(currentClass.Key, temp.sourceFile, temp.startingLine));
                        break;
                    }
                    else if (classInherits.ContainsKey(inherits))
                        inherits = classInherits[inherits];
                    else
                        break;
                }
            }

            return components;
        }

        static void WriteHeaderFile(List<Component> components)
        {
            List<String> hFile = new List<string>() {
                "//Do not edit this file",
                "//It is recreated on build",
                "#pragma once",
                "#include \"BaseComponent.h\"",
                "#include \"ComponentPtr.h\"",
                "#include \"Scene.h\"",          
                "#include <json.hpp>",          
                "#include <string>",          
                "#include <vector>",
                "#include <map>",
                ""
            };

            foreach(Component component in components)
            {
                hFile.Add("class " + component.name + ";");
            }
            hFile.Add("");

            hFile.Add("ComponentPtr<BaseComponent> CreateComponent(Scene& scene, unsigned int id);");
            hFile.Add("std::vector<std::string> GetAllComponentTypeNames();");
            hFile.Add("std::map<std::string, unsigned int> GetComponentStringIDMap();");
            hFile.Add("");

            foreach (Component component in components)
            {
                hFile.Add("void to_json(nlohmann::json& json, const ComponentPtr<" + component.name + ">& componentPtr);");
            }
            hFile.Add("");

            foreach (Component component in components)
            {
                hFile.Add("void from_json(const nlohmann::json& json, ComponentPtr<" + component.name + ">& componentPtr);");
            }

            File.WriteAllLines("PreBuild.h", hFile);
        }

        static void WriteCPPFile(List<Component> components)
        {
            List<string> cppFile = new List<string>() {
                "//Do not edit this file",
                "//It is recreated on build",
                "#include  \"PreBuild.h\"",
                ""};

            List<string> alreadyIncludedSources = new List<string>();
            foreach (Component component in components)
            {
                if (!alreadyIncludedSources.Contains(component.sourceFile))
                {
                    cppFile.Add("#include\"" + component.sourceFile + "\"");
                    alreadyIncludedSources.Add(component.sourceFile);
                }
            }
            cppFile.Add("");

            cppFile.AddRange(new List<string>() {
                "ComponentPtr<BaseComponent> CreateComponent(Scene& scene, unsigned int id)",
                "{",
                "   ComponentPtr<BaseComponent> component;",
                "   switch (id)",
                "   {"});

            foreach (Component component in components)
            {
                cppFile.AddRange(new List<string>() {
                "       case GetComponentID("  + component.name + "):",
                "           component = scene.MakeComponent<" + component.name + ">();",
                "           break;"
                });
            }

            cppFile.AddRange(new List<string>() {
                "   }",
                "   ",
                "   return component;",
                "}",
                ""});
            cppFile.Add("");

            cppFile.AddRange(new List<string>() {

                "std::map<std::string, unsigned int> GetComponentStringIDMap()",
                "{",
                "   return std::map<std::string, unsigned int> {" });

            foreach (Component component in components)
            {
                cppFile.AddRange(new List<string>() {
                "       {\"" + component.name + "\",GetComponentID("  + component.name + ")}" + ((components.IndexOf(component) < components.Count()-1) ? "," : ""),
                });
            }

            cppFile.AddRange(new List<string>() {
                "   };",
                "}"});
            cppFile.Add("");



            cppFile.Add("std::vector<std::string> GetAllComponentTypeNames()");
            cppFile.Add("{");
            string componentNames = "";
            foreach(Component component in components)
            {
                if (componentNames != "")
                    componentNames += ", ";

                componentNames += "\"" + component.name + "\"";
            }
            cppFile.Add("   return std::vector<std::string>{" + componentNames + "};");
            cppFile.Add("}");

            foreach (Component component in components)
            {
                cppFile.Add("void to_json(nlohmann::json& json, const ComponentPtr<" + component.name + ">& componentPtr)");
                cppFile.Add("{");
                cppFile.Add("   to_json(json, (ComponentPtr<BaseComponent>&)componentPtr);");
                cppFile.Add("}");
            }
            cppFile.Add("");

            foreach (Component component in components)
            {
                cppFile.Add("void from_json(const nlohmann::json& json,ComponentPtr<" + component.name + ">& componentPtr)");
                cppFile.Add("{");
                cppFile.Add("   from_json(json, (ComponentPtr<BaseComponent>&)componentPtr);");
                cppFile.Add("}");
            }

            File.WriteAllLines("PreBuild.cpp", cppFile);
        }

        static void WriteSerialisationMacros()
        {
            List<string> serialisationMacros = new List<string>(){
                "//Do not edit this file",
                "//It is recreated on build",
                "#pragma once",
                ""};
            for (int i = 0; i < cNumGets; i++)
            {
                string line = "#define GET" + i + "(";

                for (int j = 0; j < i + 1; j++)
                {
                    line += "_" + j + ", ";
                }
                line += "...) " + "_" + i;

                serialisationMacros.Add(line);
            }

            serialisationMacros.Add("");

            serialisationMacros.Add("#ifdef _MSC_FULL_VER");
            for (int i = 0; i < cNumRepeats; i++)
            {
                string line = "#define REPEAT" + (i + 1) + "(expression, ...) ";
                for (int j = 0; j < (i + 1); j++)
                {
                    if (j != 0)
                        line += "##";
                    line += "expression(GET" + j + "(__VA_ARGS__))";
                }
                serialisationMacros.Add(line);
            }
            serialisationMacros.Add("");
            serialisationMacros.Add("#elif defined __GNUC__");

            for (int i = 0; i < cNumRepeats; i++)
            {
                string line = "#define REPEAT" + (i + 1) + "(expression, ...) ";
                for (int j = 0; j < (i + 1); j++)
                {
                    line += "expression(GET" + j + "(__VA_ARGS__))";
                }
                serialisationMacros.Add(line);
            }

            serialisationMacros.Add("#endif");
            serialisationMacros.Add("");
            serialisationMacros.Add("#define REPEAT_FUNC(func, numTimes, ...) REPEAT##numTimes(func, __VA_ARGS__)");

            File.WriteAllLines("SerialisationMacros.h", serialisationMacros);
        }

        static void UpdateComponentsToSerialisePublicFields(List<Component> components)
        {
            List<string> publicSerialise = new List<string> 
            {
                 "//Do not edit this file",
                "//It is recreated on build",
                "#include <json.hpp>",
                "#include \"PreBuild.h\""
            };

            foreach (Component component in components)
            {
                publicSerialise.Add("#include \"" + component.sourceFile + "\"");
            }

            publicSerialise.Add("");

            foreach(Component component in components)
            {
                publicSerialise.Add("void " + component.name + "::PublicSerialise(nlohmann::json& json) const");
                publicSerialise.Add("{");

                foreach (string member in component.publicMembers)
                {
                    publicSerialise.Add("\tjson[\"" + member + "\"] = " + member + ";");
                }

                publicSerialise.Add("}");
                publicSerialise.Add("");

                publicSerialise.Add("void " + component.name + "::PublicDeserialise(const nlohmann::json& component)");
                publicSerialise.Add("{");
                foreach (string member in component.publicMembers)
                {
                    publicSerialise.Add("\tif (component.contains(\"" + member + "\"))");
                    publicSerialise.Add("\t\t" + member + " = component[\"" + member + "\"];");
                }
                publicSerialise.Add("}");
                publicSerialise.Add("");
            }

            File.WriteAllLines("PublicSerialise.cpp", publicSerialise);
        }

        const int cNumRepeats = 100;
        const int cNumGets = 100;

        static void Main(string[] args)
        {
            List<Component> components = GetAllComponents();
            WriteHeaderFile(components);
            WriteCPPFile(components);
            WriteSerialisationMacros();
            UpdateComponentsToSerialisePublicFields(components);
        }
    }
}
