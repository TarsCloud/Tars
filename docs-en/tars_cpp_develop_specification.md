# Table of Contents
> * Naming convention
> * Tars File Directory Specification
> * Makefile specification

# 1. Naming convention

## 1.1. Service naming

APP: The application name, which identifies a small collection of services. In the Tars system, the application name must be unique. For example: TestApp.

Server: service name, the name of the process that provides the service. The server name is named according to the service service function. It is generally named: XXServer, such as LogServer, TimerServer, etc.

Servant: A service provider that provides an interface or instance of a specific service. For example: HelloImp

Description:

A Server can contain multiple Servant, the system will use the service's App + Server + Servant, combined to define the routing name of the service in the system, called routing Obj, its name must be unique in the whole system, so that when you serve externally, you can uniquely identify yourself.

Therefore, when defining an APP, you need to pay attention to the uniqueness of the APP.

For example: Comm.TimerServer.TimerObj, Comm.LogServer.LogServerObj, etc.;

## 1.2. Namespace naming

Each business has a different name, the Application name, which is also used as the namespace for all code under the Application.

Therefore, the namespace is generally the name of the business, for example:
```
namespace Comm
```
## 1.3. Class name (interface name)

The name of the class must consist of one or more words or abbreviations that express the meaning of the class. The first letter of the word is capitalized.

E.g:
```
class HelloWorldApp
```

## 1.4. Method naming

The naming of functions is based on the principle of expressing the action of a function. It is usually started by a verb and then followed by a noun that represents the action object. Beginning with a lowercase letter, the first letter of each word is capitalized.

In addition, there are some general function naming rules.

Get number start with 'get', then keep up with the name of the object to be fetched;

Setting start with 'set', and then keep up with the name of the object to be set;

The function that responds to the message in the object, starting with 'on', and then the name of the corresponding message;

The function that performs the active action can start with 'do', and then the corresponding action name;

Use 'has' or 'can' instead of the 'is' prefix of the boolean get function, the meaning is more clear.

E.g:
```
getNumber();

setNumber();

onProcess();

doAddFile();

hasFile();

canPrint();

sendMessage();
```

## 1.5. Variable naming rules

For the definition of various variables, there is one thing in common, that is, you should use meaningful English words or English word abbreviations, do not use simple meaningless strings, try not to use Arabic numerals, and should not use the initial of Chinese Pinyin.

Names like this are not recommended: Value1, Value2, Value3, Value4....

The general rule is: lowercase letters begin, followed by each word whose first letter is uppercase, usually a noun. (if there is only one word, lowercase)

userNo (mobile number), station (province), destNo (destination number), srcNo (source number), etc.

Other: For some more important numbers, it is best to use constant substitution instead of writing numbers directly. The constants are all uppercase, and multiple words are separated by underscores.

NUMBER_OF_GIRLFRIENDS

# 2. Tars file directory specification

The Tars file is the protocol communication interface of the TARS service, so it is very important to be managed in accordance with the following specifications:

The tars file is in principle placed with the corresponding server;

Each server creates a /home/tarsproto/[namespace]/[server] subdirectory on the development machine;

All tars files need to be updated to the corresponding server directory under /home/tarsproto;

When using other server tars files, you need to use them in /home/tarsproto, you can't copy them to this directory, see Makefile specification;

The tars interface can only be added in principle and cannot be reduced or modified.

Run 'make release' inside the makefile will automatically complete the relevant operations, see the Makefile specification;

Description:

'make release' will copy the tars file to /home/tarsproto/[namespace]/[server] directory, generate tars2cpp to generate .h, and generate a [server].mk file; When others call this service, include this mk file at the bottom of the makefile.

# 3. Makefile specification

It is highly recommended to use the Makefile specification when use service that realized with Tars.

The TARS framework provides a basic Makefile for makefile.tars. The service written in Tars contains the Makefile to help you maintain the Makefile.

The TARS framework also provides a script (installation directory /script/create_tars_server.sh) to automatically generate an empty service framework and Makefile;

## 3.1. Makefile usage principle

In principle, a directory can only be a Server or a program, that is, a Makefile can only have one Target;

When you need to include other libraries, include them in the bottom of the Makefile according to the dependency order;

E.g:
```
include /home/tarsproto/TestApp/HelloServer/HelloServer.mk
include /usr/local/tars/cpp/makefile/makefile.tars
```
Makefile.tars must be included.

## 3.2. Makefile template explanation

APP: the name space of the program (ie Application)

TARGET: Server name;

CONFIG: The name of the configuration file. When you make tar, the file is included in the tarball.

INCLUDE: Other paths that need to be included;

LIB: Required Library

The Makefile for Test.HelloServer is as follows:
```
#------------------------------------------------- ----------------------

APP            := TestApp
TARGET         := HelloServer
CONFIG         := HelloServer.conf
STRIP_FLAG     := N
TARS2CPP_FLAG  :=

INCLUDE        +=
LIB            +=

#------------------------------------------------- ----------------------

Include /home/tarsproto/TestApp/HelloServer/HelloServer.mk
Include /usr/local/tars/cpp/makefile/makefile.tars
```
The key variables are usually not used, but the business can add its own values after these variables:
```
RELEASE_TARS: You need to publish the tars file in the /home/tarsproto/ directory. If you need to publish your own .h to /home/tarsproto, you can do the following:
RELEASE_TARS += xxx.h
CONFIG: The name of the configuration file. In fact, you can add the files you need later, so that the file will be included in the tarball when you call make tar.
```
For other variables, please read makefile.tars.

## 3.3. Makefile use

make help: You can see all the functions of the makefile.

make tar: generate a release file

make release: copy the tars file to the /home/tarsproto directory and automatically generate the relevant mk file

make clean: clear

make cleanall: clear all
