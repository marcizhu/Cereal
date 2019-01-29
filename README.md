# Cereal

[![standard-readme compliant](https://img.shields.io/badge/standard--readme-OK-green.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)
[![CircleCI](https://circleci.com/gh/marcizhu/Cereal/tree/master.svg?style=svg)](https://circleci.com/gh/marcizhu/Cereal/tree/master)
[![CodeFactor](https://www.codefactor.io/repository/github/marcizhu/cereal/badge)](https://www.codefactor.io/repository/github/marcizhu/cereal)

> A serialization library for C++ and [C Sharp](https://github.com/marcizhu/Cereal-CSharp/)

Cereal is a header-only library based on [TheCherno](https://github.com/thecherno/)'s
 serialization library for Java. It can be used to serialize almost any kind of data. It's really fast, lightweight and ideal for videogames and other programs where performance and resources are really valuable.
 
The library is really simple to use and fully customizable to fit all your needs. If you make any changes, please open a pull request. We'd love to see your changes!

## Table of Contents
- [Background](#background)
- [Install](#install)
- [Usage](#usage)
- [API](#api)
- [Maintainers](#maintainers)
- [Contribute](#contribute)
- [License](#license)

## Background
This project began a while ago, when both [@raresica1234](https://github.com/raresica1234) and I were developing our own game engines. We needed a way to serialize (that is, to store variables and data structures on files) and deserialize information such as game data, game settings, 3D files and much more.

The main inspiration came from [@TheCherno](https://github.com/TheCherno)'s [serialization library](https://github.com/TheCherno/JavaSerialization). We took the main idea and ported it to C++ with a few improvements.

The entire project was done in less than a week. After that, we started writing the [wiki](https://github.com/marcizhu/Cereal/wiki), writing some automated tests and working on some extra features.

## Install
To install this library, just download it and copy the contents of the `Cereal/Cereal` folder to your include forlder. Then, you only need to include the file `Cereal.h` in your source files.

### From the command line
Just copy the folder `Cereal/Cereal` from this repository to the include folder of your project so that it contains `Cereal.h` and the folder `src`:

```console
marcizhu@linux:~ $ git clone https://github.com/marcizhu/Cereal.git
Cloning into 'Cereal'...
remote: Counting objects: 1031, done.
remote: Compressing objects: 100% (38/38), done.
remote: Total 1031 (delta 17), reused 35 (delta 9), pack-reused 980
Receiving objects: 100% (1031/1031), 1.49 MiB | 678.00 KiB/s, done.
Resolving deltas: 100% (616/616), done.
marcizhu@linux:~ $ cd Cereal
marcizhu@linux:~/Cereal $ mv Cereal/Cereal ../myProject/includes/
marcizhu@linux:~/Cereal $ cd ../myProject/includes/
marcizhu@linux:~/myProject/includes $ ls
Cereal.h     src/
```

### Manually
To manually install the library, click on the green button in the top right and download this repository. After that, copy the folder `Cereal/Cereal` to your include folder.

## Usage
Using this library is pretty straightforward. Just include the file `Cereal.h`in your source file:

```cpp
#include <Cereal.h>
```

After that, all the API will be available inside the `Cereal` namespace. You may include a `using namespace Cereal;` at the top of your source file if you don't like having to type `Cereal::` in front of every class.

The following snippet illustrates how to use this library to write some configuration to a file and read it back:

```cpp
#include <Cereal.h> // Include the library

void writeDatabase()
{
    Cereal::Buffer buffer(1024); // Create a buffer

    Cereal::Database* db = new Cereal::Database("My program database"); // Create a database

    Cereal::Object* config = new Cereal::Object("Configuration"); // Create an object to store configuration

    // Adding fields to objects
    config->addField(new Cereal::Field("windowSizeX", winSize.x)); // Create a field and add it to the object
    config->addField(new Cereal::Field("windowSizeY", winSize.y));
    config->addField(new Cereal::Field("user name", userName)); 
    config->addField(new Cereal::Field("other value", 15.9f));
    
    // Adding objects to databases
    db->addObject(config); // Add object 'config' to the database

    // Write the database to the buffer & then to a file
    db->write(buffer);
    buffer.writeFile("progDatabase.db");
    
    // Free dynamic memory
    delete db; // Databases will automatically delete all the objects, fields and arrays they contain
}

void readDatabase()
{
    // Create an empty buffer and read the database file 'progDatabase.db'
    Cereal::Buffer buf;
    buf.readFile("progDatabase.db");

    // Read database from buffer
    Cereal::Database* db = new Cereal::Database; // Create an empty database
    db->read(buf); // Read the buffer

    // Getting an object from a database
    Cereal::Object* config = otherDatabase->getObject("Configuration");

    // Getting the configuration back
    winSize.x = config->getField("windowSizeX")->getValue<int>(); // Get the data
    winSize.y = config->getField("windowSizeY")->getValue<int>();
    userName = config->getField("user name")->getValue<std::string>();
    otherVal = config->getField("other value")->getValue<float>();
    
    delete db;
}
```

For a brief description of each class, its capabilities and its use, please read the [API section](#api)

### GCC/Clang (Linux/MacOS)
Finally, to compile your file (say `main.cpp`) with Cereal, just include the `-I` modifier followed by the folder where the file `Cereal.h` is located. Also include `-std=c++11` to the compilation command. This is necessary because Cereal uses some C++11 features:

```console
g++ main.cpp -Ipath/to/Cereal -std=c++11 -o output_file
```

### Microsoft Visual Studio 2015/2017 (Windows)
To compile a file with Cereal using visual studio, just add the Cereal folder to the include path. This is done in _Project Settings_ > _Project paths_ > _Include paths_.

Once this is done, your project should compile without any problems!

## Status
This library is actively maintained. We are basically testing the library, improving its features and making sure everything works as intended. More than **8,000 assertions** and 20 tests make sure this library is stable and that no new bugs are introduced.

This tests are executed after each push commit, and the status is displayed as a green tick or red mark next to each commit. You can see the details for each test case.

Right now, the library is ready to be used in any application. As far as we know, it is 100% bug free and fully cross-platform, compiling in Microsoft Windows Vista/7/8/8.1/10, Linux Debian/Ubuntu/Mint/Kali, and MacOS High Sierra.

## API
This library contains different classes. Each class can be used to serialized a different kind of data:

 - [Cereal::Field](https://github.com/marcizhu/Cereal/wiki/Fields): A field is the smallest serialization unit. It holds a name and a simple data type which can be any of the following: `bool`, `char`, `byte`, `short`, `int`, `long long`, `float`, `double` and `std::string`. The name is used to access the field and distinguish it from other fields. It is used to store name-value pairs.
 - [Cereal::Array](https://github.com/marcizhu/Cereal/wiki/Arrays): An array contains a name and a list of simple data types. Those types can be any of the previously mentioned, and this object can contain thousands, even millions of them.
 - [Cereal::Object](https://github.com/marcizhu/Cereal/wiki/Objects): Each object contains a name (like all the other data structures) but it also stores up to 65536 fields and 65536 arrays. The fields and arrays contained by an object **are not shared** across all other objects, so a field or array name doesn't have to be unique across all objects, but it must be unique across all the fields and arrays in that object.
 - [Cereal::Database](https://github.com/marcizhu/Cereal/wiki/Databases): A database is a collection of objects, each with its own fields and arrays. Databases also have a name, and that can be used if you want to store multiple databases in the same file. If you are only using one database per file, the name is not really that useful.
 - [Cereal::Header](https://github.com/marcizhu/Cereal/wiki/Headers): Headers are optional, but really useful if you want to store more than one database per file. With headers you can have up to 255 databases in a single file! Headers do NOT have a name, and only contain a list of databases.
 - [Cereal::Buffer](https://github.com/marcizhu/Cereal/wiki/Buffers): Buffers are just a helper class that holds serialized data. All data is serialized to this buffer first (using the method `Database::write()`), then it can be written to a file using `Buffer::writeFile()`. Later, a file can be read using `Buffer::readFile()`and deserialized by calling the method `Database::read()`. Easy, right?

For more information, check out the [GitHub wiki page](https://github.com/marcizhu/Cereal/wiki)

## Maintainers
This project is currently maintained by:

- [@marcizhu](https://github.com/marcizhu)
- [@raresica1234](https://github.com/raresica1234)

## Contribute
Pull requests and suggestions are always accepted. If you have any question, feel free to open an [issue](https://github.com/marcizhu/Cereal/issues) and we will reply as soon as possible :)

For support and other questions, you can also send me an e-mail at marcizhu@gmail.com.

Small note: If editing the README, please conform to the [standard-readme](https://github.com/RichardLitt/standard-readme) specification.

## License
This project is licensed under the [GNU General Public License v3.0](https://github.com/marcizhu/Cereal/blob/master/LICENSE.txt). Copyright © 2016-2018 The Cereal Team
