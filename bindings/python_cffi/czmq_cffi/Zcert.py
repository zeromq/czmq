################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
from .utils import *
from . import native
from . import destructors
libczmq = native.lib
libczmq_destructors = destructors.lib
ffi = native.ffi

class Zcert(object):
    """
    work with CURVE security certificates
    """

    def __init__(self):
        """
        Create and initialize a new certificate in memory
        """
        p = libczmq.zcert_new(self._p)
        if p == ffi.NULL:
            raise MemoryError("Could not allocate person")

        # ffi.gc returns a copy of the cdata object which will have the
        # destructor called when the Python object is GC'd:
        # https://cffi.readthedocs.org/en/latest/using.html#ffi-interface
        self._p = ffi.gc(p, libczmq_destructors.zcert_destroy_py)

    def public_key(self):
        """
        Return public part of key pair as 32-byte binary string
        """
        return libczmq.zcert_public_key(self._p)

    def secret_key(self):
        """
        Return secret part of key pair as 32-byte binary string
        """
        return libczmq.zcert_secret_key(self._p)

    def public_txt(self):
        """
        Return public part of key pair as Z85 armored string
        """
        return libczmq.zcert_public_txt(self._p)

    def secret_txt(self):
        """
        Return secret part of key pair as Z85 armored string
        """
        return libczmq.zcert_secret_txt(self._p)

    def set_meta(self, name, format, ):
        """
        Set certificate metadata from formatted string.
        """
        return libczmq.zcert_set_meta(self._p, to_bytes(name), format, )

    def unset_meta(self, name):
        """
        Unset certificate metadata.
        """
        return libczmq.zcert_unset_meta(self._p, to_bytes(name))

    def meta(self, name):
        """
        Get metadata value from certificate; if the metadata value doesn't
        exist, returns NULL.
        """
        return libczmq.zcert_meta(self._p, to_bytes(name))

    def meta_keys(self):
        """
        Get list of metadata fields from certificate. Caller is responsible for
        destroying list. Caller should not modify the values of list items.
        """
        return libczmq.zcert_meta_keys(self._p)

    def save(self, filename):
        """
        Save full certificate (public + secret) to file for persistent storage
        This creates one public file and one secret file (filename + "_secret").
        """
        return libczmq.zcert_save(self._p, to_bytes(filename))

    def save_public(self, filename):
        """
        Save public certificate only to file for persistent storage
        """
        return libczmq.zcert_save_public(self._p, to_bytes(filename))

    def save_secret(self, filename):
        """
        Save secret certificate only to file for persistent storage
        """
        return libczmq.zcert_save_secret(self._p, to_bytes(filename))

    def apply(self, socket):
        """
        Apply certificate to socket, i.e. use for CURVE security on socket.
        If certificate was loaded from public file, the secret key will be
        undefined, and this certificate will not work successfully.
        """
        return libczmq.zcert_apply(self._p, socket._p)

    def dup(self):
        """
        Return copy of certificate; if certificate is NULL or we exhausted
        heap memory, returns NULL.
        """
        return libczmq.zcert_dup(self._p)

    def eq(self, compare):
        """
        Return true if two certificates have the same keys
        """
        return libczmq.zcert_eq(self._p, compare._p)

    def print_py(self):
        """
        Print certificate contents to stdout
        """
        return libczmq.zcert_print(self._p)

    def test(verbose):
        """
        Self test of this class
        """
        return libczmq.zcert_test(verbose)

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
