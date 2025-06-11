using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using FastReflection.Types;

namespace FastReflection.Parsers
{
    public class HeaderFileParser
    {
        private readonly List<string> _reflectionMacros = new()
        {
            "RCLASS",
            "RSTRUCT",
            "RPROPERTY",
            "RMETHOD",
            "RENUM"
        };

        public List<ReflectedClass> ParseFile(string filePath)
        {
            var content = File.ReadAllText(filePath);
            var classes = new List<ReflectedClass>();

            // Remove any comments
            RemoveComments(content);
            
            var lines = new List<string>(content.Split("\r\n"));

            // Find all classes/structs with the coresponding macros
            var classMatches = Regex.Matches(content,
                @"(RCLASS|RSTRUCT)\s*\(\s*([^)]*)\s*\)\s*(?:class|struct)\s+(\w+)", RegexOptions.Multiline);

            foreach (Match match in classMatches)
            {
                var reflectedClass = new ReflectedClass
                {
                    Name = match.Groups[3].Value,
                    SourceFile = filePath
                };
                
                // Parse namespace
                for (int i = lines.FindIndex(s => s.Contains(match.Groups[3].Value)); i >= 0; i--)
                {
                    var line = lines[i];
                    if (line.StartsWith("namespace "))
                    {
                        reflectedClass.Namespace = line.Substring("namespace ".Length);
                        break;
                    }
                }

                // Parse flags from macro
                if (!string.IsNullOrEmpty(match.Groups[1].Value))
                {
                    reflectedClass.Flags = ParseFlags(match.Groups[1].Value);
                }
                
                var classBody = ExtractClassBody(content, match.Index + match.Length);
                ParseClassMembers(classBody, reflectedClass);
                
                classes.Add(reflectedClass);
            }

            return classes;
        }

        private string RemoveComments(string content)
        {
            content = Regex.Replace(content, "//.*$", "", RegexOptions.Multiline);

            content = Regex.Replace(content, @"/\*.*?\*/", "", RegexOptions.Singleline);

            return content;
        }

        private List<string> ParseFlags(string flags)
        {
            return flags.Split(',')
                .Select(flag => flag.Trim())
                .Where(flag => !string.IsNullOrEmpty(flag))
                .ToList();
        }

        private string ExtractClassBody(string content, int startIndex)
        {
            int braceCount = 0;
            int bodyStart = -1;

            for (int i = startIndex; i < content.Length; i++)
            {
                if (content[i] == '{')
                {
                    if (bodyStart == -1) bodyStart = i;
                    braceCount++;
                }
                else if (content[i] == '}')
                {
                    braceCount--;
                    if (braceCount == 0) return content.Substring(bodyStart + 1, i - bodyStart - 1);
                }
            }

            return "";
        }

        private void ParseClassMembers(string classBody, ReflectedClass reflectedClass)
        {
            var propertyMatches = Regex.Matches(classBody, @"RPROPERTY\s*\(\s*([^)]*)\s*\)\s*([^;]+)\s+(\w+)\s*;",
                RegexOptions.Multiline);

            foreach (Match match in propertyMatches)
            {
                var property = new ReflectedProperty
                {
                    Name = match.Groups[3].Value,
                    Type = match.Groups[2].Value.Trim(),
                    Flags = ParseFlags(match.Groups[1].Value)
                };
                reflectedClass.Properties.Add(property);
            }

            var methodMatches = Regex.Matches(classBody, @"RMETHOD\s*\(\s*([^)]*)\s*\)\s*([^(]+)\s+(\w+)\s*\(([^)]*)\)",
                RegexOptions.Multiline);
            foreach (Match match in methodMatches)
            {
                var method = new ReflectedMethod
                {
                    Name = match.Groups[3].Value,
                    ReturnType = match.Groups[2].Value.Trim(),
                    Flags = ParseFlags(match.Groups[1].Value),
                };

                if (!string.IsNullOrEmpty(match.Groups[4].Value))
                {
                    var paramStrings = match.Groups[4].Value.Split(',');
                    foreach (var paramStr in paramStrings)
                    {
                        var parts = paramStr.Trim().Split(' ');
                        if (parts.Length >= 2)
                        {
                            method.Parameters.Add(new ReflectedParameter
                            {
                                Type = string.Join(" ", parts.Take(parts.Length - 1)),
                                Name = parts.Last()
                            });
                        }
                    }
                }

                reflectedClass.Methods.Add(method);
            }
        }
    }
}