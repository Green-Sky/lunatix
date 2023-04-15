
## Spec for NGC Group Files

   40000 max bytes length for custom lossless NGC packets.

   37000 max bytes length for file and header, to leave some space for offline message syncing.


| what      | Length in bytes| Contents                                           |
|------     |--------        |------------------                                  |
| magic     |       6        |  0x667788113435                                    |
| version   |       1        |  0x01                                              |
| pkt id    |       1        |  0x11                                              |
| msg id    |      32        | *uint8_t  to uniquely identify the message, obtain with `tox_messagev3_get_new_message_id()`         |
| create ts |       4        |  uint32_t  unixtimestamp in UTC of local wall clock (in bigendian) |
| filename  |     255        |  *uint8_t  len TOX_MAX_FILENAME_LENGTH,  data first, then pad with NULL bytes          |
| data      |[1, 36701]      |  *uint8_t  bytes of file data, zero length files not allowed!|


header size: 299 bytes

data   size: 1 - 36701 bytes

## Send files to NGC Groups

to send files (mostly images) to an NGC group send a *uint8_t buffer (see spec above) to the group with:

`tox_group_send_custom_packet()` and specifiy `lossless` paramter as `true`
