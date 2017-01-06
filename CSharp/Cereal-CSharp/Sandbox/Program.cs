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

			Cereal.Object obj = new Cereal.Object("Object");
			Cereal.Field field = new Cereal.Field("test", 3.14159265);

			//obj.Fields.Add(field);

			field.write(ref buff);

			buff.Position = 0;
			Cereal.Field f = new Cereal.Field();
			f.read(ref buff);

			double val = f.getDouble();

			Console.WriteLine("Value: {0}", f.getDouble());

			Console.Write("\nDone.");
			Console.ReadKey();
		}
	}
}
