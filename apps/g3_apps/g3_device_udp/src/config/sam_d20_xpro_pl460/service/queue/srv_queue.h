/*******************************************************************************
  Header for the queue management module

  Company:
    Microchip Technology Inc.

  File Name:
    srv_queue.h

  Summary:
    Interface definition of the queue management module.

  Description:
    This file defines the interface for the queue management module.
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

#ifndef SRV_QUEUE_H
#define SRV_QUEUE_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Queue types

 Summary:
    List of possible queue types.

 Description:
    This type defines the possible queue types.

 Remarks:
    None.
*/

typedef enum
{
    SRV_QUEUE_TYPE_SINGLE     = 0,
    SRV_QUEUE_TYPE_PRIORITY   = 1,
} SRV_QUEUE_TYPE;

// *****************************************************************************
/* Queue access modes

 Summary:
    List of possible queue access modes.

 Description:
    This type defines the possible queue access modes.

 Remarks:
    None.
*/

typedef enum
{
    SRV_QUEUE_MODE_REMOVE   = 0,
    SRV_QUEUE_MODE_READ     = 1,
} SRV_QUEUE_MODE;

// *****************************************************************************
/* Queue positions

 Summary:
    List of possible queue positions.

 Description:
    This type defines the possible queue positions.

 Remarks:
    None.
*/

typedef enum
{
    SRV_QUEUE_POSITION_HEAD   = 0,
    SRV_QUEUE_POSITION_TAIL   = 1,
} SRV_QUEUE_POSITION;

// *****************************************************************************
/* Queue element

  Summary:
    Queue element.

  Description:
    This structure contains the data stored in each queue element.

  Remarks:
    None.
*/
typedef struct SRV_QUEUE_ELEMENT_tag
{
    /* Pointer to the previous object of the queue */
    struct SRV_QUEUE_ELEMENT_tag *prev;

    /* Pointer to the next object of the queue */
    struct SRV_QUEUE_ELEMENT_tag *next;

    /* Element priority (only with priority queues) */
    uint32_t priority;
} SRV_QUEUE_ELEMENT;

// *****************************************************************************
/* Queue data

  Summary:
    Queue data.

  Description:
    This structure contains the common data stored in a queue.

  Remarks:
    None.
*/
typedef struct
{
    /* Pointer to the head of the queue */
    SRV_QUEUE_ELEMENT *head;

    /* Pointer to the tail of the queue */
    SRV_QUEUE_ELEMENT *tail;

    /* Queue capacity */
    uint16_t capacity;

    /* Queue size */
    uint16_t size;

    /* Queue mode (single or priority queue) */
    SRV_QUEUE_TYPE type;
} SRV_QUEUE;

// *****************************************************************************
// *****************************************************************************
// Section: Service Interface Functions
// *****************************************************************************
// *****************************************************************************

/***************************************************************************
  Function:
    void SRV_QUEUE_Init(SRV_QUEUE *queue, uint16_t capacity, SRV_QUEUE_TYPE type)

  Summary:
    Initializes a queue.

  Description:
    This function initializes a queue.

  Precondition:
    None.

  Parameters:
    queue          - Pointer to the queue to be initialized.
    capacity       - Maximum number of elements in the queue.
    type           - Queue type (single or priority queue).

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    static SRV_QUEUE nodeQueue;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    </code>

  Remarks:
    This function must be called before invoking any other functionality of
    the queue management module.
*/
void SRV_QUEUE_Init(SRV_QUEUE *queue, uint16_t capacity, SRV_QUEUE_TYPE type);

/***************************************************************************
  Function:
    void SRV_QUEUE_Append(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element)

  Summary:
    Appends an element into a queue.

  Description:
    This function appends an element into a queue.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue          - Pointer to the queue where to append the element.
    element        - Pointer to the element to be appended.

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static NODE_INFO nodeInfo;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo);
    </code>

  Remarks:
    None.
*/
void SRV_QUEUE_Append(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element);

/***************************************************************************
  Function:
    void SRV_QUEUE_Append_With_Priority(SRV_QUEUE *queue, uint32_t priority,
                                        SRV_QUEUE_ELEMENT *element)

  Summary:
    Appends an element into a priority queue.

  Description:
    This function appends an element into a priority queue.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue          - Pointer to the queue where to append the element.
    priority       - Priority of the element to be appended.
    element        - Pointer to the element to be appended.

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct node_info
    {
        SRV_QUEUE_ELEMENT queueElement;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static NODE_INFO nodeInfo;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_PRIORITY);
    memset(nodeInfo.macAddress, 0xFF, 8);
    SRV_QUEUE_Append_With_Priority(&nodeQueue, 3, &nodeInfo.queueElement);
    </code>

  Remarks:
    None.
*/
void SRV_QUEUE_Append_With_Priority(SRV_QUEUE *queue, uint32_t priority,
                                    SRV_QUEUE_ELEMENT *element);

/***************************************************************************
  Function:
    void SRV_QUEUE_Insert_Before(SRV_QUEUE *queue,
                                 SRV_QUEUE_ELEMENT *currentElement,
                                 SRV_QUEUE_ELEMENT *element)

  Summary:
    Inserts an element into a queue before a given element.

  Description:
    This function inserts an element into a queue before a given element.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue          - Pointer to the queue where to insert the element.
    currentElement - Pointer to an element in the queue.
    element        - Pointer to the element to be inserted.

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static NODE_INFO nodeInfo1;
    static NODE_INFO nodeInfo2;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo1.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo1);
    memset(nodeInfo2.macAddress, 0xEE, 8);
    SRV_QUEUE_Insert_Before(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo1,
                            (SRV_QUEUE_ELEMENT *)&nodeInfo2);
    </code>

  Remarks:
    None.
*/
void SRV_QUEUE_Insert_Before(SRV_QUEUE *queue,
                             SRV_QUEUE_ELEMENT *currentElement,
                             SRV_QUEUE_ELEMENT *element);

/***************************************************************************
  Function:
    void SRV_QUEUE_Insert_After(SRV_QUEUE *queue,
                                SRV_QUEUE_ELEMENT *currentElement,
                                SRV_QUEUE_ELEMENT *element)

  Summary:
    Inserts an element into a queue after a given element.

  Description:
    This function inserts an element into a queue after a given element.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue          - Pointer to the queue where to insert the element.
    currentElement - Pointer to an element in the queue.
    element        - Pointer to the element to be inserted.

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static NODE_INFO nodeInfo1;
    static NODE_INFO nodeInfo2;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo1.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo1);
    memset(nodeInfo2.macAddress, 0xEE, 8);
    SRV_QUEUE_Insert_After(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo1,
                           (SRV_QUEUE_ELEMENT *)&nodeInfo2);
    </code>

  Remarks:
    None.
*/
void SRV_QUEUE_Insert_After(SRV_QUEUE *queue,
                            SRV_QUEUE_ELEMENT *currentElement,
                            SRV_QUEUE_ELEMENT *element);

/***************************************************************************
  Function:
    SRV_QUEUE_ELEMENT *SRV_QUEUE_Read_Or_Remove(SRV_QUEUE *queue,
                                                SRV_QUEUE_MODE accessMode,
                                                SRV_QUEUE_POSITION position)

  Summary:
    Reads or removes an element from a queue.

  Description:
    This function reads or removes an element from a queue.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue      - Pointer to the queue from which the element must be read or
                 removed.
    accessMode - Access mode (read or remove).
    position   - Position in the queue to read or remove (head or tail).

  Returns:
    In case of remove, the element will be removed from queue and returned.
    In case of read, the element will be returned without removing it from
    the queue.
    If the queue is empty, NULL is returned.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static NODE_INFO nodeInfo;
    NODE_INFO *removedNode;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo);
    removedNode = SRV_QUEUE_Read_Or_Remove(&nodeQueue, SRV_QUEUE_MODE_REMOVE,
                                           SRV_QUEUE_POSITION_HEAD);
    </code>

  Remarks:
    None.
*/
SRV_QUEUE_ELEMENT *SRV_QUEUE_Read_Or_Remove(SRV_QUEUE *queue,
                                            SRV_QUEUE_MODE accessMode,
                                            SRV_QUEUE_POSITION position);

/***************************************************************************
  Function:
    SRV_QUEUE_ELEMENT *SRV_QUEUE_Read_Element(SRV_QUEUE *queue,
                                              uint16_t elementIndex)

  Summary:
    Reads an element from a queue at the given index.

  Description:
    This function reads an element from a queue at the given index.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue        - Pointer to the queue from which the element must be read.
    elementIndex - Position of the element in the queue.

  Returns:
    Pointer to the queue element.
    If the queue is empty, NULL is returned.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static NODE_INFO nodeInfo1;
    static NODE_INFO nodeInfo2;
    NODE_INFO *nodeInfo3;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo1.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo1);
    memset(nodeInfo2.macAddress, 0xEE, 8);
    SRV_QUEUE_Insert_After(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo1,
                           (SRV_QUEUE_ELEMENT *)&nodeInfo2);
    nodeInfo3 = SRV_QUEUE_Read_Element(&nodeQueue, 2);
    </code>

  Remarks:
    None.
*/
SRV_QUEUE_ELEMENT *SRV_QUEUE_Read_Element(SRV_QUEUE *queue,
                                          uint16_t elementIndex);

/***************************************************************************
  Function:
    void SRV_QUEUE_Remove_Element(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element)

  Summary:
    Removes the given element from a queue.

  Description:
    This function removes a given element from a queue.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue   - Pointer to the queue from which the element must be removed.
    element - Element to be removed.

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static NODE_INFO nodeInfo;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo);
    SRV_QUEUE_Remove_Element(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo);
    </code>

  Remarks:
    None.
*/
void SRV_QUEUE_Remove_Element(SRV_QUEUE *queue, SRV_QUEUE_ELEMENT *element);

/***************************************************************************
  Function:
    void SRV_QUEUE_Flush(SRV_QUEUE *queue)

  Summary:
    Flushes the given queue.

  Description:
    This function flushes the given queue.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue          - Pointer to the queue to be flushed.

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static node_info_t nodeInfo;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo);
    SRV_QUEUE_Flush(&nodeQueue);
    </code>

  Remarks:
    None.
*/
void SRV_QUEUE_Flush(SRV_QUEUE *queue);

/***************************************************************************
  Function:
    void SRV_QUEUE_Set_Capacity(SRV_QUEUE *queue, uint16_t capacity)

  Summary:
    Modifies the total capacity of a queue.

  Description:
    This function modifies the total capacity of a queue.

  Precondition:
    The queue must have been initialized previously with
    function SRV_QUEUE_Init.

  Parameters:
    queue          - Pointer to the queue.
    capacity       - New capacity of the queue.

  Returns:
    None.

  Example:
    <code>
    #define NUM_MAX_NODES    750
    typedef struct _node_info_tag
    {
        struct _node_info_tag *prev;
        struct _node_info_tag *next;
        uint8_t macAddress[8]
    } NODE_INFO;

    static SRV_QUEUE nodeQueue;
    static node_info_t nodeInfo;

    SRV_QUEUE_Init(&nodeQueue, NUM_MAX_NODES, SRV_QUEUE_TYPE_SINGLE);
    memset(nodeInfo.macAddress, 0xFF, 8);
    SRV_QUEUE_Append(&nodeQueue, (SRV_QUEUE_ELEMENT *)&nodeInfo);
    SRV_QUEUE_Set_Capacity(&nodeQueue, (NUM_MAX_NODES + 10));
    </code>

  Remarks:
    None.
*/
void SRV_QUEUE_Set_Capacity(SRV_QUEUE *queue, uint16_t capacity);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif
// DOM-IGNORE-END

#endif /* SRV_QUEUE_H */
