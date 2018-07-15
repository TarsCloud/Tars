# Contents
> * Overview
> * Service thread type of Tars(C++) service
> * Analyzing the tasks of running threads
> * Method of changing the number of threads

# 1. Overview

The Tars(C++) service is a single-process, multi-threaded RPC system. This article describes the number of threads that a standard Tars(C++) service starts, and the responsibilities of each thread.

# 2. Service thread type of Tars(C++) service

Starter |Thread function|Number of threads
------|--------|-----
SERVER |Server main thread, responsible for server initialization|1
SERVER|Server network thread, responsible for sending and receiving data packets on the server (the number of threads is configurable)|Configurable
SERVER|Server-side thread that listens on the port to run business logic, responsible for accepting and processing user-defined commands, service shutdown commands, etc.|1
SERVER|Thread that assists the user in getting time, responsible for periodically calculating time and reducing system calls to gettimeofday|1
SERVER|Thread used to scroll the log, responsible for local log file creation and log writing|1
SERVER|Local log thread, responsible for file creation and log writing of local dyed logs (This thread will start if there is a daily print log or related initialization.)|1
SERVER|Remote log thread, responsible for synchronizing the local dyed log to the remote (This thread will start if there is a daily print log or related initialization.)|1
SERVER|Business processing thread, responsible for processing user business logic and completing the main functions of the RPC service. (Each ServantObj has its own business processing threads by default, but the business processing threads can also be shared.)|Configurable
Communicator|Client network thread, responsible for managing socket connections, listening for read and write events, and completing network read and write|Configurable
Communicator|Thread for statistical attribute reporting, responsible for collecting statistics and attribute information, and timing synchronization to stat and property.|1
Communicator|Asynchronous callback thread, responsible for executing asynchronous callback function, each network thread of the client has its own asynchronous thread|Configurable

# 3. Analyzing the tasks of running threads

From a running Tars service application, we look at the characteristics of each thread and make a distinction between the various thread functions.

Experimental scene:

> * The server is configured with one ServantObj, which is configured with five business processing threads.

> * The server is configured with one network thread.

> * The communicator is configured with two network threads.

> * The communicator is configured with two asynchronous callback thread.

According to the thread startup configuration described above, this service should have:

7(fixed) + 1(Number of server's network threads) + 5(Number of business processing threads) + 2(Number of communicator's network threads) + 2(Number of communicator's asynchronous callback threads) * 2(Number of communicator's network threads) = 19 threads.

# 4. Method of changing the number of threads

The above shows which thread is composed of a standard Tars(C++) service application. Any thread that indicates a number of 1 is implemented internally by the Tars(C++), and the user cannot change its number.

The number of threads that can be changed are server's network threads, business processing threads, communicator's network threads, and communicator's asynchronous callback threads.

## 4.1. Method of changing the number of business processing threads

If you want to change the number of business processing threads of a certain servant object on the server, you can fill in the number N in the "Number of threads" column when the servant object is configured on the Tars management platform, then the Tars(C++) service application will start N business processing threads for the servant.


Note:

If the service has two Servant objects, each belonging to a different business processing thread group, when calculating the number of business threads, we only need to simply add the number of business processing threads of different Servant objects.

If you set up a shared business processing thread group, the calculation method is different.

For example:

ServantA belongs to the business processing thread group HandleGroupA and starts 10 business processing threads

ServantB and ServantC belong to the business processing thread group HandleGroupBC and start 10 business processing  threads.

Then the total number of business processing threads should be: 10 + 10


## 4.2. Method for changing server's network thread, communicator's network thread, and asynchronous callback processing thread

If you want to change the number of server's network threads, communicator's network threads, and asynchronous callback threads, you can modify them on the template or add the corresponding service's private template.