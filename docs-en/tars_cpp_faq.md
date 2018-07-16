
1. How to build the Tars C++ development environment?
> * Please reference tars_install.md

2. How to quickly implement an example of a Tars C++?
> * For documentation, please refer to tars_cpp_quickstart.md, the relevant sample code, and the cpp/examples directory.

3. What is the tars/tup protocol?
> * For specific information, please refer to tars_tup.md.

4. Does Tars C++ support custom protocols (such as: HTTP)?
> * In addition to supporting tars/tup protocol, Tars C++ also supports business custom protocols. Please refer to tars_cpp_thirdprotocol.md for more details.

5. How does Tars C++ pull the business configuration file?
> * Tars C++ supports obtaining the specified business configuration file by using the addConfig method. The specific usage of business configuration can be referred to tars_config.md.

6. How is the service running in the Tars framework monitored?
> * For specific information, please refer to tars_server_monitor.md

7. How to create a Tars C++ communicator?
> * If the service is based on the TAF framework, please get it directly from Applicatin and do not create it by yourself. For example:
```
Application::getCommunicator()->stringToProxy<NotifyPrx>(ServerConfig::Notify);
```
> * If the service is not based on the TAF framework, just the TAF client, you can initialize communicator with new Communicator (CONF). For example:
``` 
TC_Config conf(“config.conf”);
CommunicatorPtr c = new Communicator(conf);
```

8. How to set the timeout time of the Tars C++ invoke?
> * For more details, please refer to tars_cpp_user_guide.md

9. After the service is released but not running, how can we find out the reason?
> * Whether the service configuration file is not properly obtained. For example, the configuration file name configured on the web platform is inconsistent with the configuration file name that was downloaded from the program add.
> * When looking for a problem, first look at whether the "service current report" on web is prompted to be normal, for example: a configuration file has a configuration file that has not been downloaded successfully, and the service is downloaded when it is started.
> * Look for the log log that you print for the service. Logs are usually in the /usr/local/app/tars/app_log/ application name/service name/directory.
> * If there is still a problem, check the node log.

10. How does the core file be opened and where is it generated?
> * The core file is opened when the tafnode startup script is added to the ulimite -c unlimited, and the current core file is generated under the /usr/local/app/tars/app_log directory.

11. Does the failure of master Registry affect the normal access of business services?
> * Without affecting the normal access to business services, the bottom of the framework will cache the IP list of the backend services.

12. How does the communicator Communicator get ip:prot through ServerName?
> * 1.When the agent is created, it does not ask the master to get the IP list. Instead, it triggers the request area to get the master IP list when calling the proxy's interface.
> * 2.If there is IP list information behind obj, it is equivalent to direct connection. This will not ask for master registry.
> * 3.If there is no IP list information behind obj, it is quite indirect and will ask for master registry.
> * 4.The strategy for requesting master registry: if the local cache has a list of IP for the requested obj, use a local cache of IP lists, and also asynchronously request the master registry to get the latest IP list
> * 5.The strategy of requesting master registry: if there is no IP list in the local cache that has no cache request obj, the request of the business will be put into the queue, and the master control is requested to get the IP list asynchronously, after getting the IP list, then the request of the service is taken out from the queue and the request is sent.
> * 6.Instead of refreshing each time and refreshing at regular intervals (default 60s), the trigger for timing refresh is business request.
