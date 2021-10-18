# cuasMQ

Dependencies
- c++17 compatible compiler
- librdkafka (ubuntu: sudo apt install librdkafka-dev)
- cppkafka (currently it is been used as submodule from original github repo)
- TODO: fork it into gradiant github spaces


How to include the library into your application:

1 - Add cuasMQ as submodule:
- mkdir include 
- cd include
- git submodule add git@github.com:vicentefernandes/cuasMQ.git
- git submodule update --init --recursive

2 - Include header files

``


