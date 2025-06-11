using System;
using System.Collections.Generic;
using System.IO;
using FastReflection.CodeGen;
using FastReflection.Parsers;
using FastReflection.Types;

namespace FastReflection
{
    public class FastReflection
    {
        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: FastReflection.exe <header file> <output directory>");
                return;
            }
            
            string inputDir = args[0];
            string outputDir = args[1];

            if (!Directory.Exists(inputDir))
            {
                Console.WriteLine($"Input directory \"{inputDir}\" does not exist");
                return;
            }

            Directory.CreateDirectory(outputDir);

            var parser = new HeaderFileParser();
            var generator = new HeaderSourceGenerator();
            var allClasses = new List<ReflectedClass>();
            
            var headerFiles = Directory.GetFiles(inputDir, "*.h", SearchOption.AllDirectories);
            
            Console.WriteLine($"Parsing {headerFiles.Length} header files");

            foreach (var headerFile in headerFiles)
            {
                Console.WriteLine($"Parsing {headerFile}");
                var classes = parser.ParseFile(headerFile);
                allClasses.AddRange(classes);
                
                Console.WriteLine($"Found {classes.Count} reflected classes");
            }
            
            Console.Write($"\nGenerating reflection code for {allClasses.Count} classes");
            generator.GenerateReflectionCode(allClasses, outputDir);
        }
    }
}