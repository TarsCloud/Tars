// **********************************************************************
// This file was generated by a TARS parser!
// TARS version 1.0.1.
// **********************************************************************

package com.qq.tars.generated.tars;

import com.qq.tars.protocol.util.*;
import com.qq.tars.protocol.annotation.*;
import com.qq.tars.protocol.tars.*;
import com.qq.tars.protocol.tars.annotation.*;

/**
 * Task任务
 */
@TarsStruct
public class TaskReq {

	@TarsStructProperty(order = 0, isRequire = false)
	public java.util.List<TaskItemReq> taskItemReq = null;
	@TarsStructProperty(order = 1, isRequire = false)
	public String taskNo = "";
	@TarsStructProperty(order = 2, isRequire = false)
	public boolean serial = false;
	@TarsStructProperty(order = 3, isRequire = false)
	public String userName = "";

	public java.util.List<TaskItemReq> getTaskItemReq() {
		return taskItemReq;
	}

	public void setTaskItemReq(java.util.List<TaskItemReq> taskItemReq) {
		this.taskItemReq = taskItemReq;
	}

	public String getTaskNo() {
		return taskNo;
	}

	public void setTaskNo(String taskNo) {
		this.taskNo = taskNo;
	}

	public boolean getSerial() {
		return serial;
	}

	public void setSerial(boolean serial) {
		this.serial = serial;
	}

	public String getUserName() {
		return userName;
	}

	public void setUserName(String userName) {
		this.userName = userName;
	}

	public TaskReq() {
	}

	public TaskReq(java.util.List<TaskItemReq> taskItemReq, String taskNo, boolean serial, String userName) {
		this.taskItemReq = taskItemReq;
		this.taskNo = taskNo;
		this.serial = serial;
		this.userName = userName;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + TarsUtil.hashCode(taskItemReq);
		result = prime * result + TarsUtil.hashCode(taskNo);
		result = prime * result + TarsUtil.hashCode(serial);
		result = prime * result + TarsUtil.hashCode(userName);
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		if (!(obj instanceof TaskReq)) {
			return false;
		}
		TaskReq other = (TaskReq) obj;
		return (
			TarsUtil.equals(taskItemReq, other.taskItemReq) &&
			TarsUtil.equals(taskNo, other.taskNo) &&
			TarsUtil.equals(serial, other.serial) &&
			TarsUtil.equals(userName, other.userName) 
		);
	}

	public void writeTo(TarsOutputStream _os) {
		if (null != taskItemReq) {
			_os.write(taskItemReq, 0);
		}
		if (null != taskNo) {
			_os.write(taskNo, 1);
		}
		_os.write(serial, 2);
		if (null != userName) {
			_os.write(userName, 3);
		}
	}

	public void readFrom(TarsInputStream _is) {
		this.taskItemReq = (java.util.List<TaskItemReq>) _is.read(taskItemReq, 0, false);
		this.taskNo = _is.read(taskNo, 1, false);
		this.serial = _is.read(serial, 2, false);
		this.userName = _is.read(userName, 3, false);
	}

}
