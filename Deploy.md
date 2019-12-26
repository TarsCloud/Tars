[查看中文版本](Deploy.zh.md)

# Directory
> * [Deployment Brief](#chapter-1)
> * [Source compilation deployment](#chapter-2)
> * [Framework docker deployment](#chapter-3)
> * [K8s Docker deployment](#chapter-4)
> * [K8s integrated deployment](#chapter-5)

# 1 <a id="chapter-1"></a>Deployment Brief

Before introducing the deployment mode of tarsdocker, this paper first introduces several typical deployment modes of tarsdocker, each with advantages and disadvantages, which can be selected according to the actual situation

Tars deployment Basics, Tars consists of MySQL + framework services + business services:
- mysql, Used to store various service information
- Framework service is composed of several services (implemented by C + +) implemented by tars and web management platform (nodejs)
- Business services are services implemented by the development team according to the product requirements. These services can be implemented by C + +, Java, go, nodejs, PHP and other languages

After tars is deployed, from a server perspective, it consists of the following parts:
- MySQL Cluster (master-slave configuration or cluster configuration): usually the master-slave configuration is enough. Normally, even if MySQL is hung, it will not affect the operation of business services (but it will affect deployment and Publishing)
- Framework service: it is composed of multiple C + + implemented tars services + web management platforms, which are usually deployed on two machines. One of them will deploy more web management platforms, tarspatch and tarsaddminregistry. In the process of running, the framework servers are hung, and the operation of business services will not be affected (deployment and publishing will be affected)
- Multiple node servers: from 1 to many, a tarsnode process must be deployed on each node server, and the tarsnode must be connected to the framework service (tarsregistry)
- The services implemented by the business itself are published to these node servers through the web management platform

The deployment of tars includes:
- mysql install
- Framework service deployment (generally two servers)
- The node server deployment (tarsnode deployment) can be done remotely through the web management platform

**Note: if the business services implemented in different languages may be running on the node server, the running environment of the node server usually needs to be installed by itself, such as JDK, node, etc**

There are several ways to deploy tars:
> [Source compilation deployment](#chapter-2)
> [Framework docker deployment](#chapter-3)
> [K8s Docker deployment](#chapter-4)
> [K8s integrated deployment](#chapter-5)

# 2 <a id="chapter-2"></a>Source compilation deployment

Source deployment is a very good way to understand tars. For details of source deployment, see [here](https://github.com/TarsCloud/Tars/blob/master/Install.zh.md), Reading is highly recommended.

The main steps are as follows:
- Install mysql
- Download TarsFramework source code
- Compile
- Download Tars Web
- Deploy with one auto deployment script

Tars framework services run on the server in independent process mode, which can be started and stopped manually, and each service can be updated independently

**Note: this method is recommended for teams familiar with tars**

# 3 <a id="chapter-3"></a>Framework docker deployment

Although the source code deployment can be updated independently, it also brings inconvenience. After all, the update of each module is more troublesome. At the same time, the module version may still depend on it, and the update maintenance is more troublesome.

In this case, you can choose the framework docker deployment: in short, you can container the framework service and the web. When you start the container, the framework service will start automatically and update as a whole

There are three modes of docker making in the tars framework:
- Make docker through source code, See [here](https://github.com/TarsCloud/Tars/blob/master/Install.md) 
- Auto make docker(not include run environment), See [here](https://github.com/TarsCloud/TarsDocker/blob/master/README.md) 
- Auto make docker(include run environment), See [here](https://github.com/TarsCloud/TarsDocker/blob/master/README.md) 

The differences between the source code make and auto make:
- Make docker through source code
>- You need to download the source code manually and make docker step by step
>- The mirror used china mirror, which is easy to be made successfully
>- The created image does not contain the business services of the runtime environment, such as Java, PHP, etc., and cannot be published to the internal of the docker image

- Auto make docker
>- Auto download source code, make, make install and make docker
>- The default source of the system used may be connected to overseas sources, which is slower (but the action is faster when it is created on GitHub)

# 4 <a id="chapter-4"></a>K8s Docker deployment

Although the deployment of docker framework greatly facilitates the deployment of the framework, there is still a lot of work to be done for the team using k8s to manage containers.

Therefore, here, a way to deploy tars on k8s is provided:
- Container framework services, use tarscloud/framework or tarscloud/tars
- Container tarsnode services, use tarscloud/tarsnoe
- tars framework and tarsnode node are deployed on k8s as pod, and the container of pod is a virtual machine
- Publish services to these containers through tar Web

Please See [here](https://github.com/TarsCloud/TarsDocker/blob/master/README.md) 

**Note: the internal docker provides the running environment of various languages. Simply speaking, the business service can be published to the internal docker for running (equivalent to treating the docker as a virtual machine)**

Although this method does not make every service a pod and run on k8s independently, it also basically solves the problem of combining tar with k8s. Although it is not elegant, it can be used as a way integrated with k8s.

The core problem that needs to be solved in this mode is that both framework services and tarsnode, as pods, may die and drift, resulting in IP changes. How to solve this problem:
- All pods can be deployed in the stateful headless mode, each with its own domain name, such as tar-0, tar-1, tar node-1, tar node-2, etc
- When tar service is deployed, the domain name (not IP) is used for all business nodes. 

# 5 <a id="chapter-5"></a>K8s integrated deployment

In the above k8s docker deployment, although tars is deployed on k8s to run, k8s is not used for actual release and capacity expansion, but k8s is regarded as a container management platform

The deep integration of tars and k8s is still in planning and development
