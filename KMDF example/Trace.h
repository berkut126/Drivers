#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(KmdfExample,(f0cc34b3,a482,4dc0,b978,b5cf42aec4fd), \
    WPP_DEFINE_BIT(DBG_INIT)                                                    \
    WPP_DEFINE_BIT(DBG_CREATE)                                                  \
    WPP_DEFINE_BIT(DBG_QUEUE_CREATE)                                            \
    WPP_DEFINE_BIT(DBG_READ_OUTPUT_FAIL)                                        \
    WPP_DEFINE_BIT(DBG_READ_INPUT_FAIL)                                         \
    WPP_DEFINE_BIT(DBG_WRITE_OUTPUT_FAIL)                                       \
    WPP_DEFINE_BIT(DBG_WRITE_INPUT_FAIL)                                        \
)
// This comment block is scanned by the trace preprocessor to define our
// TraceEvents function.
//
// begin_wpp config
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp