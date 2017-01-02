using System;
using System.Collections.Generic;

namespace Cereal_CSharp
{
	class Program
	{
		static void Main(string[] args)
		{
			Cereal.Buffer buff = new Cereal.Buffer(64);
			//buff.readFile(".\\dictionaries-english.db");

			//Cereal.Database db = new Cereal.Database();
			//db.read(ref buff);

			//List<string> words = db.getObject("English").getArray("words").getArray();

			//foreach(string s in words)
			//{
			//	Console.WriteLine(s);
			//}

			Cereal.Field field = new Cereal.Field("test", 7.12345);
			Cereal.Field f = new Cereal.Field();

			field.write(ref buff);
			buff.Position = 0;
			//Console.WriteLine("{0} {1} {2} {3} {4} {5} {6} {7}", buff.Start[0], buff.Start[1], buff.Start[2], buff.Start[3], buff.Start[4], buff.Start[5], buff.Start[6], buff.Start[7]);
			f.read(ref buff);

			Console.WriteLine("Value: {0}", f.getFloat());

			Console.Write("\nDone.");
			Console.ReadKey();
		}
	}
}
