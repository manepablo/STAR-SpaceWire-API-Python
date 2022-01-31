// ##############  constant ##############
#define STAR_STR_MAX_LEN 256

// ##############  typedefs for custom variables ##############
typedef signed char           S8;
typedef char                  U8;
typedef short                 S16;
typedef unsigned short        U16;
typedef int                   S32;
typedef unsigned int          U32;
typedef long long             S64;
typedef unsigned long long    U64;
typedef U32                   CFG_INFO_ID_TYPE;
typedef CFG_INFO_ID_TYPE STAR_DEVICE_ID;
typedef CFG_INFO_ID_TYPE STAR_CHANNEL_ID;
typedef void STAR_TRANSFER_OPERATION;

// ##############  enumerator objects ##############
typedef enum
{
    /** Channel may be used to receive traffic */
    STAR_CHANNEL_DIRECTION_IN    = 1,
    /** Channel may be used to transmit traffic */
    STAR_CHANNEL_DIRECTION_OUT   = 2,
    /** Channel may be used to both receive and transmit traffic */
    STAR_CHANNEL_DIRECTION_INOUT = STAR_CHANNEL_DIRECTION_IN |
                                        STAR_CHANNEL_DIRECTION_OUT
}STAR_CHANNEL_DIRECTION;

typedef enum
{
    /** Packet, see #STAR_SPACEWIRE_PACKET */
    STAR_STREAM_ITEM_TYPE_SPACEWIRE_PACKET,

    /** Time-code, see: #STAR_TIMECODE*/
    STAR_STREAM_ITEM_TYPE_TIMECODE,

    /**
        * A change in the state of a link, see: #STAR_LINK_STATE_EVENT
        *
        * \versionAdded \ref changes_v2_0_beta_1
        */
    STAR_STREAM_ITEM_TYPE_LINK_STATE_EVENT,

    /** Contiguous chunk of data within a single packet, see: #STAR_DATA_CHUNK */
    STAR_STREAM_ITEM_TYPE_DATA_CHUNK,

    /**
        * A change in the speed of a link, see: #STAR_LINK_SPEED_EVENT
        *
        * \versionAdded \ref changes_v2_0_beta_1
        */
    STAR_STREAM_ITEM_TYPE_LINK_SPEED_EVENT,

    /** An error injection control word, see: #STAR_ERROR_IN_DATA_INJECT */
    STAR_STREAM_ITEM_TYPE_ERROR_INJECT,

    /**
        * Timestamp of the last data received on the link, see:
        * #STAR_TIMESTAMP_EVENT
        *
        * \versionAdded \ref changes_v3_6
        */
    STAR_STREAM_ITEM_TYPE_TIMESTAMP_EVENT,

    /**
        * SpaceFibre broadcast message, see: #STAR_BROADCAST_MESSAGE
        *
        * \versionAdded \ref changes_v4_00
        */
    STAR_STREAM_ITEM_TYPE_BROADCAST_MESSAGE

} STAR_STREAM_ITEM_TYPE;

typedef enum  {

    /** Error occurred determining EOP type */
    STAR_EOP_TYPE_INVALID,
    /** End of Packet marker*/
    STAR_EOP_TYPE_EOP,
    /** Error End of Packet marker*/
    STAR_EOP_TYPE_EEP,
    /** No End of Packet marker present */
    STAR_EOP_TYPE_NONE

} STAR_EOP_TYPE;

typedef enum star_api_transfer_status
{
    /**
        * Not yet started.  When a transfer operation is created, this will be its
        * status.
        */
    STAR_TRANSFER_STATUS_NOT_STARTED,

    /**
        * Transfer has begun.  When a transfer operation is submitted, this will be
        * its status, until it has completed. */
    STAR_TRANSFER_STATUS_STARTED,

    /**
        * Transfer has completed.  Once a transmit operation has successfully
        * transmitted all its traffic, or a receive operation has received all its
        * requested traffic, this will be its status.
        */
    STAR_TRANSFER_STATUS_COMPLETE,

    /**
        * Transfer was cancelled.  When a transfer is cancelled by calling
        * STAR_cancelTransferOperation(), this will be its status.
        */
    STAR_TRANSFER_STATUS_CANCELLED,

    /**
        * An error occurred while processing the transfer.  This will be the status
        * of a transfer operation if there was an error creating it, submitting it,
        * or there was an error while transmitting or receiving.
        */
    STAR_TRANSFER_STATUS_ERROR

} STAR_TRANSFER_STATUS;

/**
 * \ingroup transfers
 *
 * Flags used to determine what kind of stream items
 * to receive on a receive operation.
 *
 * \versionAdded \ref changes_v0_8
 */
typedef enum
{
    /** #STAR_SPACEWIRE_PACKET  */
    STAR_RECEIVE_PACKETS            = 1U << 0,

    /** #STAR_DATA_CHUNK */
    STAR_RECEIVE_CHUNKS             = 1U << 1,

    /** #STAR_TIMECODE */
    STAR_RECEIVE_TIMECODES          = 1U << 2,

    /**
     * Flow control characters. Note that transmitting and receiving FCTs is not
     * currently supported.
     */
    STAR_RECEIVE_FCTS               = 1U << 3,

    /**
     * Null characters. Note that transmitting and receiving Nulls is not
     * currently supported.
     */
    STAR_RECEIVE_NULL               = 1U << 4,

    /**
     * #STAR_LINK_STATE_EVENT
     *
     * \versionAdded \ref changes_v2_0_beta_1
     */
    STAR_RECEIVE_LINK_STATE_EVENTS  = 1U << 5,

    /**
     * #STAR_LINK_SPEED_EVENT
     *
     * \versionAdded \ref changes_v2_0_beta_1
     */
    STAR_RECEIVE_LINK_SPEED_EVENTS  = 1U << 6,

    /**
     * #STAR_TIMESTAMP_EVENT
     *
     * \versionAdded \ref changes_v3_6
     */
    STAR_RECEIVE_TIMESTAMP_EVENTS   = 1U << 7,

    /**
     * #STAR_BROADCAST_MESSAGE
     *
     * \versionAdded \ref changes_v4_00
     */
    STAR_RECEIVE_BROADCAST_MESSAGES   = 1U << 8
} STAR_RECEIVE_MASK;


// ##############  typedefs for custom structs ##############
/** A structure used to store version information for a module. */
typedef struct
{
    /** The name of the module. */
    char name[STAR_STR_MAX_LEN];

    /** The author of the module. */
    char author[STAR_STR_MAX_LEN];

    /** The major version number of this module. */
    U16 major;

    /** The minor version number of this module. */
    U16 minor;

    /** The edit number of this module. */
    U16 edit;

    /** The patch number of this module. Edit will be 0 if patch is non zero */
    U16 patch;

} STAR_VERSION_INFO;

typedef struct STAR_STREAM_ITEM
{
    /** The type of stream item pointed to by item */
    STAR_STREAM_ITEM_TYPE itemType;

    /** A stream item object */
    void *item;

    /**
        * \internal
        * The receive operation that the stream item was received by, or NULL if it
        * wasn't received by a receive operation.
        */
    void *pReceivedOperation;

    /**
        * \internal
        * The next stream item in the list of stream items, if this is a receive
        * operation stream item.
        */
    struct STAR_STREAM_ITEM *pNext;

} STAR_STREAM_ITEM;
typedef struct
{
    /** Array of SpaceWire path address elements */
    U8* pPath;
    /** The length of the address */
    U16 pathLength;

} STAR_SPACEWIRE_ADDRESS;

typedef struct
{
    /** The packets address (optional)
    \note This member is provided for convenience only. There is
          no difference between specifying a packet with an address of [ \c 0xFE, \c 0xAB ]
          and data of [ \c 0x01, \c 0x02, \c 0x03, \c 0x04 ], and specifying a packet with no
          explicit address, and data of [ \c 0xFE, \c 0xAB, \c 0x01, \c 0x02, \c 0x03, \c 0x04 ] */
    STAR_SPACEWIRE_ADDRESS* address;
    /** The data that makes up the packet */
    void* dataChunks;

} STAR_SPACEWIRE_PACKET;

// ############## function prototype defines ##############
STAR_VERSION_INFO* STAR_getApiVersion(void);
STAR_DEVICE_ID* STAR_getDeviceList(U32 * count);
char* STAR_getDeviceSerialNumber(STAR_DEVICE_ID deviceId );
char* STAR_getDeviceName(STAR_DEVICE_ID deviceId );
U32 STAR_getDeviceChannels(STAR_DEVICE_ID deviceId);
STAR_CHANNEL_ID STAR_openChannelToLocalDevice(STAR_DEVICE_ID device, STAR_CHANNEL_DIRECTION  direction, unsigned char channelNumber, int isQueued);
STAR_STREAM_ITEM* STAR_createPacket(STAR_SPACEWIRE_ADDRESS * address, unsigned char * data, unsigned int dataLen, STAR_EOP_TYPE eopType);
STAR_TRANSFER_OPERATION* STAR_createTxOperation(STAR_STREAM_ITEM ** ppItems, unsigned int streamItemCount);
int STAR_submitTransferOperation(STAR_CHANNEL_ID channelId, STAR_TRANSFER_OPERATION * transferOp);
STAR_TRANSFER_STATUS STAR_waitOnTransferOperationCompletion(STAR_TRANSFER_OPERATION *   pTransferOperation, int timeout);
int STAR_disposeTransferOperation(STAR_TRANSFER_OPERATION * pTransferOperation);
STAR_TRANSFER_OPERATION* STAR_createRxOperation(int itemCount, STAR_RECEIVE_MASK mask);
STAR_STREAM_ITEM* STAR_getTransferItem(STAR_TRANSFER_OPERATION * pReceiveOperation, unsigned int index);
unsigned int STAR_getTransferItemCount(STAR_TRANSFER_OPERATION * pReceiveOperation);
unsigned char* STAR_getPacketData(STAR_SPACEWIRE_PACKET * packet, unsigned int * dataLength);
int STAR_closeChannel(STAR_CHANNEL_ID channelId);
void* RMAP_BuildWriteCommandPacket(U8 *pTargetAddress, unsigned long targetAddressLength, U8 *pReplyAddress, unsigned long replyAddressLength, char verifyBeforeWrite, char acknowledge, char incrementAddress, U8 key, U16 transactionIdentifier, U32 writeAddress, U8 extendedWriteAddress, U8 *pData, U32 dataLength, unsigned long *pRawPacketLength, void *pPacketStruct, char  alignment);
