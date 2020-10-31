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
// MessageId: MSG_DRIVER_ENTRY
//
// MessageText:
//
// Entered DriverEntry
//
#define MSG_DRIVER_ENTRY                 0x40000001L

//
// MessageId: MSG_CREATING
//
// MessageText:
//
// Creating device
//
#define MSG_CREATING                     0x40000002L

//
// MessageId: MSG_CREATE_FAILED
//
// MessageText:
//
// Device not created
//
#define MSG_CREATE_FAILED                0x40000003L

//
// MessageId: MSG_CREATE_SUCCESS
//
// MessageText:
//
// Device created
//
#define MSG_CREATE_SUCCESS               0x40000004L

//
// MessageId: MSG_CREATE
//
// MessageText:
//
// Entered OnCreate
//
#define MSG_CREATE                       0x40000005L

//
// MessageId: MSG_CLOSE
//
// MessageText:
//
// Entered OnClose
//
#define MSG_CLOSE                        0x40000006L

//
// MessageId: MSG_MOCKED
//
// MessageText:
//
// Entered mocked dispatch
//
#define MSG_MOCKED                       0x40000007L

//
// MessageId: MSG_CTL
//
// MessageText:
//
// Entered OnDeviceControl
//
#define MSG_CTL                          0x40000008L

//
// MessageId: MSG_MOCKED_CREATE
//
// MessageText:
//
// Creating mocked device
//
#define MSG_MOCKED_CREATE                0x40000009L

//
// MessageId: MSG_MOCKED_DELETE
//
// MessageText:
//
// Deleting mocked device
//
#define MSG_MOCKED_DELETE                0x4000000AL

