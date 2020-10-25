//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: MSG_CREATE_BUFFERED
//
// MessageText:
//
// Creating BufferedIO device
//
#define MSG_CREATE_BUFFERED              0x40000001L

//
// MessageId: MSG_CREATE_BUFFERED_SUCCESS
//
// MessageText:
//
// BufferedIO device created successfully
//
#define MSG_CREATE_BUFFERED_SUCCESS      0x40000002L

//
// MessageId: MSG_CREATE_BUFFERED_FAILED
//
// MessageText:
//
// BufferedIO device not created
//
#define MSG_CREATE_BUFFERED_FAILED       0x40000003L

//
// MessageId: MSG_CREATE_DIRECT
//
// MessageText:
//
// Creating DirectIO device
//
#define MSG_CREATE_DIRECT                0x40000004L

//
// MessageId: MSG_CREATE_DIRECT_SUCCESS
//
// MessageText:
//
// DirectIO device created successfully
//
#define MSG_CREATE_DIRECT_SUCCESS        0x40000005L

//
// MessageId: MSG_CREATE_DIRECT_FAILED
//
// MessageText:
//
// DirectIO device not created
//
#define MSG_CREATE_DIRECT_FAILED         0x40000006L

//
// MessageId: MSG_CREATE_NEITHER
//
// MessageText:
//
// Creating NEITHERIO device
//
#define MSG_CREATE_NEITHER               0x40000007L

//
// MessageId: MSG_CREATE_NEITHER_SUCCESS
//
// MessageText:
//
// NEITHERIO device created successfully
//
#define MSG_CREATE_NEITHER_SUCCESS       0x40000008L

//
// MessageId: MSG_CREATE_NEITHER_FAILED
//
// MessageText:
//
// NEITHERIO device not created
//
#define MSG_CREATE_NEITHER_FAILED        0x40000009L

//
// MessageId: MSG_DRIVER_ENTRY
//
// MessageText:
//
// Driver Entry
//
#define MSG_DRIVER_ENTRY                 0x4000000AL

//
// MessageId: MSG_CREATE
//
// MessageText:
//
// Entering OnCreate
//
#define MSG_CREATE                       0x4000000BL

//
// MessageId: MSG_CLOSE
//
// MessageText:
//
// Entering OnClose
//
#define MSG_CLOSE                        0x4000000CL

//
// MessageId: MSG_READ
//
// MessageText:
//
// Entering OnRead
//
#define MSG_READ                         0x4000000DL

//
// MessageId: MSG_BUFFERED_READ
//
// MessageText:
//
// Entering BufferedIO read
//
#define MSG_BUFFERED_READ                0x4000000EL

//
// MessageId: MSG_BUFFERED_BUFFER_EMPTY
//
// MessageText:
//
// BufferedIO driver buffer empty
//
#define MSG_BUFFERED_BUFFER_EMPTY        0x4000000FL

//
// MessageId: MSG_DIRECT_READ
//
// MessageText:
//
// Entering DIRECTIO read
//
#define MSG_DIRECT_READ                  0x40000010L

//
// MessageId: MSG_DIRECT_BUFFER_EMPTY
//
// MessageText:
//
// DIRECTIO driver buffer empty
//
#define MSG_DIRECT_BUFFER_EMPTY          0x40000011L

//
// MessageId: MSG_NEITHER_READ
//
// MessageText:
//
// Entering NEITHERIO read
//
#define MSG_NEITHER_READ                 0x40000012L

//
// MessageId: MSG_NEITHER_BUFFER_EMPTY
//
// MessageText:
//
// NEITHERIO driver buffer empty
//
#define MSG_NEITHER_BUFFER_EMPTY         0x40000013L

//
// MessageId: MSG_DIRECT_USER_BUFFER_EMPTY
//
// MessageText:
//
// MmGetSystemAddressForMdlSafe failed
//
#define MSG_DIRECT_USER_BUFFER_EMPTY     0x40000014L

//
// MessageId: MSG_WRITE
//
// MessageText:
//
// Entering OnWrite
//
#define MSG_WRITE                        0x40000015L

//
// MessageId: MSG_BUFFERED_WRITE
//
// MessageText:
//
// Entering BufferedIO write
//
#define MSG_BUFFERED_WRITE               0x40000016L

//
// MessageId: MSG_DIRECT_WRITE
//
// MessageText:
//
// Entering DIRECTIO write
//
#define MSG_DIRECT_WRITE                 0x40000017L

//
// MessageId: MSG_NEITHER_WRITE
//
// MessageText:
//
// Entering NEITHERIO write
//
#define MSG_NEITHER_WRITE                0x40000018L

//
// MessageId: MSG_DIRECT_READ_MDL_EMPTY
//
// MessageText:
//
// MDL supplied in IRP is null
//
#define MSG_DIRECT_READ_MDL_EMPTY        0x40000019L

