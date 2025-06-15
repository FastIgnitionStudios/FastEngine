using System.Collections.Generic;

namespace FastReflection.Types
{
    public enum AccessFlags
    {
        Public,
        Protected,
        Private,
    }
    
    
    public class ReflectedClass
    {
        public string Name { get; set; }
        public string Namespace { get; set; }
        
        public string ClassType { get; set; }
        public string MacroType { get; set; }
        public List<ReflectedProperty> Properties { get; set; } = new();
        public List<ReflectedMethod> Methods { get; set; } = new();
        public List<string> Flags { get; set; } = new();
        public string SourceFile { get; set; }
        
        
    }

    public class ReflectedProperty
    {
        public string Name { get; set; }
        public string Type { get; set; }
        public List<string> Flags { get; set; } = new();
        public AccessFlags AccessFlags { get; set; }
    }

    public class ReflectedMethod
    {
        public string Name { get; set; }
        public string ReturnType { get; set; }
        public List<ReflectedParameter> Parameters { get; set; } = new();
        public List<string> Flags { get; set; } = new();
        public AccessFlags AccessFlags { get; set; }
    }

    public class ReflectedParameter
    {
        public string Name { get; set; }
        public string Type { get; set; }
    }
}