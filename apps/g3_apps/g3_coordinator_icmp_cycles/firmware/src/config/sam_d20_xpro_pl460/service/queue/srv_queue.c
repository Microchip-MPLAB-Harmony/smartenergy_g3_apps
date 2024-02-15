/*******************************************************************************
  Source for the queue management module

  Company:
    Microchip Technology Inc.

  File Name:
    srv_queue.c

  Summary:
    Implementation of the queue management module.

  Description:
    This file contains the source code for the implementation of the queue
    management module.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "srv_queue.h"
#include "service/log_report/srv_log_report.h"

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

static void lSRV_QUEUE_Insert_Last_Element(SRV_QUEUE *queue,
                                           SRV_QUEUE_ELEMENT *element)
{
    if (queue->size >= queue->capacity)
    {
        /* Buffer cannot be appended as queue is full */
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_FULL_INSERT_END,
            "Error in lSRV_QUEUE_Insert_Last_Element: QUEUE_FULL\r\n");
        return;
    }

    if (queue->tail->next != NULL)
    {
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_BAD_TAIL,
            "Bad element in tail\r\n");
    }

    element->prev = queue->tail;
    /* Add the element at the end */
    element->next = NULL;
    element->prev->next = element;
    queue->tail = element;
    queue->size++;
}

static void lSRV_QUEUE_Insert_First_Element(SRV_QUEUE *queue,
                                            SRV_QUEUE_ELEMENT *element)
{
    if (queue->size >= queue->capacity)
    {
        /* Buffer cannot be appended as queue is full */
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_FULL_INSERT_FIRST,
            "Error in lSRV_QUEUE_Insert_First_Element: QUEUE_FULL\r\n");
        return;
    }

    element->next = NULL;
    element->prev = NULL;
    /* Add the element at the beginning */
    queue->head = element;
    /* Update the list */
    queue->tail = element;
    queue->size = 1;
}

static SRV_QUEUE_ELEMENT *lSRV_QUEUE_Remove_Tail(SRV_QUEUE *queue)
{
    SRV_QUEUE_ELEMENT *element;

    element = queue->tail;

    if (queue->size > 1U)
    {
        queue->tail = queue->tail->prev;
        queue->tail->next = NULL;
    }
    else
    {
        /* Empty queue */
        queue->head = NULL;
        queue->tail = NULL;
    }

    /* Clear previous and next pointers */
    element->prev = NULL;
    element->next = NULL;
    queue->size--;

    return (element);
}

static SRV_QUEUE_ELEMENT *lSRV_QUEUE_Remove_Head(SRV_QUEUE *queue)
{
    if (queue->size == 0U)
    {
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_EMPTY_REMOVE_HEAD,
            "Error in lSRV_QUEUE_Remove_Head: QUEUE_EMPTY\r\n");
        return NULL;
    }

    SRV_QUEUE_ELEMENT *element;

    element = queue->head;

    if (queue->size > 1U)
    {
        /* Point head to the previous element */
        queue->head = queue->head->next;
        /* First element previous pointer is null */
        queue->head->prev = NULL;
    }
    else
    {
        /* Empty queue */
        queue->head = NULL;
        queue->tail = NULL;
    }

    /* Clear previous and next pointers */
    element->prev = NULL;
    element->next = NULL;
    queue->size--;

    return (element);
}

// *****************************************************************************
// *****************************************************************************
// Section: Common Interface Implementation
// *****************************************************************************
// *****************************************************************************

void SRV_QUEUE_Init(SRV_QUEUE *queue, uint16_t capacity, SRV_QUEUE_TYPE type)
{
    /* Initialize pointers, type and capacity */
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->capacity = capacity;
    queue->type = type;

}

void SRV_QUEUE_Append(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element)
{
    SRV_QUEUE_ELEMENT *currentElement;
    uint16_t queueCounter;

    /* Check if element is already in the queue (size = 1) */
    if ((queue->size == 1U) && (queue->head == element))
    {
        /* Do nothing - element is already in queue */
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_WARNING, QUEUE_APPEND_AGAIN_ONE_ELEMENT,
            "Trying to append an element already in a queue with size 1");
        return;
    }

    /* Check if queue is full */
    if (queue->size >= queue->capacity)
    {
        /* Element cannot be appended because queue is full */
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_FULL_APPEND,
            "Error in SRV_QUEUE_Append: QUEUE_FULL\r\n");
        return;
    }
    else
    {
        /* Check whether queue is empty */
        if (queue->size == 0U)
        {
            lSRV_QUEUE_Insert_First_Element(queue, element);
        }
        else
        {
            if (queue->type == SRV_QUEUE_TYPE_SINGLE)
            {
                lSRV_QUEUE_Insert_Last_Element(queue, element);
            }
            else
            {
                /* Insert in priority queue */
                /* Search for the first element with the same priority */
                /* and insert at the end */
                queueCounter = queue->size + 1U;
                currentElement = queue->tail;
                while (queueCounter != 0U)
                {
                    if (element->priority < currentElement->priority)
                    {
                        if (currentElement->prev != NULL)
                        {
                            /* Move current element to previous element*/
                            currentElement = currentElement->prev;
                            queueCounter--;
                        }
                        else
                        {
                            /* First element of the queue */
                            /* Add element at the beginning*/
                            SRV_QUEUE_Insert_Before(queue, currentElement,
                                                    element);
                            break;
                        }
                    }
                    else
                    {
                        SRV_QUEUE_Insert_After(queue, currentElement, element);
                        break;
                    }
               }
            }
        }
    }
}

void SRV_QUEUE_Append_With_Priority(SRV_QUEUE *queue, uint32_t priority,
                                    SRV_QUEUE_ELEMENT *element)
{
    if (queue->type != SRV_QUEUE_TYPE_PRIORITY)
    {
        /* Cannot be added with priority */
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_APPEND_PRIO_SINGLE,
            "Error adding with priority in a SINGLE_QUEUE\r\n");
        return;
    }

    element->priority = priority;
    SRV_QUEUE_Append(queue, element);
}

void SRV_QUEUE_Remove_Element(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element)
{
    SRV_QUEUE_ELEMENT *currentElement;
    uint16_t i = 1U;

    currentElement = queue->head;

    while (i <= queue->size)
    {
        if (currentElement == element)
        {
            /* Element to be freed found. */
            /* Rebuild next and previous pointers */
            if (i == 1U)
            {
                /* Remove first element of the queue */
                (void) lSRV_QUEUE_Remove_Head(queue);
            }
            else if (i == queue->size)
            {
                /* Remove last element of the queue */
                (void) lSRV_QUEUE_Remove_Tail(queue);
            }
            else
            {
                /* Remove object in the middle of the queue */
                currentElement->prev->next = currentElement->next;
                currentElement->next->prev = currentElement->prev;
                currentElement->next = NULL;
                currentElement->prev = NULL;
                /* Decrement number of elements in queue */
                queue->size--;
            }

            break;
        }
        else
        {
            /* Current element points to next element in the queue */
            currentElement = currentElement->next;
            i++;
        }
    }
}

SRV_QUEUE_ELEMENT *SRV_QUEUE_Read_Element(SRV_QUEUE *queue,
                                          uint16_t elementIndex)
{
    SRV_QUEUE_ELEMENT *element;
    uint16_t queueIndex = 0U;

    if ((queue->size == 0U) || (elementIndex > queue->size))
    {
        return NULL;
    }

    element = queue->head;

    while (queueIndex < queue->size)
    {
        if (queueIndex < elementIndex)
        {
            element = element->next;
            queueIndex++;
        }
        else
        {
            break;
        }
    }

    return element;
}

void SRV_QUEUE_Insert_Before(SRV_QUEUE *queue,
                             SRV_QUEUE_ELEMENT *currentElement,
                             SRV_QUEUE_ELEMENT *element)
{
    if (queue->size >= queue->capacity)
    {
        /* Buffer cannot be appended as queue is full */
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_FULL_INSERT_BEFORE,
            "Error in SRV_QUEUE_Insert_Before: QUEUE_FULL\r\n" );
        return;
    }

    if (currentElement->prev == NULL)
    {
        queue->head = element;
        element->prev = NULL;
    }
    else
    {
        currentElement->prev->next = element;
        element->prev = currentElement->prev;
    }

    element->next = currentElement;
    currentElement->prev = element;
    queue->size++;
}

void SRV_QUEUE_Insert_After(SRV_QUEUE *queue,
                            SRV_QUEUE_ELEMENT *currentElement,
                            SRV_QUEUE_ELEMENT *element)
{
    if (queue->size >= queue->capacity)
    {
        /* Buffer cannot be appended as queue is full */
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_FULL_INSERT_AFTER,
            "Error in SRV_QUEUE_Insert_After: QUEUE_FULL\r\n");
        return;
    }

    if (currentElement->next == NULL)
    {
        queue->tail = element;
        element->next = NULL;
    }
    else
    {
        currentElement->next->prev = element;
        element->next = currentElement->next;
    }

    element->prev = currentElement;
    currentElement->next = element;
    queue->size++;
}

SRV_QUEUE_ELEMENT *SRV_QUEUE_Read_Or_Remove(SRV_QUEUE *queue,
                                            SRV_QUEUE_MODE accessMode,
                                            SRV_QUEUE_POSITION position)
{
    SRV_QUEUE_ELEMENT *currentElement;

    if (queue->size == 0U)
    {
        SRV_LOG_REPORT_Message_With_Code(SRV_LOG_REPORT_ERROR, QUEUE_EMPTY_READ_REMOVE,
            "Error in SRV_QUEUE_Read_Or_Remove: QUEUE_EMPTY\r\n");
        return NULL;
    }

    if (position == SRV_QUEUE_POSITION_HEAD)
    {
        /* Remove or read first element of the queue */
        if (accessMode == SRV_QUEUE_MODE_REMOVE)
        {
            currentElement = lSRV_QUEUE_Remove_Head(queue);
        }
        else
        {
            currentElement = queue->head;
        }
    }
    else
    {
        /* Remove or read last element of the queue */
        if (accessMode == SRV_QUEUE_MODE_REMOVE)
        {
            currentElement = lSRV_QUEUE_Remove_Tail(queue);
        }
        else
        {
            currentElement = queue->tail;
        }
    }

    return (currentElement);
}

void SRV_QUEUE_Flush(SRV_QUEUE *queue)
{
    while (queue->size > 0U)
    {
        /* Remove the buffer from the queue and free it */
        (void) SRV_QUEUE_Read_Or_Remove(queue, SRV_QUEUE_MODE_REMOVE,
                                 SRV_QUEUE_POSITION_HEAD);
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0U;
}

void SRV_QUEUE_Set_Capacity(SRV_QUEUE *queue, uint16_t capacity)
{
    /* Capacity can be reduced more than size (number of elements currently */
    /* present in the queue) */
    /* The only consequence is that no more elements will be appended */
    queue->capacity = capacity;
}
