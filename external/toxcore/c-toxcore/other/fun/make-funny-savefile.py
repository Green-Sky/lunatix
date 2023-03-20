#!/usr/bin/python
"""
Generate a new (and empty) save file with predefined keys. Used to play
with externally generated keys.

(c) 2015 Alexandre Erwin Ittner

Distributed under the GNU GPL v3 or later, WITHOUT ANY WARRANTY. See the
file "LICENSE.md" for license information.


Usage:

  ./make-funny-savefile.py <public key> <private key> <user name> <out file>

  The new user profile will be saved to <out file>.

  The keys must be an hex-encoded valid key pair generated by any means
  (eg. strkey.c); username may be anything. A random nospam value will be
  generated.

  Once the savefile is done, load it in your favorite client to get some
  DHT nodes, set status and status messages, add friends, etc.


Example (of course, do not try using this key for anything real):

  ./make-funny-savefile.py 123411DC8B1A4760B648E0C7243B65F01069E4858F45C612CE1A6F673B603830 CC39440CFC063E4A95B7F2FB2580210558BE5C073AFC1C9604D431CCA3132238 "Test user" test.tox


"""
import os
import struct
import sys

PUBLIC_KEY_LENGTH = 32
PRIVATE_KEY_LENGTH = 32

# Constants taken from messenger.c
MESSENGER_STATE_COOKIE_GLOBAL = 0x15ED1B1F
MESSENGER_STATE_COOKIE_TYPE = 0x01CE
MESSENGER_STATE_TYPE_NOSPAMKEYS = 1
MESSENGER_STATE_TYPE_DHT = 2
MESSENGER_STATE_TYPE_FRIENDS = 3
MESSENGER_STATE_TYPE_NAME = 4
MESSENGER_STATE_TYPE_STATUSMESSAGE = 5
MESSENGER_STATE_TYPE_STATUS = 6
MESSENGER_STATE_TYPE_TCP_RELAY = 10
MESSENGER_STATE_TYPE_PATH_NODE = 11

STATUS_MESSAGE = "New user".encode("utf-8")


def abort(msg: str) -> None:
    print(msg)
    exit(1)


if len(sys.argv) != 5:
    abort("Usage: %s <public key> <private key> <user name> <out file>" %
          (sys.argv[0]))

try:
    public_key = bytes.fromhex(sys.argv[1])
except:
    abort("Bad public key")

try:
    private_key = bytes.fromhex(sys.argv[2])
except:
    abort("Bad private key")

if len(public_key) != PUBLIC_KEY_LENGTH:
    abort("Public key with wrong length")

if len(private_key) != PRIVATE_KEY_LENGTH:
    abort("Private key with wrong length")

user_name = sys.argv[3].encode("utf-8")

if len(user_name) > 32:
    abort("User name too long (for this script, at least)")

out_file_name = sys.argv[4]
nospam = os.urandom(4)


def make_subheader(h_type: int, h_length: int) -> bytes:
    return (struct.pack("<I", h_length) + struct.pack("<H", h_type) +
            struct.pack("<H", MESSENGER_STATE_COOKIE_TYPE))


data = (
    # Main header
    struct.pack("<I", 0) + struct.pack("<I", MESSENGER_STATE_COOKIE_GLOBAL) +
    # Keys
    make_subheader(
        MESSENGER_STATE_TYPE_NOSPAMKEYS,
        len(nospam) + PUBLIC_KEY_LENGTH + PRIVATE_KEY_LENGTH,
    ) + nospam + public_key + private_key +
    # Name (not really needed, but helps)
    make_subheader(MESSENGER_STATE_TYPE_NAME, len(user_name)) + user_name +
    # Status message (not really needed, but helps)
    make_subheader(MESSENGER_STATE_TYPE_STATUSMESSAGE, len(STATUS_MESSAGE)) +
    STATUS_MESSAGE)

try:
    with open(out_file_name, "wb") as fp:
        fp.write(data)
except Exception as e:
    abort(str(e))