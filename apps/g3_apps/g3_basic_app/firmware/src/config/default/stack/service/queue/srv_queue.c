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
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include "srv_queue.h"
#include "sys_debug.h"

static bool _qmm_is_queue_consistent(SRV_QUEUE *queue)
{
	queue_element_t *p_element;
	uint16_t us_i, us_num_elements;
	bool b_check_pointers;
	bool b_consistent = true;

	/* No data to check consistent */
	if ((q->ini_queue != NULL) && (q->last_element != NULL)) {
		b_check_pointers = true;
	} else {
		b_check_pointers = false;
	}

	/* Check for size 0 */
	if (q->size == 0) {
		if ((q->head != NULL) || (q->tail != NULL)) {
			mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: Non Well Initialized queue 0 size");
			mac_debug_report_error(QMM_QUEUE_NOT_INIT_0);
			b_consistent = false;
		}

		return b_consistent;
	}

	/* Check consistency with elements in the queue*/
	us_num_elements = q->size;
	p_element = q->head;
	for (us_i = 0; us_i < us_num_elements; us_i++) {
		/* Check first element */
		if ((us_i == 0) && (p_element->prev != NULL)) {
			mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: First element bad initialized");
			mac_debug_report_error(QMM_QUEUE_FIRST_BAD_INIT);
			b_consistent = false;
			break;
		}

		/* Check last element */
		if (us_i == (us_num_elements - 1)) {
			if (p_element != q->tail) {
				mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: Latest element different from tail");
				mac_debug_report_error(QMM_QUEUE_LAST_NOT_TAIL);
				b_consistent = false;
				break;
			}

			if (p_element->next != NULL) {
				mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: There are more elements than size");
				mac_debug_report_error(QMM_QUEUE_TOO_BIG);
				b_consistent = false;
				break;
			}
		} else {
			if(b_check_pointers && ((p_element->next < q->ini_queue) || (p_element->next > q->last_element))) {
				mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: Bad next element");
				mac_debug_report_error(QMM_QUEUE_BAD_NEXT_ELEMENT);
				b_consistent = false;
				break;
			}

			if (p_element->next->prev != p_element) {
				mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: Wrongly chained queue");
				mac_debug_report_error(QMM_QUEUE_WRONG_CHAIN);
				b_consistent = false;
				break;
			}
		}

		if (b_check_pointers && ((p_element < q->ini_queue) || (p_element > q->last_element))) {
			mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: Bad element in the queue");
			mac_debug_report_error(QMM_DEBUG_BAD_ELEMENT);
			b_consistent = false;
			break;
		}

		p_element = p_element->next;
	}

	return b_consistent;
}

/**
 * \brief Insert and element in the queue after the last element
 *
 * Insert an element in the queue after the last element
 *
 * \param q The queue which should be initialized.
 * \param q_element element to be inserted at the queue tail
 *
 */
static void _qmm_insert_queue_end(SRV_QUEUE *queue, queue_element_t *q_element)
{
	if (q->size >= q->capacity) {
		/* Buffer cannot be appended as queue is full */
		/* PRIME_DEBUG QUEUE full no resources */
		mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG:Error in _qmm_insert_queue_end: QUEUE_FULL");
		mac_debug_report_error(QMM_QUEUE_FULL_INSERT_END);
		return;
	}

	if (q->mode & DEBUG_QUEUE) {
		if ((q_element < q->ini_queue) || (q_element > q->last_element)) {
			mac_debug_report_error(QMM_DEBUG_BAD_ELEMENT);
			mac_debug_report_error((uint32_t)q_element);
		}

		if (q->tail->next != NULL) {
			mac_debug_report_error(QMM_DEBUG_BAD_TAIL);
		}
	}

	q_element->prev = q->tail;
	/* Terminate the list */
	q_element->next = NULL;
	q_element->prev->next = q_element;
	q->tail = q_element;
	q->size++;
}

/**
 * \brief Modify total capacity of a queue.
 *
 * Modify total capacity of a queue.
 *
 * \param q The queue which should be modified.
 * \param new_capacity
 *
 */
void qmm_set_capacity(SRV_QUEUE *queue, uint16_t us_new_capacity)
{
	/* Capacity can be reduced more than size (number of elements currently present in the queue)
	 * The only consequence is that no more elements will be appended */
		q->capacity = us_new_capacity;
}

/**
 * \brief Insert and element in the queue in the before
 *
 * Insert an element in the queue before the current element of the queue
 *
 * \param q The queue which should be initialized.
 * \param q_current_element position to insert the new element
 * \param q_element element to be inserted at the queue head
 *
 */
void qmm_insert_queue_before(SRV_QUEUE *queue, queue_element_t *q_current_element, queue_element_t *q_element)
{
	if (q->size >= q->capacity) {
		/* Buffer cannot be appended as queue is full */
		/* PRIME_DEBUG QUEUE full no resources */
		mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG:Error in qmm_inser_queue_before: QUEUE_FULL");
		mac_debug_report_error(QMM_QUEUE_FULL_INSERT_BEFORE);
		return;
	}

	if (q_current_element->prev == NULL) {
		q->head = q_element;
		q_element->prev = NULL;
	} else {
		q_current_element->prev->next = q_element;
		q_element->prev = q_current_element->prev;
	}

	q_element->next = q_current_element;
	q_current_element->prev = q_element;
	q->size++;

	_qmm_is_queue_consistent(q);
}

/**
 * \brief Insert and element in the queue after the current element
 *
 * Insert an element in the queue before the current element of the queue
 *
 * \param q The queue which should be initialized.
 * \param q_current_element position to insert the new element
 * \param q_element element to be inserted at the queue head
 *
 */

void qmm_insert_queue_after(SRV_QUEUE *queue, queue_element_t *q_current_element, queue_element_t *q_element)
{
	if (q->size >= q->capacity) {
		/* Buffer cannot be appended as queue is full */
		/* PRIME_DEBUG queue full error  */
		mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG:Error in qmm_insert_queue_after: QUEUE_FULL");
		mac_debug_report_error(QMM_QUEUE_FULL_INSERT_AFTER);
		return;
	}

	if (q_current_element->next == NULL) {
		q->tail = q_element;
		q_element->next = NULL;
	} else {
		q_current_element->next->prev = q_element;
		q_element->next = q_current_element->next;
	}

	q_element->prev = q_current_element;
	q_current_element->next = q_element;
	q->size++;

	_qmm_is_queue_consistent(q);
}

/**
 * \brief Insert the first element of the queue
 *
 * Insert an element in the queue before the current element of the queue
 *
 * \param q              The queue which should be initialized.
 * \param queue_element  Element to be inserted at the queue head
 *
 */
static void _qmm_insert_first_element_queue(SRV_QUEUE *q, queue_element_t *queue_element)
{
	if (q->size >= q->capacity) {
		/* Buffer cannot be appended as queue is full */
		/* PRIME_DEBUG QUEUE full no resources */
		mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG:Error in _qmm_insert_first_element_queue: QUEUE_FULL");
		mac_debug_report_error(QMM_QUEUE_FULL_INSERT_FIRST);
		return;
	}

	if (q->mode & DEBUG_QUEUE) {
		if ((queue_element < q->ini_queue) || (queue_element > q->last_element)) {
			mac_debug_report_error(QMM_DEBUG_BAD_ELEMENT);
			mac_debug_report_error((uint32_t)queue_element);
		}
	}

	queue_element->next = NULL;
	queue_element->prev = NULL;
	/* Add the buffer at the head */
	q->head = queue_element;
	/* Update the list */
	q->tail = queue_element;
	q->size = 1;
}

/*
 * \brief removes a queue element from queue tail
 *
 * This function  removes a queue element from the queue tail and return queue_element
 * return null is queue is empty
 *
 * \param q Queue from which buffer is to be emoved.
 *
 */
static queue_element_t *_qmm_remove_tail(SRV_QUEUE *queue)
{
	queue_element_t *q_element;
	q_element = q->tail;
	if (q->size > 1) {
		q->tail = q->tail->prev;
		q->tail->next = NULL;
	} else {
		/*empty queue */
		q->head = NULL;
		q->tail = NULL;
	}

	/* erase prev and next pointer */
	q_element->prev = NULL;
	q_element->next = NULL;
	q->size--;
	return (q_element);
}

/*
 * \brief removes a queue element from queue head
 *
 * This function  removes a queue element from the queue head and return queue_element
 * return null is queue is empty
 *
 * \param q Queue from which buffer is to be removed.
 *
 */
static queue_element_t *_qmm_remove_head(SRV_QUEUE *queue)
{
	if (q->size == 0) {
		mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG:Error in _qmm_remove_head: QUEUE_EMPTY");
		mac_debug_report_error(QMM_QUEUE_EMPTY_REMOVE_HEAD);
		return NULL;
	}

	queue_element_t *q_element;
	q_element = q->head;
	if (q->size > 1) {
		/* point head to the previous element */
		q->head = q->head->next;
		/* first element prev pointer is null */
		q->head->prev = NULL;
	} else {
		/* empty queue */
		q->head = NULL;
		q->tail = NULL;
	}

	/* erase prev and next pointer */
	q_element->prev = NULL;
	q_element->next = NULL;
	q->size--;
	return (q_element);
}

/*
 * \brief Reads or removes a buffer from queue
 *
 * This function reads or removes a buffer from a queue as per the search criteria provided. If search criteria is NULL, then the first  buffer is returned,
 * otherwise buffer matching the given criteria is returned
 *
 * \param q Queue from which buffer is to be read or removed.
 *
 * \param mode Mode of operations. If this parameter has value REMOVE_MODE, buffer will be removed from queue and returned. If this parameter is READ_MODE,
 * buffer pointer will be returned without removing from queue.
 *
 * \param search Search criteria structure pointer.
 *
 * \return void header pointer, if the queue element is successfully removed or read, otherwise NULL is returned.
 */
queue_element_t *qmm_queue_read_or_remove(SRV_QUEUE *queue, queue_mode_t access_mode, queue_mode_t queue_mode)
{
	queue_element_t *element_current;

	if (q->size == 0) {
		mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG:Error in qmm_queue_read_or_remove: QUEUE_EMPTY");
		mac_debug_report_error(QMM_QUEUE_EMPTY_READ_REMOVE);
		return NULL;
	}

	if (queue_mode == HEAD) {
		/* remove or read first element of the queue */
		if (access_mode == REMOVE_MODE) {
			element_current = _qmm_remove_head(q);
		} else {
			element_current = q->head;
		}
	} else {
		/* remove or read last element of the queue */
		if (access_mode == REMOVE_MODE) {
			element_current = _qmm_remove_tail(q);
		} else {
			element_current = q->tail;
		}
	}

	if (access_mode == REMOVE_MODE) {
		_qmm_is_queue_consistent(q);
	}

	return (element_current);
}

/**
 * \brief Initializes the queue.
 *
 * This function initializes the queue. Note that this function should be called before invoking any other functionality of QMM.
 *
 * \param q The queue which should be initialized.
 * \param capacity  maximum number of element in queue
 * \param  mode queue modes  SINGLE_QUEUE or PRIORITY_QUEUE
 *
 */
void qmm_init(SRV_QUEUE *queue, uint16_t capacity, uint8_t mode, ...)
{
	/* Initialize pointers, mode and capacity */
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	q->capacity = capacity;
	q->mode = mode;

	if (mode & DEBUG_QUEUE) {
		va_list ap;

		va_start(ap, mode);
		q->ini_queue = va_arg(ap, void *);
		q->last_element = va_arg(ap, void *);
		va_end(ap);
	} else {
		q->ini_queue = NULL;
		q->last_element = NULL;
	}
}

void SRV_QUEUE_Append(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element);
{
    SRV_QUEUE_ELEMENT *currentElement;
    uint16_t queueCounter;

    /* Check if element is already in the queue (size > 1) */
    if ((element->next != NULL) || (element->prev != NULL)) 
    {
	/* Do nothing - element is already in queue */
	//TBD mac_debug_generate_warning(MAC_DBG_QMM, "PRIME_DEBUG:Try to append an element already in queue");
	//TBD mac_debug_report_error(QMM_QUEUE_APPEND_AGAIN);
	return;
    }

    /* Check if element is already in the queue (size = 1) */
    if ((queue->size == 1) && (queue->head == element)) 
    {
	/* Do nothing - element is already in queue */
	//TBD mac_debug_generate_warning(MAC_DBG_QMM, "PRIME_DEBUG:Try to append an element already in queue size 1");
	//TBD mac_debug_report_error(QMM_QUEUE_APPEND_AGAIN_1);
	return;
    }

    if (((queue->ini_queue != NULL) && (queue->last_element != NULL)) && 
        ((element < queue->ini_queue) || (element > queue->last_element))) 
    {
	//TBD mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: Trying to append a bad element");
	//TBD mac_debug_report_error(QMM_QUEUE_APPEND_BAD_ELEMENT);
	return;
    }

	/* Check if queue is full */
	if (q->size >= q->capacity) {
		/* Buffer cannot be appended because queue is full QUEUE_FULL */
		mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG:Error in qmm_queue_append: QUEUE_FULL");
		mac_debug_report_error(QMM_QUEUE_FULL_APPEND);
		return;
	} else { /* 1 */
		 /* Check whether queue is empty */
		if (q->size == 0) {
			_qmm_insert_first_element_queue(q, queue_element);
		} else { /* 2 */
			if (q->mode == SINGLE_QUEUE) {
				_qmm_insert_queue_end(q, queue_element);
			} else { /* 3 */
				 /* insert in priority queue */
				 /* search for the firsts element with the same priority and insert at the end */
				uc_counter_queue = q->size + 1;
				element_current = q->tail;
				while (uc_counter_queue != 0) {
					if (queue_element->priority < element_current->priority) {
						if (element_current->prev != NULL) {
							/* move current element to previous element*/
							element_current = element_current->prev;
							uc_counter_queue--;
						} else {
							/* first element of the queue : add element in the begining  of the  queue */
							qmm_insert_queue_before(q, element_current, queue_element);
							break;
						}
					} else {
						qmm_insert_queue_after(q, element_current, queue_element);
						break;
					}
				}
			}
		}
	}

    _is_queue_consistent(queue);
}

void SRV_QUEUE_Append_With_Priority(SRV_QUEUE *queue, uint32_t priority, 
                                    SRV_QUEUE_ELEMENT *element);
{
    if (queue->type != SRV_QUEUE_TYPE_PRIORITY) {
	/* Cannot be added with priority */
	//TBD mac_debug_generate_error(MAC_DBG_QMM, "PRIME_DEBUG: Add with priority in a SINGLE_QUEUE");
	//TBD mac_debug_report_error(QMM_QUEUE_APPEND_PRIO_SINGLE);
	return;
    }

    element->priority = priority;
    SRV_QUEUE_Append(queue, element);
}

void SRV_QUEUE_Remove_Element(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element);
{
    SRV_QUEUE_ELEMENT *currentElement;
    uint16_t i;

    currentElement = queue->head;

    for (i = 1; i <= queue->size; i++) 
    {
	if (currentElement == element) 
        {
            /* Element to be freed found. */
            /* Rebuild next and previous pointers */
            if (i == 1) 
            {
		/* Remove first element of the queue */
		_remove_head(queue);
		break;
            } 
            else if (i == queue->size) 
            {
		/* Remove last element of the queue */
		_remove_tail(queue);
		break;
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
	}
    }

    _is_queue_consistent(queue);
}

SRV_QUEUE_ELEMENT *SRV_QUEUE_Read_Element(SRV_QUEUE *queue, 
                                          uint16_t elementIndex)
{
    SRV_QUEUE_ELEMENT *element;
    uint16_t queueIndex = 0;

    if (queue->size == 0 || elementIndex > queue->size) 
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

/**
 * \brief Internal function for flushing a specific queue
 *
 * \param q Queue to be flushed
 *
 */
void SRV_QUEUE_Flush(SRV_QUEUE *queue)
{
    while (queue->size > 0) 
    {
	/* Remove the buffer from the queue and free it */
	SRV_QUEUE_Read_Or_Remove(queue, SRV_QUEUE_MODE_REMOVE, 
                                 SRV_QUEUE_POSITION_HEAD);
    }
    
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
}
