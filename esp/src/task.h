//
// Created by Joel Neumann on 26.04.24.
//

#ifndef DREA_V2_TASK_H
#define DREA_V2_TASK_H

#include <Arduino.h>

struct TaskHandles
{
	TaskHandle_t task;
	SemaphoreHandle_t semaphore;
};

// Inspired/Copied by https://github.com/scottbez1/smartknob/blob/master/firmware/src/task.h
template<class T>
class Task
{
public:
	Task(const char *name, const uint32_t stack_depth, const UBaseType_t priority,
	     const BaseType_t core_id = tskNO_AFFINITY) : name_(name),
	                                                  stack_depth_(stack_depth),
	                                                  priority_(priority),
	                                                  core_id_(core_id) {}

	virtual ~Task() = default;

	[[nodiscard]] TaskHandle_t getHandle() const
	{
		return task_handle_;
	}

protected:
	void beginTask()
	{
		const BaseType_t result = xTaskCreatePinnedToCore(taskFunction, name_, stack_depth_, this, priority_,
		                                                  &task_handle_, core_id_);
		assert("Failed to create task" && result == pdPASS);
	}

private:
	static void taskFunction(void *params)
	{
		T *t = static_cast<T *>(params);
		t->run();
	}

	const char *name_;
	uint32_t stack_depth_{};
	UBaseType_t priority_{};
	TaskHandle_t task_handle_ = nullptr;
	const BaseType_t core_id_;
};

#endif//DREA_V2_TASK_H
