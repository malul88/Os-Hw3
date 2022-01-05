//
// Created by adi.amuzig on 09/12/2020.
//

#ifndef PART3_EVENT_DATA_H
#define PART3_EVENT_DATA_H

#include "priority_queue.h"
#include "date.h"
#include <stdbool.h>

typedef struct EventData_t *EventData;

PQElement eventDataCreate(int event_id, char* name, Date date);

void eventDataDestroy(PQElement event_data);

PQElement eventDataCopy(PQElement event_data);

bool eventDataEqual(PQElement event_data1, PQElement event_data2);

int getEventId(PQElement event_data);

char* getEventName(PQElement event_data);

Date getEventDate(PQElement event_data);

bool eventDataCheckMemberExists(PQElement event_data, int member_id);

PriorityQueueResult eventDataAddMember(PQElement event_data,
                                       PQElement member_data);

PriorityQueue getEventDataMembers(EventData event_data);

PriorityQueueResult eventDataRemoveMember(PQElement event_data,
                                          PQElement member_data);

bool eventDataChangeDate(EventData event_data, Date new_date);

#endif //PART3_EVENT_DATA_H
