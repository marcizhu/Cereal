using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Cereal;

namespace Cereal_CSharp
{
	class Program
	{
		static void Main(string[] args)
		{
			Cereal.Buffer buff = new Cereal.Buffer(1024);

			Cereal.Database db = new Cereal.Database("DB Name");
			Cereal.Object obj = new Cereal.Object("Obj");
			obj.addField(new Cereal.Field("test", "string"));
			db.addObject(obj);

			db.write(ref buff);

			buff.Position = 0;

			Cereal.Database second = new Cereal.Database();
			second.read(ref buff);

			Console.WriteLine("Name: {0}", second.getObject("Obj").getField("test").Name);
			string x = second.getObject("Obj").getField("test").getString();
			Console.WriteLine("Value: {0}", x);

			//Cereal.Buffer newBuff = new Cereal.Buffer(buff.Length);
			//db.write(ref newBuff);

			//newBuff.writeFile(".\\test.db");

			Console.Write("\nDone.");
			Console.ReadKey();
		}
	}
}
