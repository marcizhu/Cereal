# Cereal: A serialization library for C++ and C Sharp
## What is Cereal?
Cereal is a header-only library based on [TheCherno](https://github.com/thecherno/)'s
 serialization library for Java. This project is maintained by [raresica1234](https://github.com/raresica1234/) and [marcizhu](https://github.com/marcizhu/).
 The library is really simple and fully customizable to fit all your needs. Right now, it can serialize and deserialize the following data objects:
 - Fields: A field just stores a simple data type, like an `int` or a `short`
 - Arrays: An array is a collection of simple data types, for example you can have 10 `floats` stored
 - Objects: Each object contains a name (like all the other data structures) but also it stores fields and arrays
 - Databases: A database is a collection of objects, each with its own fields and arrays
 - Headers: Headers are optional, but really useful if you want to store more than one database per file. With headers you can have up to 255 databases in a single file!

For more information, check out the [wiki page](https://github.com/marcizhu/Cereal/wiki)
