# Contents
> * Overview
> * Service monitoring principle

# 1. Overview

For all business services based on the Tars(C++), Tars Node can provide real-time monitoring. The general service exits abnormally, and the phenomenon of zombie is actively monitored by the Tars Node, which makes the service run more reliable.

# 2. Service monitoring principle

## 2.1. Overview of service monitoring principles

The Tars Node service will open a monitoring thread to be responsible for timing (interval time can be configured) to poll and monitor of the running status of all services managed by the Tars Node, and to report the running status of the service to the master. If a service meets the relevant trigger conditions, Tars Node takes corresponding measures to manage the service, so that the running status of the service is in real time under the monitoring of Tars Node.

The above monitoring thread mainly includes the following key features:

1.The frequency(in seconds) of monitoring thread going to poll the service.

2.Timeout period (in seconds) for business service heartbeat reporting. If the process of processing a service under a node is time consuming, the user can consider increasing this value.

3.The frequency (in seconds) that the Tars Node reports the running status of the service to the master.

4.Tars Node has two methods to report the running status of the service to the master: single report and batch report. The batch report mode is selected by default. If the Tars Node manages a large number of services, you can choose the batch method to improve efficiency.

## 2.2. Service monitoring strategy

The monitoring thread considers the judgment strategy of several important monitoring points in the process of monitoring the service.

### 2.2.1. A strategy for judging that a service process does not exist

At present, the method for detecting whether the service process exists by the Tars Node is to send a NULL signal to the process of the specified ID through the kill function of the Linux system to determine whether the process exists.

### 2.2.2. A strategy for judging that service has heartbeat timeout

The heartbeat timeout period of the service is configured in the Node. This configuration applies to the heartbeat timeout period of all services managed by the Tars Node.

Each business service reports a heartbeat time to the Tars Node every 10 seconds. The Tars Node checks whether the heartbeat time of each business service has timed out under a certain monitoring frequency.

Tars Node also checks if the heartbeat time of all BindAdapters in each business service times out.

### 2.2.3. A strategy for judging that service is allowed to start

Considering various fault conditions and disaster tolerance characteristics, Tars Node will only send a start command to the service that meets the start condition.

The strategies involved here are:

1.A business service whose current state is in a transition state does not allow restarting, that is, a service in a state of "Activating", "Deactivating", "Destroying", "Loading", "Patching", "BatchPatching", and the like.

2.The business service may be stopped through the O&M interface, that is, the service is manually stopped. At this time, the "internal working status" of the service is set to false. In this case, the Tars Node monitoring thread is not allowed to start the service.

3.Some services may indeed have systemic problems that prevent them from starting successfully. In order to prevent frequent restarts of these services, the Tars Node adopts a"service restart penalty time" strategy for these services: that is, each time the service is restarted, the number of restarts and the restart time will be recorded, and the maximum restart time is N times in X seconds. After N times, the service still cannot be restarted, and then try to restart the service at a frequency of Y seconds.

The default value in the current framework is: within 60 seconds, the service is started up to 10 times. And after 10 times of startup failure, it is retried every 600 seconds.