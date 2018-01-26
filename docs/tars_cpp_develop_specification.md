# 目录
> * 命名规范
> * Tars文件目录规范
> * Makefile规范

# 1. 命名规范

## 1.1.	服务命名

APP：应用名，标识一组服务的一个小集合，在Tars系统中，应用名必须唯一。例如：TestApp。

Server：服务名，提供服务的进程名称，Server名字根据业务服务功能命名，一般命名为：XXServer，例如LogServer，TimerServer等；

Servant：服务者，提供具体服务的接口或实例。例如:HelloImp

说明：

一个Server可以包含多个Servant，系统会使用服务的App + Server + Servant，进行组合，来定义服务在系统中的路由名称，称为路由Obj，其名称在整个系统中必须是唯一的，以便在对外服务时，能唯一标识自身。

因此在定义APP时，需要注意APP的唯一性。

例如：Comm.TimerServer.TimerObj，Comm.LogServer.LogServerObj等；

## 1.2. Namespace命名

每个业务都有一个不同的名称，即Application名称，该名称也做为该Application下面所有代码的namespace。

因此namespace一般为业务的名称，例如：
```
namespace Comm
```

## 1.3.	class命名(接口名)

class 的名字必须由一个或多个能表达该类的意思的单词或缩写组成，单词首字母大写。

例如：
```
class HelloWorldApp
```

## 1.4.	方法命名

函数的命名是以能表达函数的动作意义为原则的，一般是由动词打头，然后跟上表示动作对象的名词。小写字母开头，后面每个单词首字母大写。

另外，还有一些通用的函数命名规则。

取数用get打头，然后跟上要取的对象的名字；

设置用set打头，然后跟上要设的对象的名字；

对象中响应消息进行动作的函数，以on打头，然后是相应的消息的名称；

进行主动动作的函数，可以命名为do打头，然后是相应的动作名称；

用 has 或者 can 来代替布尔型获取函数的 is 前缀，意义更加明确。

例如：
```
getNumber(); 

setNumber(); 
      
onProcess(); 

doAddFile();

hasFile();

canPrint();

sendMessage(); 
```

## 1.5.	变量命名规则

对于各种变量的定义，都有一个共同点，就是应该使用有实际意义的英文单词或英文单词缩写，不要使用简单的没有意义的字串，尽可能不使用阿拉伯数字，更切忌使用中文拼音的首字母。

如这样的名称是不提倡的：Value1,Value2,Value3,Value4…。

一般规则为：小写字母开头,后面每个单词的首字母大写,一般为名词。（如果只有一个单词，则小写）

userNo（手机号）、station（省份）、destNo（目的号码）、srcNo（源号码）等等

其他: 对于一些比较重要的数，最好用常量替代，而不要直接写数，常量全部大写，多个单词之间以下划线分隔开。

NUMBER_OF_GIRLFRIENDS

# 2. Tars文件目录规范

Tars文件是TARS服务的协议通信接口，因此非常重要，在管理上必须按照以下规范：

tars文件原则上和相应的server放在一起；

每个server在开发机上建立/home/tarsproto/[namespace]/[server]子目录；

所有tars文件需要更新到/home/tarsproto下相应server的目录；

使用其他server的tars文件时，需要到/home/tarsproto中使用，不能copy到本目录下，见Makefile规范；

tars的接口原则上只能增加，不能减少或修改；

makefile里面运行make release会自动完成相关操作，详见Makefile规范；

说明：

make release会将tars文件copy到/home/tarsproto/[namespace]/[server]目录下，同时生成调用tars2cpp生成.h, 并生成一个[server].mk文件；其他调用该服务时，在makefile底部包含这个mk文件即可。

# 3. Makefile规范

使用Tars实现的服务，强烈建议使用该Makefile规范。

TARS框架提供了一个makefile.tars的基础Makefile，采用Tars编写的服务包含该Makefile会有效的帮助你对Makefile的维护；

TARS框架也提供了脚本(安装目录/script/create_tars_server.sh)可以自动生成空的服务框架和Makefile；

## 3.1. Makefile使用原则

原则上一个目录只能是一个Server或者程序，即Makefile只能有一个Target；

需要包含其他库时，根据依赖关系倒序include在Makefile文件底部；

例如：
```
include /home/tarsproto/TestApp/HelloServer/HelloServer.mk
include /usr/local/tars/cpp/makefile/makefile.tars
```
makefile.tars必须包含。

## 3.2. Makefile模板解释

APP：程序的名字空间（即Application）

TARGET：Server名称；

CONFIG：配置文件名称，make tar时将该文件包含在tar包中；

INCLUDE：其他需要包含的路径；

LIB： 需要的库

Test.HelloServer的makefile实例如下：
```
#-----------------------------------------------------------------------

APP           := TestApp
TARGET        := HelloServer
CONFIG        := HelloServer.conf
STRIP_FLAG    := N
TARS2CPP_FLAG  :=

INCLUDE       += 
LIB           += 

#-----------------------------------------------------------------------

include /home/tarsproto/TestApp/HelloServer/HelloServer.mk
include /usr/local/tars/cpp/makefile/makefile.tars
```
关键的变量，通常不用，但是业务可以在这些变量后面添加自己的值：
```
RELEASE_TARS：需要发布在/home/tarsproto/目录下面的tars文件，如果需要把自己.h也发布到/home/tarsproto则可以如下：
RELEASE_TARS += xxx.h
CONFIG：配置文件名称，其实后面可以增加自己需要的文件，这样在调用make tar时也会把该文件包含到tar包中；
```
其他变量请阅读makefile.tars。

## 3.3.	Makefile使用

make help：可以看到makefile所有使用功能。

make tar：生成发布文件

make release：copy tars文件到/home/tarsproto相应目录，并自动生成相关的mk文件

make clean：清除

make cleanall：清除所有
