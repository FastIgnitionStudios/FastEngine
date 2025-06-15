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
            var allFiles = new List<SourceFile>();
            var totalClasses = 0;
            
            var headerFiles = Directory.GetFiles(inputDir, "*.h", SearchOption.AllDirectories);
            
            Console.WriteLine($"Parsing {headerFiles.Length} header files");
            foreach (var headerFile in headerFiles)
            {
                SourceFile sourceFile = new();
                sourceFile.classes = parser.ParseFile(headerFile);
                sourceFile.path = headerFile;
                allFiles.Add(sourceFile);
                totalClasses += sourceFile.classes.Count;
            }
            
            Console.Write($"\nGenerating reflection code for {totalClasses} classes");
            generator.GenerateReflectionCode(allFiles, outputDir);
        }
    }
}